#pragma once

#include <DirectXMath.h>
#include "ParticleModel.h"
#include "Quaternion.h"
#include "Structures.h"
using namespace DirectX;

class Rigidbody : public ParticleModel
{
public:
	Rigidbody(MeshData* mesh, Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration, float mass, bool gravity, float _deltaTime) : ParticleModel(_transform, _useConstAccel, initialVelocty, initialAcceleration, mass, gravity, _deltaTime)
	{
		inertiaTensor = XMFLOAT3X3((1 / 12) * objectMass * ((mesh->YSize * mesh->YSize) + (mesh->ZSize * mesh->ZSize)), 0, 0,
						0, (1 / 12) * objectMass * ((mesh->XSize * mesh->XSize) + (mesh->ZSize * mesh->ZSize)), 0,
						0, 0, (1 / 12) * objectMass * ((mesh->XSize * mesh->XSize) + (mesh->YSize * mesh->ZSize)));

		angularAcceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		angularVelocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}

	XMVECTOR Torque(XMVECTOR force, XMVECTOR point);

	//XMFLOAT3X3 CalculateInertiaTensor();
	void SetInertiaTensor(XMFLOAT3X3 newInertiaTensor) { inertiaTensor = newInertiaTensor; }
	XMFLOAT3X3 GetInertiaTensor() { return inertiaTensor; }

	XMVECTOR CalcAngularAcceleration();
	XMVECTOR CalcAngularVelocity(float deltaTime);

	void DampAngularVelocity(float deltaTime);

	void CalcOrientation(float deltaTime);
private:
	float angularDamp = 0.99f;
	XMFLOAT3X3 inertiaTensor;

	XMVECTOR torque;

	XMVECTOR angularAcceleration;
	XMVECTOR angularVelocity;

	XMMATRIX orientation;
};