#pragma once

#include <DirectXMath.h>
#include "ParticleModel.h"
#include "Structures.h"
using namespace DirectX;

class Rigidbody : public ParticleModel
{
public:
	XMVECTOR Torque(XMVECTOR force, XMVECTOR point);

	XMFLOAT3X3 CalculateInertiaTensor();
	void SetInertiaTensor(XMFLOAT3X3 newInertiaTensor) { inertiaTensor = newInertiaTensor; }
	XMFLOAT3X3 GetInertiaTensor() { return inertiaTensor; }
private:
	float angularDamp = 0.99f;
	MeshData mesh;
	XMFLOAT3X3 inertiaTensor = XMFLOAT3X3((1 / 12) * objectMass * ((mesh.YSize * mesh.YSize) + (mesh.ZSize * mesh.ZSize)), 0, 0,
										  0, (1 / 12) * objectMass * ((mesh.XSize * mesh.XSize) + (mesh.ZSize * mesh.ZSize)),0,
										  0,0, (1 / 12) * objectMass * ((mesh.XSize * mesh.XSize) + (mesh.YSize * mesh.ZSize)));
};