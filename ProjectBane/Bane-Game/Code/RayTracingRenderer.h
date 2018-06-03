#pragma once

#include "Engine/Engine.h"

/*
struct MATERIAL
{
	float3 Color;
	float Metallic;
	float Roughness;
	float P0, P1, P2;
};

struct POINT_LIGHT
{
	float3 Color;
	float P0;
	float3 Position;
	float Intensity;
};

struct DIRECTIONAL_LIGHT
{
	float3 Direction;
	float3 Color;
	float2 P0;
};

struct PLANE
{
	MATERIAL Material;
	float3 Position;
	float Size;
	float3 Normal;
	float P0;
};

struct SPHERE
{
	MATERIAL Material;
	float3 Center;
	float Radius;
};

struct BOX
{
	MATERIAL Material;
	float3 Min;
	float3 Max;
	float2 P0;
};

Resources
{
	cbuffer ScreenData : register(b0)
{
	float2 ScreenDimensions;
	float Frame;
}

cbuffer GeometryData : register(b1)
{
	PLANE        Planes[30];
	SPHERE       Spheres[30];
	BOX          Boxes[30];
	POINT_LIGHT	 PointLights[5];
	DIRECTIONAL_LIGHT DirectionalLights[5];
	int          NumPlanes;
	int	         NumSpheres;
	int          NumBoxes;
	int          NumPointLights;
	int          NumDirectionalLights;
}
*/

typedef struct GEOM_MATERIAL {
	XMFLOAT3 Color;
	float Metallic;
	float Roughness;
	float P0, P1, P2;
} GEOM_MATERIAL;

typedef struct GEOM_PLANE {
	GEOM_MATERIAL Material;
	XMFLOAT3 Position;
	float Size;
	XMFLOAT3 Normal;
	float P0;
} GEOM_PLANE;

typedef struct GEOM_SPHERE {
	GEOM_MATERIAL Material;
	XMFLOAT3 Center;
	float Radius;
} GEOM_SPHERE;

typedef struct GEOM_BOX {
	GEOM_MATERIAL Material;
	XMFLOAT3 Min;
	float P0;
	XMFLOAT3 Max;
	float P1;
} GEOM_BOX;

typedef struct GEOM_POINT_LIGHT {
	XMFLOAT3 Color;
	float Range;
	XMFLOAT3 Position;
	float Intensity;
} GEOM_POINT_LIGHT;

typedef struct GEOM_DIRECTIONAL_LIGHT {
	XMFLOAT3 Direction;
	float P0;
	XMFLOAT3 Color;
	float P1;
} GEOM_DIRECTIONAL_LIGHT;

class RayTracingRenderer : public SceneRenderer
{
public:

	virtual void Initialize() override final;
	virtual void Render() override final;
	virtual void Present() override final;
	virtual void Shutdown() override final;
	virtual IDeviceSwapChain* GetSwapChain() override final
	{
		return m_Device->GetSwapChain();
	}

	virtual void AddCamera(CameraComponent* InCamera)
	{
		MainCamera = InCamera;
	}

	void AddPlane(GEOM_PLANE InPlane);
	void AddSphere(GEOM_SPHERE InSphere);
	void AddBox(GEOM_BOX InBox);
	void AddPointLight(GEOM_POINT_LIGHT InPointLight);
	void AddDirectionalLight(GEOM_DIRECTIONAL_LIGHT InDirLight);

private:


	struct
	{
		IVertexBuffer* VB;
		IIndexBuffer* IB;
		IShaderResourceTable* Table;
		IGraphicsPipelineState* Pipeline;
		IConstantBuffer* CBs[3];
	} m_OnScreenQuad;


	struct GEOMETRY_DATA
	{
		GEOM_PLANE   Planes[30];
		GEOM_SPHERE  Spheres[30];
		GEOM_BOX     Boxes[30];
		GEOM_POINT_LIGHT	 PointLights[5];
		GEOM_DIRECTIONAL_LIGHT DirectionalLights[5];
		int			 Padding01;
		int          NumPlanes;
		int	         NumSpheres;
		int          NumBoxes;
		int          NumPointLights;
		int          NumDirectionalLights;
	} m_GeometryData;

	/*
	cbuffer GeometryData : register(b1)
	{
	PLANE        Planes[MAX_GEOMETRY];
	int          NumPlanes;
	SPHERE       Spheres[MAX_GEOMETRY];
	int	         NumSpheres;
	BOX          Boxes[MAX_GEOMETRY];
	int          NumBoxes;
	POINT_LIGHT	 PointLights[MAX_LIGHTS];
	int          NumPointLights;
	DIRECTIONAL_LIGHT DirectionalLights[MAX_LIGHTS];
	int          NumDirectionalLights;
	}
	*/


	IRuntimeGraphicsDevice* m_Device;

};


