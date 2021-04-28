#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration, float mass, bool gravity, float _deltaTime, float _boundSphereRadius)
{
	transform = _transform;
	useConstAccel = _useConstAccel;
	velocity = initialVelocty;
	acceleration = initialAcceleration;
	objectMass = mass;
	useGravity = gravity;
	netForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deltaTime = _deltaTime;
	gravityForce = 0.5f;
	dragFactor = 0.5f;
	drag = XMFLOAT3(0, 0, 0);
	thrustEnabled = false;
	thrustForce = (objectMass * gravityForce) * 1.5f;
	boundingSphereRadius = _boundSphereRadius;
	enableBoundingSphere = _boundSphereRadius > 0.0f ? true : false; //disable bound sphere if not greater than 0
	//boundSphere = new BoundingSphere(_boundSphereRadius, transform);
	//boundSphere->SetEnabled(_boundSphereRadius > 0.0f ? true : false); //disable bound sphere if not greater than 0
}
ParticleModel::~ParticleModel()
{
	if (transform == nullptr) delete(transform);
}

void ParticleModel::Update(float _deltaTime)
{
	deltaTime = _deltaTime;

	//MotionInFluid();

	// calculate net external force
	UpdateNetForce();

	// update acceleration of object using Newton's second law of motion
	// assumption: net external force is constant between consecutive updates of object state
	UpdateAccel();

	// update world position and velocity of object
	Move();
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
		netForce.x += force.x * deltaTime;
		netForce.y += force.y * deltaTime;
		netForce.z += force.z * deltaTime;
	}

	if (useGravity)
	{
		netForce.y -= (objectMass * gravityForce); //mass * gravity

		if (thrustEnabled)
		{
			netForce.y += thrustForce;
		}
	}

	netForce.x += drag.x;
	netForce.y += drag.y;
	netForce.z += drag.z;
}

void ParticleModel::UpdateAccel()
{
	acceleration.x += netForce.x / objectMass;
	acceleration.y += netForce.y / objectMass;
	acceleration.z += netForce.z / objectMass;
}

void ParticleModel::Move()
{
	// update world position of object by adding displacement to previously calculated position
	XMFLOAT3 oldPos = transform->position;
	transform->position.x = oldPos.x + velocity.x * deltaTime + 0.5f * acceleration.x * deltaTime * deltaTime;
	transform->position.y = oldPos.y + velocity.y * deltaTime + 0.5f * acceleration.y * deltaTime * deltaTime;
	transform->position.z = oldPos.z + velocity.z * deltaTime + 0.5f * acceleration.z * deltaTime * deltaTime;

	if (useGravity)
	{
		if (transform->position.y < 0.5f) //replace with object size or add collision later on
		{
			// stop movement
			acceleration.y = 0.0f;
			velocity.y = 0.0f;
			netForce.y = 0.0f;
			transform->position.y = 0.5f;
		}
		/*if (transform->position.y < 0.75f)
		{
			thrustEnabled = true;
		}
		else
			thrustEnabled = false;*/
	}

	if (velocity.x < (objectMass * 2) && velocity.y < (objectMass * 2) && velocity.z < (objectMass * 2))
	{
		// update velocity of object by adding change relative to previously calculated velocity
		XMFLOAT3 oldVel = velocity;
		velocity.x = oldVel.x + acceleration.x * deltaTime;
		velocity.y = oldVel.y + acceleration.y * deltaTime;
		velocity.z = oldVel.z + acceleration.z * deltaTime;
	}
}

void ParticleModel::MotionInFluid()
{
	DragForce(true);
}
void ParticleModel::DragForce(bool laminar)
{
	if (laminar)
		DragLamFlow();
	else
		DragTurbFlow();
}
void ParticleModel::DragLamFlow()
{
	drag.x = -dragFactor * velocity.x;
	drag.y = -dragFactor * velocity.y;
	drag.z = -dragFactor * velocity.z;
}
void ParticleModel::DragTurbFlow()
{
	float velMag = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z));
	XMFLOAT3 unitVel;
	XMStoreFloat3(&unitVel, XMVector3Normalize(XMLoadFloat3(&velocity)));

	float dragMag = dragFactor * velMag * velMag;

	drag.x = -dragMag * unitVel.x;
	drag.y = -dragMag * unitVel.y;
	drag.z = -dragMag * unitVel.z;

}

bool ParticleModel::CheckCollision(XMFLOAT3 otherPos, float otherRadius)
{
	if (enableBoundingSphere)
	{
		return SphereCollisionCheck(otherPos, otherRadius);// boundSphere->CheckCollision(otherPos, otherRadius);
	}
	else
	{
		return false;
	}
}
bool ParticleModel::SphereCollisionCheck(XMFLOAT3 otherPos, float otherRadius)
{
	// use pythagoras to find distance as float
	float dx = otherPos.x - transform->position.x;
	float dy = otherPos.y - transform->position.y;
	float dz = otherPos.z - transform->position.z;
	float distance = sqrt(dx * dx + dy * dy + dz + dz) - otherRadius;
	//distance = fabs(distance) - otherRadius;

	if (distance < boundingSphereRadius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ParticleModel::Collide(XMFLOAT3 otherPos, float otherRadius)
{
	if (enableBoundingSphere)
	{
		SphereCollide(otherPos, otherRadius);
		//boundSphere->Collide(otherPos, otherRadius);
	}
}
void ParticleModel::SphereCollide(XMFLOAT3 otherPos, float otherRadius)
{
	float dx = otherPos.x - transform->position.x;
	float dy = otherPos.y - transform->position.y;
	float dz = otherPos.z - transform->position.z;
	float pos[3] = { dx, dy, dz };
	sort(pos, pos + 3); //sort to find if x y or z is shortest

	if (pos[0] == dx)
	{
		for each(XMFLOAT3 force in forces)
		{
			force.x = 0.0f;
		}
		velocity.x = 0.0f;
		acceleration.x = 0.0f;
		if (dx > 0.0f) // if other object x distance is positive
		{
			//transform->position.x += dx * 0.1f;
			transform->position.x = otherPos.x - otherRadius;
			//transform->position.x -= 0.5f;
		}
		else
		{
			//transform->position.x -= dx * 0.1f;
			transform->position.x = otherPos.x + otherRadius;
			//transform->position.x += 0.5f;
		}
	}
	//if (pos[0] == dy)
	//{
	//	for each (XMFLOAT3 force in forces)
	//	{
	//		force.y = 0.0f;
	//	}
	//	velocity.y = -velocity.y * 0.2f;
	//	acceleration.y = -acceleration.y * 0.2f;
	//	if (dy > 0.0f)
	//	{
	//		//transform->position.y += dy * 0.1f;
	//		transform->position.y = _position.y + _radius;
	//	}
	//	else
	//	{
	//		//transform->position.y -= dy * 0.1f;
	//		transform->position.y = _position.y - _radius;
	//	}
	//}
	//if (pos[0] == dz)
	//{
	//	for each (XMFLOAT3 force in forces)
	//	{
	//		force.z = 0.0f;
	//	}
	//	velocity.z = -velocity.z * 0.5f;
	//	acceleration.z = -acceleration.z * 0.5f;
	//	//if (dz > 0.0f)
	//	//{
	//	//	//transform->position.z += dz * 0.1f;
	//	//	//transform->position.z = otherPos.z - otherRadius;
	//	//	transform->position.z -= 0.5f;
	//	//}
	//	//else
	//	//{
	//	//	//transform->position.z -= dz * 0.1f;
	//	//	//transform->position.z = otherPos.z + otherRadius;
	//	//	transform->position.z += 0.5f;
	//	//}
	//}
}