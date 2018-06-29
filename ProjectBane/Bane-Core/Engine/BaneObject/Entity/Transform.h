#pragma once

#include "Core/KieckerMath.h"

class Transform
{
public:

	Transform() :
		m_Position(0.0f, 0.0f, 0.0f),
		m_Rotation(float3(0.f, 0.f, 0.f)),
		m_Scale(1.0f, 1.0f, 1.0f)
	{
	}
	Transform(const float3& InPosition, const float3& InRotation, const float3& InScale) :
		m_Position(InPosition),
		m_Rotation(InRotation),
		m_Scale(InScale)
	{
	}

	ForceInline float3 GetPosition()
	{
		return m_Position;
	}

	ForceInline Quaternion GetRotation()
	{
		return m_Rotation;
	}

	ForceInline float3 GetScale()
	{
		return m_Scale;
	}

	ForceInline void Translate(const float3& Direction)
	{
		m_Position.x += Direction.x;
		m_Position.y += Direction.y;
		m_Position.z += Direction.z;
	}

	ForceInline void Translate(float X, float Y, float Z)
	{
		Translate(float3(X, Y, Z));
	}

	ForceInline void Rotate(const float3& Euler)
	{
		m_Rotation *= Quaternion(Euler);
		m_Rotation.Normalize();
	}

	ForceInline void Rotate(float X, float Y, float Z)
	{
		float3 Rot(X, Y, Z);
		Rotate(Rot);
	}

	ForceInline void SetRotation(const float3& Euler)
	{
		m_Rotation = Quaternion(Euler).Normalized();
	}

	ForceInline void SetRotation(float X, float Y, float Z)
	{
		SetRotation(float3(X, Y, Z));
	}

	ForceInline void SetPosition(const float3& InPosition)
	{
		float3 Translation = float3(InPosition.x - m_Position.x, InPosition.y - m_Position.y, InPosition.z - m_Position.z);
		m_Position = InPosition;
	}

	ForceInline void Scale(float Scalar)
	{
		m_Scale.x = Scalar;
		m_Scale.y = Scalar;
		m_Scale.z = Scalar;
	}

	ForceInline void SetScale(const float3& InScale)
	{
		m_Scale = InScale;
	}

	ForceInline matrix GetMatrix() const
	{
		return matTransformation(m_Position, m_Rotation, m_Scale);
	}

	ForceInline float3 GetForward() const
	{
		float3 Result(0.0f, 0.0f, 1.0f);
		float3x3 RotationMat = (float3x3)m_Rotation.RotationMatrix();
		Result = RotationMat * Result;
		return Result;
	}

	ForceInline float3 GetUpVector() const
	{
		float3 Result(0.0f, 1.0f, 0.0f);
		matrix RotationMat= m_Rotation.RotationMatrix();
		Result = (float3x3)RotationMat * Result;
		return Result;
	}

private:

	float3 m_Position;
	Quaternion m_Rotation;
	float3 m_Scale;
};


