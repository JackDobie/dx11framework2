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

void ParticleModel::AddVelocity(XMFLOAT3 addVel)
{
	velocity.x += addVel.x;
	velocity.y += addVel.y;
	velocity.z += addVel.z;
}
void ParticleModel::AddAcceleration(XMFLOAT3 addAcc)
{
	acceleration.x += addAcc.x;
	acceleration.y += addAcc.y;
	acceleration.z += addAcc.z;
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
	for each (XMFLOAT3 force in forces)
	{
		netForce.x += force.x;
		netForce.y += force.y;
		netForce.z += force.z;
	}
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