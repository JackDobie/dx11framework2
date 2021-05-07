#include "Rigidbody.h"

XMVECTOR Rigidbody::CalcAngularAcceleration()
{
	XMMATRIX matTensor = XMLoadFloat3x3(&inertiaTensor);
	XMMATRIX invTensor = XMMatrixInverse(nullptr, matTensor);
	XMVECTOR torque = Torque(XMLoadFloat3(&torquePoint), XMLoadFloat3(&torqueForce));
	angularAcceleration = XMVector3Transform(torque, invTensor);
	return angularAcceleration;
}
XMVECTOR Rigidbody::CalcAngularAcceleration(XMVECTOR torque)
{
	XMMATRIX matTensor = XMLoadFloat3x3(&inertiaTensor);
	XMMATRIX invTensor = XMMatrixInverse(nullptr, matTensor);
	angularAcceleration = XMVector3Transform(torque, invTensor);
	return angularAcceleration;
}

XMVECTOR Rigidbody::CalcAngularVelocity(float deltaTime)
{
	angularVelocity = angularVelocity + angularAcceleration * deltaTime;
	return angularVelocity;
}

void Rigidbody::DampAngularVelocity(float deltaTime)
{
	angularVelocity *= pow(angularDamp, deltaTime);
}

XMMATRIX Rigidbody::CalcOrientation(float deltaTime)
{
	XMFLOAT3 angVelV3;
	XMStoreFloat3(&angVelV3, angularVelocity);
	qOrientation.addScaledVector(angVelV3, deltaTime);
	qOrientation.normalise();
	CalculateTransformMatrixRowMajor(orientation, GetPosition(), qOrientation);
	return orientation;
}

void Rigidbody::Rotate(float deltaTime)
{
	CalcAngularAcceleration();
	CalcAngularVelocity(deltaTime);
	DampAngularVelocity(deltaTime);
}