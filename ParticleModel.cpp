#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration, float mass)
{
	transform = _transform;
	useConstAccel = _useConstAccel;
	//velocity = XMLoadFloat3(&initialVelocty);
	//acceleration = XMLoadFloat3(&initialAcceleration);
	velocity = initialVelocty;
	acceleration = initialAcceleration;
	objectMass = mass;
	netForce = XMFLOAT3(0, 0, 0);
}
ParticleModel::~ParticleModel()
{
	if (transform == nullptr) delete(transform);
}

void ParticleModel::Update(float deltaTime)
{
	// calculate net external force
	UpdateNetForce();

	// update acceleration of object using Newton's second law of motion
	// assumption: net external force is constant between consecutive updates of object state
	UpdateAccel();

	// update world position and velocity of object
	Move(deltaTime);
	
	/*if (useConstAccel)
		moveConstAcceleration(deltaTime);
	else
		moveConstVelocity(deltaTime);*/
}

//void ParticleModel::moveConstVelocity(float deltaTime)
//{
//	// update world position of object by adding displacement (velocity * time step) to previous position
//	transform->position = XMFLOAT3((velocity.x * deltaTime) + transform->position.x, (velocity.y * deltaTime) + transform->position.y, (velocity.z * deltaTime) + transform->position.z);
//	//XMVECTOR oldPos = XMLoadFloat3(&transform->position);
//	//XMStoreFloat3(&transform->position, XMVectorMultiplyAdd(velocity, XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f), oldPos));
//
//	//XMVectorAdd(XMLoadFloat3(&transform->position.x), XMVectorMultiply(XMLoadFloat3(&velocity), XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f));
//	//transform->position = XMFLOAT3(transform->position.x + (velocity.x * deltaTime), transform->position.y + (velocity.y * deltaTime), transform->position.z + (velocity.z * deltaTime));
//}
//
//void ParticleModel::moveConstAcceleration(float deltaTime)
//{
//	//XMVECTOR oldPos = XMLoadFloat3(&transform->position);
//	XMFLOAT3 oldPos = transform->position;
//
//	// update world position of object by adding displacement to previously calculated position
//	// position = previous position + previous velocity x deltaTime + 0.5 x acceleration x time step x time step
//	//XMVECTOR deltaVec = XMVectorSet(deltaTime, deltaTime, deltaTime, 0.0f);
//	XMFLOAT3 deltaF3 = XMFLOAT3(deltaTime, deltaTime, deltaTime);
//	//XMFLOAT3 vel, acc;
//	//XMStoreFloat3(&vel, velocity);
//	//XMStoreFloat3(&acc, acceleration);
//
//	transform->position = XMFLOAT3(oldPos.x + velocity.x * deltaTime + 0.5f * acceleration.x * deltaF3.x * deltaF3.x,
//		oldPos.y + velocity.y * deltaTime + 0.5f * acceleration.y * deltaF3.y * deltaF3.y,
//		oldPos.z + velocity.z * deltaTime + 0.5f * acceleration.z * deltaF3.z * deltaF3.z);
//
//	/*XMStoreFloat3(&transform->position,
//		XMVectorAdd(
//			XMVectorMultiply(XMVectorAdd(oldPos, velocity), deltaVec),
//			XMVectorMultiply(
//				XMVectorMultiply(XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f), acceleration),
//				XMVectorMultiply(deltaVec, deltaVec))));*/ //does not work
//
//	// update velocity of object by adding change relative to previously calculated velocty
//	// velocity = previous velocity + acceleration x deltaTime
//
//	//velocity = (velocity + acceleration) * deltaF3;
//	velocity = XMFLOAT3((velocity.x + acceleration.x) * deltaTime, (velocity.y + acceleration.y) * deltaTime, (velocity.z + acceleration.z) * deltaTime);
//}

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
	velocity.x += addVel.x;
	velocity.y += addVel.y;
	velocity.z += addVel.z;
	/*XMVECTOR addVec = XMLoadFloat3(&addVel);
	velocity = XMVectorAdd(velocity, addVec);*/
}
void ParticleModel::AddAcceleration(XMFLOAT3 addAcc)
{
	acceleration.x += addAcc.x;
	acceleration.y += addAcc.y;
	acceleration.z += addAcc.z;
	/*XMVECTOR addVec = XMLoadFloat3(&addAcc);
	acceleration = XMVectorAdd(acceleration, addVec);*/
}

void ParticleModel::AddVelOrAcc(XMFLOAT3 addF3)
{
	if (useConstAccel)
		AddAcceleration(addF3);
	else
		AddVelocity(addF3);
}

void ParticleModel::UpdateNetForce()
{
	// calculate net external force
	// for each force ( eg thrust, brake force, friction force )
	//{
	//	netforce.x += force.x; netforce.y += force.y;
	//}
	netForce.x += 0.005;
	netForce.y += 0;
	netForce.z += 0;
}

void ParticleModel::UpdateAccel()
{
	acceleration.x += netForce.x / objectMass;
	acceleration.y += netForce.y / objectMass;
	acceleration.z += netForce.z / objectMass;
}

void ParticleModel::Move(float deltaTime)
{
	// update world position of object by adding displacement to previously calculated position
	XMFLOAT3 oldPos = transform->position;
	transform->position.x = oldPos.x + velocity.x * deltaTime + 0.5f * acceleration.x * deltaTime * deltaTime;
	transform->position.y = oldPos.y + velocity.y * deltaTime + 0.5f * acceleration.y * deltaTime * deltaTime;
	transform->position.z = oldPos.z + velocity.z * deltaTime + 0.5f * acceleration.z * deltaTime * deltaTime;

	// update velocity of object by adding change relative to previously calculated velocity
	XMFLOAT3 oldVel = velocity;
	velocity.x = oldVel.x + acceleration.x * deltaTime;
	velocity.y = oldVel.y + acceleration.y * deltaTime;
	velocity.z = oldVel.z + acceleration.z * deltaTime;
}