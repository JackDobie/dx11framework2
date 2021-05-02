#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration, float mass, bool gravity, float _deltaTime, bool enableCollision, Geometry _geometry)
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
	dragFactor = 0.1f;
	drag = XMFLOAT3(0, 0, 0);
	thrustEnabled = false;
	thrustForce = (objectMass * gravityForce) * 1.5f;
	useCollision = enableCollision;
	geometry = _geometry;
	colliding = false;

	enableDrag = false;
	useLaminarDrag = true;

	if (enableCollision)
	{
		CreateAABB();
	}
}
ParticleModel::~ParticleModel()
{
	if (transform == nullptr) delete(transform);
}

void ParticleModel::Update(float _deltaTime)
{
	deltaTime = _deltaTime;

	if(enableDrag)
		MotionInFluid();

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
	float clamprangeAdd = 2;
	float clamprangeAcc = 15;
	acceleration.x += clamp(addAcc.x, -clamprangeAdd, clamprangeAdd);
	acceleration.y += clamp(addAcc.y, -clamprangeAdd, clamprangeAdd);
	acceleration.z += clamp(addAcc.z, -clamprangeAdd, clamprangeAdd);

	acceleration.x = clamp(acceleration.x, -clamprangeAcc, clamprangeAcc);
	acceleration.y = clamp(acceleration.y, -clamprangeAcc, clamprangeAcc);
	acceleration.z = clamp(acceleration.z, -clamprangeAcc, clamprangeAcc);
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

	if (enableDrag)
	{
		velocity.x -= drag.x;
		velocity.y -= drag.y;
		velocity.z -= drag.z;

		acceleration.x -= drag.x;
		acceleration.y -= drag.y;
		acceleration.z -= drag.z;
	}
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
	transform->position.x = oldPos.x + velocity.x * deltaTime;// +0.5f * acceleration.x * deltaTime * deltaTime;
	transform->position.y = oldPos.y + velocity.y * deltaTime;// +0.5f * acceleration.y * deltaTime * deltaTime;
	transform->position.z = oldPos.z + velocity.z * deltaTime;// +0.5f * acceleration.z * deltaTime * deltaTime;

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
	}

	velocity.x = velocity.x + acceleration.x * deltaTime;
	velocity.y = velocity.y + acceleration.y * deltaTime;
	velocity.z = velocity.z + acceleration.z * deltaTime;

	float clamprange = 10;
	velocity.x = clamp(velocity.x, -clamprange, clamprange);
	velocity.y = clamp(velocity.y, -clamprange, clamprange);
	velocity.z = clamp(velocity.z, -clamprange, clamprange);
	//Debug::Print(velocity);

	/*if(acceleration.x > 0.0f)
		acceleration.x -= 0.1f;
	if(acceleration.y > 0.0f)
		acceleration.y -= 0.1f;
	if(acceleration.z > 0.0f)
		acceleration.z -= 0.1f;*/
}

void ParticleModel::MotionInFluid()
{
	DragForce(useLaminarDrag);
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
	drag.x = dragFactor * velocity.x;
	drag.y = dragFactor * velocity.y;
	drag.z = dragFactor * velocity.z;
}
void ParticleModel::DragTurbFlow()
{
	float velMag = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z));
	XMFLOAT3 unitVel;
	XMStoreFloat3(&unitVel, XMVector3Normalize(XMLoadFloat3(&velocity)));

	float dragMag = dragFactor * velMag * velMag;

	drag.x = dragMag * unitVel.x;
	drag.y = dragMag * unitVel.y;
	drag.z = dragMag * unitVel.z;
}

bool ParticleModel::CheckCollision(XMFLOAT3 otherPos, vector<AABBFace> otherFaces)
{
	if (useCollision)
	{
		for each (AABBFace a in AABBFaces)
		{
			for each (AABBFace b in otherFaces)
			{
				if (a.minX + transform->position.x <= b.maxX + otherPos.x &&
					a.maxX + transform->position.x >= b.minX + otherPos.x &&
					a.minY + transform->position.y <= b.maxY + otherPos.y &&
					a.maxY + transform->position.y >= b.minY + otherPos.y &&
					a.minZ + transform->position.z <= b.maxZ + otherPos.z &&
					a.maxZ + transform->position.z >= b.minZ + otherPos.z)
				{
					return true;
				}
			}
		}
		colliding = false;
		return false;
	}
	else
	{
		return false;
	}
}

void ParticleModel::Collide(XMFLOAT3 otherPos, vector<AABBFace> otherFaces)
{
	acceleration.x = 0;
	acceleration.y = 0;
	acceleration.z = 0;
	//collide
	if (!colliding)
	{
		colliding = true;
		velocity.x = -(velocity.x * 0.1f);
		velocity.y = -(velocity.y * 0.1f);
		velocity.z = -(velocity.z * 0.1f);
	}
}

void ParticleModel::CreateAABB()
{
	AABBFace face;

	// front face
	face.minX = geometry.centreOfMass.x - (geometry.modelDimensions.x / 2);
	face.maxX = geometry.centreOfMass.x + (geometry.modelDimensions.x / 2);

	face.minY = geometry.centreOfMass.y - (geometry.modelDimensions.y / 2);
	face.maxY = geometry.centreOfMass.y + (geometry.modelDimensions.y / 2);

	face.minZ = geometry.centreOfMass.z - (geometry.modelDimensions.z / 2);
	face.maxZ = geometry.centreOfMass.z - (geometry.modelDimensions.z / 2);
	
	AABBFaces.push_back(face);

	// back face
	face.minX = geometry.centreOfMass.x - ((geometry.modelDimensions.x / 2) * transform->scale.x);
	face.maxX = geometry.centreOfMass.x + ((geometry.modelDimensions.x / 2) * transform->scale.x);

	face.minY = geometry.centreOfMass.y - ((geometry.modelDimensions.y / 2) * transform->scale.y);
	face.maxY = geometry.centreOfMass.y + ((geometry.modelDimensions.y / 2) * transform->scale.y);

	face.minZ = geometry.centreOfMass.z + ((geometry.modelDimensions.z / 2) * transform->scale.z);
	face.maxZ = geometry.centreOfMass.z + ((geometry.modelDimensions.z / 2) * transform->scale.z);

	AABBFaces.push_back(face);

	// left face
	face.minX = geometry.centreOfMass.x - ((geometry.modelDimensions.x / 2) * transform->scale.x);
	face.maxX = geometry.centreOfMass.x - ((geometry.modelDimensions.x / 2) * transform->scale.x);

	face.minY = geometry.centreOfMass.y - ((geometry.modelDimensions.y / 2) * transform->scale.y);
	face.maxY = geometry.centreOfMass.y + ((geometry.modelDimensions.y / 2) * transform->scale.y);

	face.minZ = geometry.centreOfMass.z - ((geometry.modelDimensions.z / 2) * transform->scale.z);
	face.maxZ = geometry.centreOfMass.z + ((geometry.modelDimensions.z / 2) * transform->scale.z);

	AABBFaces.push_back(face);

	// right face
	face.minX = geometry.centreOfMass.x + ((geometry.modelDimensions.x / 2) * transform->scale.x);
	face.maxX = geometry.centreOfMass.x + ((geometry.modelDimensions.x / 2) * transform->scale.x);

	face.minY = geometry.centreOfMass.y - ((geometry.modelDimensions.y / 2) * transform->scale.y);
	face.maxY = geometry.centreOfMass.y + ((geometry.modelDimensions.y / 2) * transform->scale.y);

	face.minZ = geometry.centreOfMass.z - ((geometry.modelDimensions.z / 2) * transform->scale.z);
	face.maxZ = geometry.centreOfMass.z + ((geometry.modelDimensions.z / 2) * transform->scale.z);

	AABBFaces.push_back(face);

	// top face
	face.minX = geometry.centreOfMass.x - ((geometry.modelDimensions.x / 2) * transform->scale.x);
	face.maxX = geometry.centreOfMass.x + ((geometry.modelDimensions.x / 2) * transform->scale.x);

	face.minY = geometry.centreOfMass.y + ((geometry.modelDimensions.y / 2) * transform->scale.y);
	face.maxY = geometry.centreOfMass.y + ((geometry.modelDimensions.y / 2) * transform->scale.y);

	face.minZ = geometry.centreOfMass.z - ((geometry.modelDimensions.z / 2) * transform->scale.z);
	face.maxZ = geometry.centreOfMass.z + ((geometry.modelDimensions.z / 2) * transform->scale.z);

	AABBFaces.push_back(face);

	// top face
	face.minX = geometry.centreOfMass.x - ((geometry.modelDimensions.x / 2) * transform->scale.x);
	face.maxX = geometry.centreOfMass.x + ((geometry.modelDimensions.x / 2) * transform->scale.x);

	face.minY = geometry.centreOfMass.y - ((geometry.modelDimensions.y / 2) * transform->scale.y);
	face.maxY = geometry.centreOfMass.y - ((geometry.modelDimensions.y / 2) * transform->scale.y);

	face.minZ = geometry.centreOfMass.z - ((geometry.modelDimensions.z / 2) * transform->scale.z);
	face.maxZ = geometry.centreOfMass.z + ((geometry.modelDimensions.z / 2) * transform->scale.z);

	AABBFaces.push_back(face);
}