#include "Rigidbody.h"

XMVECTOR Rigidbody::CalcAngularAcceleration()
{
	XMMATRIX matTensor = XMLoadFloat3x3(&inertiaTensor);
	XMMATRIX invTensor = XMMatrixInverse(nullptr, matTensor);
	XMVECTOR torque = Torque(XMVectorSet(0.5f, 0.5f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
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
	CalcAngularAcceleration();
	CalcAngularVelocity(deltaTime);
	//DampAngularVelocity(deltaTime);

	XMFLOAT3 angVelV3;
	XMStoreFloat3(&angVelV3, angularVelocity);
	//Quaternion qOrientation;
	qOrientation.addScaledVector(Vector3(angVelV3), deltaTime);
	qOrientation.normalise();
	//XMMATRIX orientation;
	CalculateTransformMatrixRowMajor(orientation, GetPosition(), qOrientation);
	return orientation;
}