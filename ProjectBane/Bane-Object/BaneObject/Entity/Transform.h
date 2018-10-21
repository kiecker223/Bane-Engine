#pragma once

#include "KieckerMath.h"

class Transform
{
public:

	Transform() :
		m_Position(0.0, 0.0, 0.0),
		m_Rotation(float3(0.f, 0.f, 0.f)),
		m_Scale(1.0, 1.0, 1.0)
	{
	}
	Transform(const double3& InPosition, const float3& InRotation, const double3& InScale) :
		m_Position(InPosition),
		m_Rotation(InRotation),
		m_Scale(InScale)
	{
	}

	inline double3 GetPosition()
	{
		return m_Position;
	}

	inline Quaternion GetRotation()
	{
		return m_Rotation;
	}

	inline double3 GetScale()
	{
		return m_Scale;
	}

	inline void Translate(const double3& Direction)
	{
		m_Position.x += Direction.x;
		m_Position.y += Direction.y;
		m_Position.z += Direction.z;
	}

	inline void Translate(double X, double Y, double Z)
	{
		Translate(double3(X, Y, Z));
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

	inline void SetPosition(const double3& InPosition)
	{
		m_Position = InPosition;
	}

	inline void Scale(double Scalar)
	{
		m_Scale.x = Scalar;
		m_Scale.y = Scalar;
		m_Scale.z = Scalar;
	}

	inline void SetScale(const double3& InScale)
	{
		m_Scale = InScale;
	}

	inline float4x4 GetMatrix(const double3& CameraPos) const
	{
		return matTransformation(fromDouble3(m_Position - CameraPos), m_Rotation, fromDouble3(m_Scale));
	}

	inline float4x4 GetRawMatrix() const
	{
		return matTransformation(fromDouble3(m_Position), m_Rotation, fromDouble3(m_Scale));
	}

	inline double3 GetForward() const
	{
		float3 Result(0.0, 0.0, 1.0);
		float3x3 RotationMat = (float3x3)m_Rotation.RotationMatrix();
		Result = RotationMat * Result;
		return fromFloat3(Result);
	}

	inline double3 GetUpVector() const
	{
		float3 Result(0.0, 1.0, 0.0);
		auto RotationMat = m_Rotation.RotationMatrix();
		Result = (float3x3)RotationMat * Result;
		return fromFloat3(Result);
	}

	inline double3 GetRightVector() const
	{
		static const double3 Up(0., 1., 0.);
		return cross(GetForward(), Up);
	}

private:

	double3 m_Position;
	Quaternion m_Rotation;
	double3 m_Scale;
};


