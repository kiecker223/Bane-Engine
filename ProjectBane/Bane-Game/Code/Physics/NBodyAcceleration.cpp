#include "NBodyAcceleration.h"
#include <numeric>
#include <iostream>


const uint32 NumNodesAllocated = 65537;
const double Theta = 1.0;


void NBodyAcceleration::Construct(std::vector<PhysicsData>& InOutPhysicsData)
{

	double Infinity = std::numeric_limits<double>::infinity();
	double Min = Infinity;
	double Max = -Infinity;

	for (PhysicsData& Data : InOutPhysicsData)
	{
		for (uint32 i = 0; i < 3; i++)
		{
			if (Data.Position.p[i] < Min)
			{
				Min = Data.Position.p[i];
			}
			if (Data.Position.p[i] > Max)
			{
				Max = Data.Position.p[i];
			}
		}
	}

	Max += 20.0;
	Min -= 20.0;

	if (m_MemoryPool.GetNumBytes() == 0)
	{
		m_MemoryPool.Initialize(sizeof(NBodyNode) * NumNodesAllocated);
	}
	m_MemoryPool.Reset();

	if (!Topmost)
	{
		Topmost = m_MemoryPool.Allocate<NBodyNode>();
	}
	else
	{
		Topmost->~NBodyNode();
	}

	Topmost->Bounds.Min = vec3(Min, Min, Min);
	Topmost->Bounds.Max = vec3(Max, Max, Max);

	for (PhysicsData& Data : InOutPhysicsData)
	{
		Topmost->Bodies.push_back(&Data);
	}

	Topmost->CalculateCenterOfMass();
	Topmost->CalculateForce();
	Topmost->Parent = nullptr;

	// Add one for the current thread
	uint32 ThreadCount = Dispatcher::GetThreadCount() + 1;

	m_TreeBuilder.Initialize(ThreadCount, Topmost, m_MemoryPool, InOutPhysicsData);

	if (!m_ConstructTask)
	{
		m_ConstructTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [this](uint32 DispatchSize, uint32 DispatchIndex)
		{
			m_TreeBuilder.IndividualThreadFunc(DispatchIndex + 1);
		});
	}
	Dispatcher::DispatchTask(m_ConstructTask);
	m_TreeBuilder.IndividualThreadFunc(0);
	Dispatcher::WaitOnTask(m_ConstructTask);
}

void NBodyAcceleration::Simulate(std::vector<PhysicsData>& InOutPhysicsData, double DeltaTime)
{
	if (!m_SimulateTask)
	{
		m_SimulateTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [&InOutPhysicsData, DeltaTime, this](uint32 DispatchSize, uint32 DispatchIndex)
		{
			SimulateImpl(InOutPhysicsData, DeltaTime, DispatchSize + 1, DispatchIndex + 1);
		});
	}
	Dispatcher::DispatchTask(m_SimulateTask);
	SimulateImpl(InOutPhysicsData, DeltaTime, Dispatcher::GetThreadCount() + 1, 0);
	Dispatcher::WaitOnTask(m_SimulateTask);

	if (!m_FreeMemTask)
	{
		m_FreeMemTask = new Task(TASK_DISPATCH_ON_ALL_THREADS, [this](uint32 DispatchSize, uint32 DispatchIndex)
		{
			m_TreeBuilder.IndividualThreadFreeFunc(DispatchIndex + 1);
		});
	}

	Topmost->~NBodyNode();

	Dispatcher::DispatchTask(m_FreeMemTask);
	m_TreeBuilder.IndividualThreadFreeFunc(0);
	Dispatcher::WaitOnTask(m_FreeMemTask);

	Topmost = nullptr;
}

double CalculateTheta(const vec3& Src, const NBodyNode* Dst)
{
	return Dst->Bounds.GetExtents().x / length(Src - Dst->CenterOfMass);
}

static bool AllComponentsEqual(const vec3& Lhs, const vec3& Rhs)
{
	return Lhs.x == Rhs.x && Lhs.y == Rhs.y && Lhs.z == Rhs.z;
}

void CalculateAcceleration(PhysicsData& Dst, const vec3& RhsCenter, const double& RhsMass, double DeltaTime)
{
	if (AllComponentsEqual(RhsCenter, Dst.Position))
	{
		return;
	}

	vec3 ForceDir = RhsCenter - Dst.Position;
	double DistanceSqrd = lengthSqrd(ForceDir);
	ForceDir /= sqrt(DistanceSqrd);
	double Force = M_GRAV_CONST * (RhsMass * Dst.Mass) / (DistanceSqrd);
	if (isnan(Force)) { return; }
	double FDT = Force * DeltaTime;
	Dst.Velocity += ForceDir * FDT;
}
void CalculateForceImpl(PhysicsData& SourceBody, NBodyNode* RootNode, double DeltaTime)
{
	const auto Infinity = std::numeric_limits<double>::infinity();

	bool bHasNextNode = RootNode->NextNodes[0] != nullptr;
	double CalcTheta = Infinity;
	if (bHasNextNode)
	{
		CalcTheta = CalculateTheta(SourceBody.Position, RootNode);
		if (isnan(CalcTheta))
		{
			CalcTheta = Infinity;
		}
	}

	if (RootNode->Force == 0.0)
	{
		return;
	}
	if (CalcTheta < Theta || !bHasNextNode)
	{
		CalculateAcceleration(SourceBody, RootNode->CenterOfMass, RootNode->Force, DeltaTime);
	}
	else
	{
		for (uint32 i = 0; i < 8; i++)
		{
			CalculateForceImpl(SourceBody, RootNode->NextNodes[i], DeltaTime);
		}
	}
}



void NBodyAcceleration::SimulateImpl(std::vector<PhysicsData>& InOutPhysicsData, double DeltaTime, uint32 DispatchSize, uint32 DispatchIndex)
{
	uint32 IterationCount = static_cast<uint32>(InOutPhysicsData.size()) / DispatchSize;
	uint32 StartIndex = IterationCount * DispatchIndex;
	uint32 EndIndex = StartIndex + IterationCount;
	double Infinity = std::numeric_limits<double>::infinity();
	for (uint32 b = StartIndex; b < EndIndex; b++)
	{
		for (uint32 i = 0; i < 8; i++)
		{
			CalculateForceImpl(InOutPhysicsData[b], Topmost->NextNodes[i], DeltaTime);
		}
	}
}

void NBodyAcceleration::TreeBuilder::Initialize(uint32 InThreadCount, NBodyNode* InTopmost, LinearAllocator& ParentAllocator, std::vector<PhysicsData>& InOutPhysicsData)
{
	Topmost = InTopmost;
	pOperatingData = &InOutPhysicsData;
	ThreadCount = InThreadCount;
	if (ThreadStack == nullptr)
	{
		ThreadStack = new PerThreadData[ThreadCount];
	}

	// Potential bug, odd number of threads
	// @TODO: Need to fix that
	uint32 NumNodesAllowedToAllocatePerThread = (NumNodesAllocated - 1) / ThreadCount;

	for (uint32 i = 0; i < ThreadCount; i++)
	{
		if (!ThreadStack[i].Allocator)
		{
			ThreadStack[i].Allocator = new LinearAllocator();
		}
		uint8* OldPtr = ParentAllocator.AllocateBytes(NumNodesAllowedToAllocatePerThread * sizeof(NBodyNode));
		ThreadStack[i].Allocator->Initialize(OldPtr, ParentAllocator.GetCurrentPointer());
	}

	for (uint32 i = 0; i < 8; i++) 
	{
		BoundingBox NewRegion = Topmost->Bounds.Subdivide(i);
		NBodyNode* NewNode = ThreadStack[0].Allocator->Allocate<NBodyNode>();
		NewNode->Bounds = NewRegion;
		NewNode->Parent = Topmost;
		NewNode->ChildIndex = i;
		Topmost->NextNodes[i] = NewNode;
		ThreadStack[0].AllocatedNodes.push_back(NewNode);
		WorkQueue.SafeAdd(NewNode);
	}
}

void NBodyAcceleration::TreeBuilder::IndividualThreadFunc(uint32 ThreadIdx)
{
	uint32 NumTries = 0;
	while (true)
	{
		NBodyNode* Node = WorkQueue.Pop();
		if (!Node)
		{
			NumTries++;
			if (NumTries == 5)
			{
				return;
			}
			else
			{
				continue;
			}
		}
		else
		{
			NumTries = 0;
		}

		Node->Bounds = Node->Parent->Bounds.Subdivide(Node->ChildIndex);
		for (auto* Body : Node->Parent->Bodies)
		{
			if (Node->Bounds.PointInBounds(Body->Position))
			{
				Node->Bodies.push_back(Body);
			}
		}

		if (Node->Bodies.size() == 1)
		{
			Node->Force = Node->Bodies[0]->Mass;
			Node->CenterOfMass = Node->Bodies[0]->Position;
		
			continue; // ?????
		}
		else if (Node->Bodies.size() == 0)
		{
			// What do I do here?
			// Potentially avoid a crash so we won't let this tree continue
			continue;
		}
		else if (Node->Bodies.size() > 1)
		{
			Node->CalculateCenterOfMass();
			if (Node->CalculateForce() == 0.0)
			{
				__debugbreak();
			}
			for (uint32 i = 0; i < 8; i++)
			{
				NBodyNode* NewNode = ThreadStack[ThreadIdx].Allocator->Allocate<NBodyNode>();
				ThreadStack[ThreadIdx].AllocatedNodes.push_back(NewNode);
				NewNode->Parent = Node;
				NewNode->ChildIndex = i;
				Node->NextNodes[i] = NewNode;
				WorkQueue.Add(NewNode);
			}
		}
	}
}

void NBodyAcceleration::TreeBuilder::IndividualThreadFreeFunc(uint32 ThreadIdx)
{
	auto& Mem = ThreadStack[ThreadIdx].AllocatedNodes;

	for (uint32 i = 0; i < Mem.size(); i++)
	{
		Mem[i]->~NBodyNode();
	}
	Mem.clear();
}
