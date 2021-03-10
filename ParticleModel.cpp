#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration)
{
	transform = _transform;
	useConstAccel = _useConstAccel;
	velocity = XMLoadFloat3(&initialVelocty);
	acceleration = XMLoadFloat3(&initialAcceleration);
}
ParticleModel::~ParticleModel()
{
	if (transform == nullptr) delete(transform);
}

void ParticleModel::Update(float deltaTime)
{
	if (useConstAccel)
		moveConstAcceleration(deltaTime);
	else
		moveConstVelocity(deltaTime);
}

void ParticleModel::moveConstVelocity(float deltaTime)
{
	// update world position of object by adding displacement (velocity * time step) to previous position
	XMVECTOR oldPos = XMLoadFloat3(&transform->position);
	XMStoreFloat3(&transform->position, XMVectorMultiplyAdd(velocity, XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f), oldPos));

	//XMVectorAdd(XMLoadFloat3(&transform->position.x), XMVectorMultiply(XMLoadFloat3(&velocity), XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f));
	//transform->position = XMFLOAT3(transform->position.x + (velocity.x * deltaTime), transform->position.y + (velocity.y * deltaTime), transform->position.z + (velocity.z * deltaTime));
}

void ParticleModel::moveConstAcceleration(float deltaTime)
{
	XMVECTOR oldPos = XMLoadFloat3(&transform->position);
	// update world position of object by adding displacement to previously calculated position
	// position = previous position + previous velocity x deltaTime + 0.5 x acceleration x time step x time step
	XMVECTOR deltaVec = XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f);
	//XMVectorAdd(velocity, XMLoadFloat3(&transform->position));
	//XMVectorMultiply(XMVectorAdd(velocity, XMLoadFloat3(&transform->position)), XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f));
	//XMVectorMultiply(acceleration, step);
	//XMVectorMultiply(XMVectorMultiply(XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f), acceleration), XMVectorMultiply(step, step));
	XMStoreFloat3(&transform->position,
		XMVectorAdd(
			XMVectorMultiply(
				XMVectorAdd(oldPos, velocity), deltaVec),
			XMVectorMultiply(
				XMVectorMultiply(XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f), acceleration),
				XMVectorSet(1.0f,1.0f,1.0f,0.0f)/*XMVectorMultiply(deltaVec, deltaVec)*/)));

	// update velocity of object by adding change relative to previously calculated velocty
	// velocity = previous velocity + acceleration x deltaTime

	velocity = (velocity + acceleration) * deltaVec;
}

//void ParticleModel::MovePosition(XMFLOAT3 pos)
//{
//	transform->position = XMFLOAT3(transform->position.x + pos.x, transform->position.y + pos.y, transform->position.z + pos.z);
//}
//void ParticleModel::ChangeRotation(XMFLOAT3 rot)
//{
//	transform->rotation = XMFLOAT3(transform->rotation.x + rot.x, transform->rotation.y + rot.y, transform->rotation.z + rot.z);
//}

void ParticleModel::AddVelocity(XMFLOAT3 addVel)
{
	XMVECTOR addVec = XMLoadFloat3(&addVel);
	velocity = XMVectorAdd(velocity, addVec);
}
void ParticleModel::AddAcceleration(XMFLOAT3 addAcc)
{
	XMVECTOR addVec = XMLoadFloat3(&addAcc);
	velocity = XMVectorAdd(acceleration, addVec);
}

void ParticleModel::AddVelOrAcc(XMFLOAT3 addF3)
{
	if (useConstAccel)
		AddAcceleration(addF3);
	else
		AddVelocity(addF3);
}