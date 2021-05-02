#pragma once

#include <DirectXMath.h>
#include "ParticleModel.h"
#include "Quaternion.h"
#include "Structures.h"
using namespace DirectX;

class Rigidbody : public ParticleModel
{
public:
	Rigidbody(XMFLOAT3 modelDimensions, Transform* transform, bool useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration, float mass, bool gravity, float deltaTime, bool enableCollision, Geometry geometry) : ParticleModel(transform, useConstAccel, initialVelocty, initialAcceleration, mass, gravity, deltaTime, enableCollision, geometry)
	{
		inertiaTensor._11 = 0.08333333333 * objectMass * ((modelDimensions.y * modelDimensions.y) + (modelDimensions.z * modelDimensions.z));
		inertiaTensor._12 = 0;
		inertiaTensor._13 = 0;
		inertiaTensor._21 = 0;
		inertiaTensor._22 = 0.08333333333 * objectMass * ((modelDimensions.x * modelDimensions.x) + (modelDimensions.z * modelDimensions.z));
		inertiaTensor._23 = 0;
		inertiaTensor._31 = 0;
		inertiaTensor._32 = 0;
		inertiaTensor._33 = 0.08333333333 * objectMass * ((modelDimensions.x * modelDimensions.x) + (modelDimensions.y * modelDimensions.y));

		/*inertiaTensor = XMFLOAT3X3((1 / 12) * objectMass * ((modelDimensions.y * modelDimensions.y) + (modelDimensions.z * modelDimensions.z)), 0, 0,
									0, (1 / 12) * objectMass * ((modelDimensions.x * modelDimensions.x) + (modelDimensions.z * modelDimensions.z)), 0,
									0, 0, (1 / 12) * objectMass * ((modelDimensions.x * modelDimensions.x) + (modelDimensions.y * modelDimensions.y)));*/

		angularAcceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		angularVelocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		orientation = XMMatrixRotationRollPitchYaw(transform->rotation.x, transform->rotation.y, transform->rotation.z);
		qOrientation = Quaternion();
		rotating = false;
		torquePoint = XMFLOAT3(0.5f, 1.0f, 0.5f);
		torqueForce = XMFLOAT3(1.0f, 1.0f, 1.0f);
	}

	XMVECTOR Torque(XMVECTOR point, XMVECTOR force) { return XMVector3Cross(point, force); }

	//XMFLOAT3X3 CalculateInertiaTensor();
	void SetInertiaTensor(XMFLOAT3X3 newInertiaTensor) { inertiaTensor = newInertiaTensor; }
	XMFLOAT3X3 GetInertiaTensor() { return inertiaTensor; }

	XMVECTOR CalcAngularAcceleration();
	XMVECTOR CalcAngularAcceleration(XMVECTOR torque);
	XMVECTOR CalcAngularVelocity(float deltaTime);

	void DampAngularVelocity(float deltaTime);

	void Rotate(float deltaTime);
	XMMATRIX CalcOrientation(float deltaTime);
	Quaternion qOrientation;
	XMMATRIX orientation;

	bool* GetRotating() { return &rotating; }
	void SetRotating(bool newRot) { rotating = newRot; }

	float* GetAngularDamp() { return &angularDamp; }
	void SetAngularDamp(float newAngDamp) { angularDamp = newAngDamp; }

	XMFLOAT3 GetTorquePoint() { return torquePoint; }
	XMFLOAT3 GetTorqueForce() { return torqueForce; }
private:
	float angularDamp = 1.0f;
	XMFLOAT3X3 inertiaTensor;

	XMVECTOR angularAcceleration;
	XMVECTOR angularVelocity;

	bool rotating;

	XMFLOAT3 torquePoint;
	XMFLOAT3 torqueForce;
};