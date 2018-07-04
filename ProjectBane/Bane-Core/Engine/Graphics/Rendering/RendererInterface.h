#pragma once

#include "Core/KieckerMath.h"
#include "../Interfaces/GraphicsCommandList.h"
#include "Core/Systems/Logging/Logger.h"
#include "LightData.h"
#include "BaneObject/Entity/Entity.h"

#include <vector>

class Scene;
class Mesh;
class Material;
class IGraphicsCommandContext;
class IDeviceSwapChain;
class SkyboxComponent;
class CameraComponent;



class SceneRenderer
{
public:

	virtual void AllocateScene(Scene* pScene) { UNUSED(pScene); }

	virtual void DumpCurrentScene() { }

	virtual void Render() = 0;

	virtual void Initialize() = 0;

	virtual void Present() = 0;

	virtual void Shutdown() = 0;

	virtual Scene* GetScene() { return nullptr; }

	virtual IDeviceSwapChain* GetSwapChain() = 0;

	virtual bool SupportsAsyncContexts() { return false; }

	virtual void AddBasicMesh(const Mesh& InMesh, const Material& InMaterial, Entity* Owner, IConstantBuffer* CameraCB) { UNUSED(InMesh); UNUSED(InMaterial); UNUSED(Owner); UNUSED(CameraCB); }

	virtual void AddOpaqueObject(const Mesh& InMesh, const Material& InMaterial, Entity* Owner) { UNUSED(InMesh); UNUSED(InMaterial); UNUSED(Owner); }

	virtual void AddCamera(CameraComponent* InCamera) { UNUSED(InCamera); }

	virtual void AddLight(class LightComponent* InLight) { UNUSED(InLight); }

	virtual void SetSkybox(SkyboxComponent* InSkybox) { UNUSED(InSkybox); }

protected:
	
	CameraComponent* MainCamera = nullptr;
	std::vector<CameraComponent*> Cameras;

	SkyboxComponent* Skybox;

};
