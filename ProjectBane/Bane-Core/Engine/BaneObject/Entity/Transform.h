#pragma once

#include "Core/ExpanseMath.h"

class Transform
{
public:

	Transform() :
		m_Position(0.0f, 0.0f, 0.0f),
		m_Rotation(XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f)),
		m_Scale(1.0f, 1.0f, 1.0f)
	{
	}
	Transform(const XMFLOAT3& InPosition, const XMVECTOR& InRotation, const XMFLOAT3& InScale) :
		m_Position(InPosition),
		m_Rotation(InRotation),
		m_Scale(InScale)
	{
	}

	ForceInline XMFLOAT3 GetPosition()
	{
		return m_Position;
	}

	ForceInline XMVECTOR GetRotation()
	{
		return m_Rotation;
	}

	ForceInline XMFLOAT3 GetScale()
	{
		return m_Scale;
	}

	ForceInline void Translate(const XMFLOAT3& Direction)
	{
		m_Position.x += Direction.x;
		m_Position.y += Direction.y;
		m_Position.z += Direction.z;
	}

	ForceInline void Translate(float X, float Y, float Z)
	{
		Translate(XMFLOAT3(X, Y, Z));
	}

	ForceInline void Translate(const XMVECTOR& Direction)
	{
		XMFLOAT3 Dir;
		XMStoreFloat3(&Dir, Direction);
		Translate(Dir);
	}

	ForceInline void Rotate(const XMFLOAT3& Euler)
	{
		XMVECTOR NewRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(Euler.x), XMConvertToRadians(Euler.y), XMConvertToRadians(Euler.z));
		m_Rotation = XMQuaternionMultiply(m_Rotation, NewRotation);
	}

	ForceInline void Rotate(float X, float Y, float Z)
	{
		Rotate(XMFLOAT3(X, Y, Z));
	}

	ForceInline void SetRotation(const XMFLOAT3& Euler)
	{
		m_Rotation = XMQuaternionRotationRollPitchYaw(Euler.x, Euler.y, Euler.z);
	}

	ForceInline void SetRotation(float X, float Y, float Z)
	{
		SetRotation(XMFLOAT3(X, Y, Z));
	}

	ForceInline void SetPosition(const XMFLOAT3& InPosition)
	{
		XMFLOAT3 Translation = XMFLOAT3(InPosition.x - m_Position.x, InPosition.y - m_Position.y, InPosition.z - m_Position.z);
		m_Position = InPosition;
	}

	ForceInline void Scale(float Scalar)
	{
		m_Scale.x = Scalar;
		m_Scale.y = Scalar;
		m_Scale.z = Scalar;
	}

	ForceInline void SetScale(const XMFLOAT3& InScale)
	{
		m_Scale = InScale;
	}

	ForceInline XMMATRIX GetMatrix() const
	{
		return XMMatrixTranspose(XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
			(XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z)) *
			(XMMatrixRotationQuaternion(m_Rotation)));
	}

	ForceInline XMVECTOR GetForward() const
	{
		XMFLOAT3 ForwardDir(0.0f, 0.0f, -1.0f);
		XMVECTOR Result = XMLoadFloat3(&ForwardDir);
		XMMATRIX RotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(m_Rotation));
		Result = XMVector3Transform(Result, RotationMatrix);
		return Result;
	}

private:

	XMFLOAT3 m_Position;
	XMVECTOR m_Rotation;
	XMFLOAT3 m_Scale;
};


