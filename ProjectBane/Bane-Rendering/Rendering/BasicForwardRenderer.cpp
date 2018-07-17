#include "BasicForwardRenderer.h"
#include "BaneObject/Components/CameraComponent.h"


struct FORWARD_CAMERA_CONSTANTS
{
	matrix Model;
	matrix View;
	matrix Projection;
} CameraConstants;

void BasicForwardRenderer::Initialize()
{
	ApiRuntime::CreateRuntime();
	GetApiRuntime()->Initialize();
	m_CameraConstants = GetApiRuntime()->GetGraphicsDevice()->CreateConstBuffer<FORWARD_CAMERA_CONSTANTS>();
}

void BasicForwardRenderer::Render()
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	IGraphicsCommandContext* Context = Device->GetGraphicsContext();

	Context->BeginPass(Device->GetBackBufferTargetPass());

	void* Buff = Context->Map(m_CameraConstants);
	
	for (uint i = 0; i < m_DrawList.size(); i++)
	{
		MeshDrawArgs& DrawItem = m_DrawList[i];

		CameraConstants.Model = DrawItem.OwningEntity->GetTransform()->GetMatrix();
		CameraConstants.Projection = MainCamera->GetProjection();
		CameraConstants.View = MainCamera->GetLookAt();
		memcpy(Buff, &CameraConstants, sizeof(FORWARD_CAMERA_CONSTANTS));
		
		Device->CreateShaderResourceView(DrawItem.UsedMaterial.GetTable(), m_CameraConstants, 0);
		DrawItem.UsedMaterial.Bind(Context);

		Context->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context->SetVertexBuffer(DrawItem.DrawnMesh.GetVertexBuffer());
		Context->SetIndexBuffer(DrawItem.DrawnMesh.GetIndexBuffer());
		Context->DrawIndexed(DrawItem.DrawnMesh.GetIndexCount(), 0, 0);
	}
	
	Context->Unmap(m_CameraConstants);
	Context->EndPass();
}

void BasicForwardRenderer::Present()
{
	GetApiRuntime()->GetGraphicsDevice()->GetSwapChain()->Present();
}

void BasicForwardRenderer::Shutdown()
{
	ApiRuntime::Shutdown();
}

void BasicForwardRenderer::AddBasicMesh(const Mesh& InMesh, const Material& InMaterial, Entity* Owner, IConstantBuffer* CameraCB)
{
	m_DrawList.push_back({ InMesh, InMaterial, Owner, CameraCB });
}

void BasicForwardRenderer::AddCamera(CameraComponent* InCamera)
{
	Cameras.push_back(InCamera);
	if (InCamera->GetPriority() == 1)
	{
		if (MainCamera != nullptr)
		{
			BaneLog() << "Uhh, not supposed to be another camera here!";
			return;
		}
		MainCamera = InCamera;
	}
}
