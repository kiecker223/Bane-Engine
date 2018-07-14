#pragma once

#include "KieckerMath.h"

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

	inline float3 GetPosition()
	{
		return m_Position;
	}

	inline Quaternion GetRotation()
	{
		return m_Rotation;
	}

	inline float3 GetScale()
	{
		return m_Scale;
	}

	inline void Translate(const float3& Direction)
	{
		m_Position.x += Direction.x;
		m_Position.y += Direction.y;
		m_Position.z += Direction.z;
	}

	inline void Translate(float X, float Y, float Z)
	{
		Translate(float3(X, Y, Z));
	}

	inline void Rotate(const float3& Euler)
	{
		m_Rotation *= Quaternion(Euler);
		m_Rotation.Normalize();
	}

	inline void Rotate(float X, float Y, float Z)
	{
		float3 Rot(X, Y, Z);
		Rotate(Rot);
	}

	inline void SetRotation(const float3& Euler)
	{
		m_Rotation = Quaternion(Euler).Normalized();
	}

	inline void SetRotation(float X, float Y, float Z)
	{
		SetRotation(float3(X, Y, Z));
	}

	inline void SetPosition(const float3& InPosition)
	{
		float3 Translation = float3(InPosition.x - m_Position.x, InPosition.y - m_Position.y, InPosition.z - m_Position.z);
		m_Position = InPosition;
	}

	inline void Scale(float Scalar)
	{
		m_Scale.x = Scalar;
		m_Scale.y = Scalar;
		m_Scale.z = Scalar;
	}

	inline void SetScale(const float3& InScale)
	{
		m_Scale = InScale;
	}

	inline matrix GetMatrix() const
	{
		return matTransformation(m_Position, m_Rotation, m_Scale);
	}

	inline float3 GetForward() const
	{
		float3 Result(0.0f, 0.0f, 1.0f);
		float3x3 RotationMat = (float3x3)m_Rotation.RotationMatrix();
		Result = RotationMat * Result;
		return Result;
	}

	inline float3 GetUpVector() const
	{
		float3 Result(0.0f, 1.0f, 0.0f);
		matrix RotationMat = m_Rotation.RotationMatrix();
		Result = (float3x3)RotationMat * Result;
		return Result;
	}

	inline float3 GetRightVector() const
	{
		static const float3 Up(0.f, 1.f, 0.f);
		return cross(GetForward(), Up);
	}

private:

	float3 m_Position;
	Quaternion m_Rotation;
	float3 m_Scale;
};


