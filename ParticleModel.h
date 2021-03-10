#pragma once

#include <directxmath.h>
#include <d3d11.h>
#include "Transform.h"

class ParticleModel
{
public:
	ParticleModel(Transform* _transform, bool _useConstAccel, XMFLOAT3 initialVelocty, XMFLOAT3 initialAcceleration);
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

	void SetVelocity(XMFLOAT3 newVelocity) { velocity = XMLoadFloat3(&newVelocity); }
	void AddVelocity(XMFLOAT3 addVel);
	void SetAcceleration(XMFLOAT3 newAcceleration) { acceleration = XMLoadFloat3(&newAcceleration); }
	void AddAcceleration(XMFLOAT3 addAcc);

	void AddVelOrAcc(XMFLOAT3 addF3);

	void SetUsingConstAccel(bool _useConstAccel) { useConstAccel = _useConstAccel; }
private:
	void moveConstVelocity(float deltaTime);
	void moveConstAcceleration(float deltaTime);

	Transform* transform;

	XMVECTOR velocity;
	XMVECTOR acceleration;

	bool useConstAccel;
};