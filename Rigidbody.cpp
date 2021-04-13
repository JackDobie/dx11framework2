#include "Rigidbody.h"

XMVECTOR Rigidbody::Torque(XMVECTOR force, XMVECTOR point)
{
	return XMVector3Cross(point, force);
}

XMVECTOR Rigidbody::CalcAngularAcceleration()
{
	XMMATRIX matTensor = XMLoadFloat3x3(&inertiaTensor);
	XMMATRIX invTensor = XMMatrixInverse(nullptr, matTensor);
	return XMVector3Transform(torque, invTensor);
}

XMVECTOR Rigidbody::CalcAngularVelocity(float deltaTime)
{
	angularVelocity = angularVelocity + angularAcceleration * deltaTime;
	return angularVelocity;
}

void Rigidbody::DampAngularVelocity(float deltaTime)
{
	angularVelocity *= XMVectorPow(angularVelocity, XMVectorSet(deltaTime, deltaTime, deltaTime, deltaTime));
}

void Rigidbody::CalcOrientation(float deltaTime)
{
	XMFLOAT3 angVelV3;
	XMStoreFloat3(&angVelV3, angularVelocity);
	Quaternion qOrientation;
	qOrientation.addScaledVector(Vector3(angVelV3), deltaTime);
	qOrientation.normalise();
	CalculateTransformMatrixRowMajor(orientation, GetPosition(), qOrientation);

}