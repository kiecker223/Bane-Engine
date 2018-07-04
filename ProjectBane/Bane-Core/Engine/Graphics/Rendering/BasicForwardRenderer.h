#pragma once

#include "Engine/Graphics/Rendering/RendererInterface.h"
#include "../Interfaces/ApiRuntime.h"
#include "Mesh.h"
#include "Material.h"


class BasicForwardRenderer : public SceneRenderer
{
public:

	virtual void DumpCurrentScene() override final { }

	virtual void Initialize() override final;

	virtual void Render() override final;

	virtual void Present() override final;

	virtual void Shutdown() override final;

	virtual Scene* GetScene() override final { return nullptr; }

	virtual IDeviceSwapChain* GetSwapChain() { return nullptr; };

	virtual bool SupportsAsyncContexts() { return true; }

	virtual void AddBasicMesh(const Mesh& InMesh, const Material& InMaterial, Entity* Owner, IConstantBuffer* CameraCB) override;

	virtual void AddCamera(CameraComponent* InCamera);

private:

	struct MeshDrawArgs
	{
		Mesh DrawnMesh;
		Material UsedMaterial;
		Entity* OwningEntity; // Rip cache 
		IConstantBuffer* CameraCB;
	};

	IConstantBuffer* m_CameraConstants;
	std::vector<MeshDrawArgs> m_DrawList;
};