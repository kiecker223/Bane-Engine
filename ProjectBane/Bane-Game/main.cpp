#include "Engine/Engine.h"
#include "CameraMovementComponent.h"
#include "Code/RayTracingRenderer.h"
#include <memory>

class TestComponent2 : public Component
{

	IMPLEMENT_COMPONENT(TestComponent2)

public:

	TestComponent2() { }

	virtual void Tick(double DT) override final
	{
		GetOwner()->GetTransform()->Translate(0.0f, 0.0f, -0.01f);
		GetOwner()->GetTransform()->Rotate(0.0f, 0.01f, 0.0f);
	}

	virtual void Start() override final
	{
		BaneLog() << "Eyy";
	}

};

class TestComponent3 : public Component
{
	IMPLEMENT_COMPONENT(TestComponent3)
public:
	TestComponent3() { }

	virtual void Tick(double DT) override final
	{
		GetOwner()->GetTransform()->Rotate(0.0f, 0.01f, 0.0f);
	}
};

class TestComponent4 : public Component
{
	IMPLEMENT_COMPONENT(TestComponent4)
public:
	TestComponent4() { }

	virtual void Tick(double DT) override final
	{
		GetOwner()->GetTransform()->Rotate(0.01f, 0.01f, 0.01f);
	}

};

class TestComponent5 : public Component
{
	IMPLEMENT_COMPONENT(TestComponent5)
public:

	LightComponent* LightComp;

	TestComponent5() { }

	void Start()
	{
		LightComp = GetOwner()->GetComponent<LightComponent>();
	}

	float Frame = 0.0f;
	virtual void Tick(double DT) override final
	{
		Frame += 1.f;
		GetOwner()->GetTransform()->Translate(0.0f, 0.0f, sinf(Frame));
	}
};

class TestComponent6 : public Component
{
	IMPLEMENT_COMPONENT(TestComponent6)
public:
	LightComponent* LightComp;

	void Start() override final
	{
		LightComp = GetOwner()->GetComponent<LightComponent>();
	}

	float Frame = 0.0f;

	void Tick(double DT) override final
	{
		Frame++;
		LightComp->Intensity = abs(sinf(Frame / 100.f)) * 40.f;
		LightComp->Range = abs(sinf(Frame / 200.f)) * 30.0f;
	}


};

class TestComponent7 : public Component
{
	IMPLEMENT_COMPONENT(TestComponent7)
public:

	MeshRenderingComponent* MeshComp;

	void Start() override final
	{
		MeshComp = GetOwner()->GetComponent<MeshRenderingComponent>();
	}

	float Frame = 0.0f;

	void Tick(double DT) override final
	{
		Frame++;
		MATERIAL_PARAMETERS& Params = MeshComp->GetMaterialParameters();
		Params.Roughness = sinf(Frame) * 0.5f + 0.5f;
		Params.SpecularFactor = 1.0f - Params.Roughness;
	}

};


void InitApplication()
{
	/*
	// Mesh
	Entity* entity = GetEntityManager()->CreateEntity("TestEntity");
	//entity->AddComponent<TestComponent3>();
	{
		auto* mrc = entity->AddComponent<MeshRenderingComponent>();
		entity->AddComponent<TestComponent7>();
		mrc->RenderedMesh.LoadFromFile("plane.obj");
		mrc->RenderedMaterial.LoadFromFile("Shaders/TestRenderingShader.hlsl");
		auto& Mat = mrc->RenderedMaterial;
		Mat.SetDiffuseTexture("Resources/AbstractTex.jpg");
		Mat.SetNormalTexture("Resources/brickwork-normal.jpg");
		Mat.SetTexture(GetTextureCache()->GetDefaultWhiteTexture(), 2);
		entity->GetTransform()->Translate(0.0f, -10.0f, 5.0f);
		entity->GetTransform()->Scale(14.f);
	}
	
	//Skybox
	{
		Entity* Skybox = GetEntityManager()->CreateEntity("Skybox");
		auto* SkyboxMesh = Skybox->AddComponent<SkyboxComponent>();
		SkyboxMesh->SetSkyboxShader("Shaders/SkyboxShader.hlsl");
		SkyboxMesh->SetSkybox("TestSkybox", "nx.tga", "ny.tga", "nz.tga", "px.tga", "py.tga", "pz.tga");
	}

	{
		Entity* light = GetEntityManager()->CreateEntity("TestLight");
		auto* LightComp = light->AddComponent<LightComponent>();
		light->GetTransform()->Translate(0.0f, 0.0f, 10.0f);
		light->GetTransform()->Rotate(-90.0f, 0.0f, 0.0f);
		LightComp->Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		LightComp->Intensity = 8.0f;
		LightComp->Range = 15.0f;
		//light->AddComponent<TestComponent4>();
		LightComp->SetLightType(LIGHT_TYPE_DIRECTIONAL);
	}
	
	{
		Entity* Sphere = GetEntityManager()->CreateEntity("Sphere");
		auto* SphereMesh = Sphere->AddComponent<MeshRenderingComponent>();
		auto& SphereMat = SphereMesh->RenderedMaterial;
		SphereMesh->RenderedMesh.LoadFromFile("sphere.obj");
		SphereMesh->RenderedMaterial.LoadFromFile("Shaders/TestRenderingShader.hlsl");
		MATERIAL_PARAMETERS Parameters = { };
		Parameters.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		Parameters.SpecularFactor = 0.5f;
		Parameters.Roughness = 0.7f;
		SphereMesh->RenderedMaterial.SetMaterialParameters(Parameters);
		SphereMat.SetTexture(GetTextureCache()->GetDefaultBlueTexture(), 0);
		SphereMat.SetTexture(GetTextureCache()->GetDefaultNormal(), 1);
		SphereMat.SetTexture(GetTextureCache()->GetDefaultWhiteTexture(), 2);
		Sphere->GetTransform()->Scale(3.0f);
		//Sphere->GetTransform()->Translate(0.0f, 0.0f, -8.f);
	}

	// Light2
	{
		Entity* light2 = GetEntityManager()->CreateEntity("TestLight2");
		light2->GetTransform()->Translate(-8.0f, -8.0f, 8.0f);
		auto* LightComp2 = light2->AddComponent<LightComponent>();
		light2->AddComponent<TestComponent5>();
		LightComp2->Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		LightComp2->Intensity = 10.0f;
		LightComp2->Range = 16.0f;
		LightComp2->SetLightType(LIGHT_TYPE_POINT);
	}

	{
		Entity* Teapot = GetEntityManager()->CreateEntity("Teapot");
		auto* mrc = Teapot->AddComponent<MeshRenderingComponent>();
		auto& TeapotMesh = mrc->RenderedMesh;
		auto& TeapotMaterial = mrc->RenderedMaterial;
		MATERIAL_PARAMETERS Parameters = { };
		Parameters.Color = XMFLOAT3(0.0f, 0.0f, 1.f);
		Parameters.SpecularFactor = 32.f;
		Parameters.Roughness = 0.4f;
		TeapotMaterial.SetMaterialParameters(Parameters);
		TeapotMesh.LoadFromFile("duck.dae");
		TeapotMaterial.LoadFromFile("Shaders/TestRenderingShader.hlsl");
		TeapotMaterial.SetDiffuseTexture("MetalDiffuse.jpg");
		TeapotMaterial.SetTexture(GetTextureCache()->GetDefaultNormal(), 1);
		TeapotMaterial.SetTexture(GetTextureCache()->GetDefaultWhiteTexture(), 2);
		Teapot->GetTransform()->Translate(-15.0f, 0.0f, 0.0f);
		Teapot->GetTransform()->Scale(0.03f);
	}
	*/
	{
		Entity* camera = GetEntityManager()->CreateEntity("Camera");
		CameraComponent* CamComponent = camera->AddComponent<CameraComponent>();
		camera->GetTransform()->Translate(0.0f, 0.0f, -3.0f);
		CamComponent->SetPriority(1);
		camera->AddComponent<CameraMovementComponent>();
	}

	GetApplicationInstance()->GetSceneRenderer()->GetSwapChain()->SetSwapInterval(0);
}

void UpdateApplication()
{
}

void CleanupApplication()
{

}

int main(int argc, char** argv)
{
	Application application;
	application.SetSceneRenderer(new RayTracingRenderer());
	application.SetStartCallback(&InitApplication);
	application.SetUpdateCallback(&UpdateApplication);
	application.SetCleanupCallback(&CleanupApplication);
	application.InitSystems();
	application.Run();
	application.Shutdown();
	return 0;
}


