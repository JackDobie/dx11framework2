#pragma once

#include <directxmath.h>
#include <d3d11.h>
#include <vector>
#include "Transform.h"
#include "Debug.h"

class ParticleModel
{
public:
	ParticleModel(Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration, float mass, bool gravity, float _deltaTime, float _boundSphereRadius);
	~ParticleModel();

	void Update(float deltaTime);

	void SetPosition(XMFLOAT3 position) { transform->position = position; }
	void SetPosition(float x, float y, float z) { transform->position.x = x; transform->position.y = y; transform->position.z = z; }
	XMFLOAT3 GetPosition() const { return transform->position; }

	void SetRotation(XMFLOAT3 rotation) { transform->rotation = rotation; }
	void SetRotation(float x, float y, float z) { transform->rotation.x = x; transform->rotation.y = y; transform->rotation.z = z; }
	XMFLOAT3 GetRotation() const { return transform->rotation; }

	void SetScale(XMFLOAT3 scale) { transform->scale = scale; }
	void SetScale(float x, float y, float z) { transform->scale.x = x; transform->scale.y = y; transform->scale.z = z; }
	XMFLOAT3 GetScale() const { return transform->scale; }

	void SetVelocity(XMFLOAT3 newVelocity) { velocity = newVelocity; }
	void AddVelocity(XMFLOAT3 addVel);
	void SetAcceleration(XMFLOAT3 newAcceleration) { acceleration = newAcceleration; }
	void AddAcceleration(XMFLOAT3 addAcc);

	void AddVelOrAcc(XMFLOAT3 addF3);

	void SetUsingConstAccel(bool _useConstAccel) { useConstAccel = _useConstAccel; }

	float objectMass;

	XMFLOAT3 GetNetForce() { return netForce; }
	void SetNetForce(XMFLOAT3 newNetForce) { netForce = newNetForce; }

	bool GetGravity() { return useGravity; }
	void SetGravity(bool gravity) { useGravity = gravity; }

	std::vector<XMFLOAT3> GetForces() { return forces; }
	/// <summary> Adds a force to add constant acceleration to the object </summary>
	void AddForce(XMFLOAT3 newForce) { forces.push_back(newForce); }
	/// <summary> Adds a force to add constant acceleration to the object </summary>
	void AddForceX(float x) { forces.push_back(XMFLOAT3(x, 0.0f, 0.0f)); }
	/// <summary> Adds a force to add constant acceleration to the object </summary>
	void AddForceY(float y) { forces.push_back(XMFLOAT3(0.0f, y, 0.0f)); }
	/// <summary> Adds a force to add constant acceleration to the object </summary>
	void AddForceZ(float z) { forces.push_back(XMFLOAT3(0.0f, 0.0f, z)); }

	bool GetThrustEnabled() { return thrustEnabled; }
	void SetThrustEnabled(bool isEnabled) { thrustEnabled = isEnabled; }

	float GetBoundingSphereRadius() { return boundingSphereRadius; }
	void SetBoundingSphereRadius(float newRadius) { boundingSphereRadius = newRadius; enableBoundingSphere = newRadius > 0.0f ? true : false; }
	bool GetBoundingSphereEnabled() { return enableBoundingSphere; }

	bool CollisionCheck(XMFLOAT3 position, float radius);
	//bool CollisionCheck(GameObject* otherObject); //including gameobject breaks code. do not use this
private:
	void Move();

	void UpdateNetForce();
	void UpdateAccel();

	void MotionInFluid();
	void DragForce(bool laminar);
	void DragLamFlow();
	void DragTurbFlow();
	float dragFactor;
	XMFLOAT3 drag;

	Transform* transform;

	XMFLOAT3 velocity;
	XMFLOAT3 acceleration;

	bool useConstAccel;

	XMFLOAT3 netForce;
	std::vector<XMFLOAT3> forces;

	float deltaTime;

	bool useGravity;
	float gravityForce;

	bool thrustEnabled;
	float thrustForce;

	float boundingSphereRadius;
	bool enableBoundingSphere;
};