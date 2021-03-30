#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Rigidbody
{
public:
	XMVECTOR Torque(XMVECTOR force, XMVECTOR point);

	XMFLOAT3X3 CalculateInertiaTensor();
	void SetInertiaTensor(XMFLOAT3X3 newInertiaTensor) { inertiaTensor = newInertiaTensor; }
	XMFLOAT3X3 GetInertiaTensor() { return inertiaTensor; }
private:
	XMFLOAT3X3 inertiaTensor;
};