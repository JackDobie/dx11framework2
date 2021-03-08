#pragma once

#include <directxmath.h>
#include "Transform.h"

class ParticleModel
{
public:
	ParticleModel(Transform* _transform);
	~ParticleModel();

	void Update(float t);

	void SetPosition(XMFLOAT3 position) { transform->position = position; }
	void SetPosition(float x, float y, float z) { transform->position.x = x; transform->position.y = y; transform->position.z = z; }
	XMFLOAT3 GetPosition() const { return transform->position; }
	void MovePosition(XMFLOAT3 pos);

	void SetRotation(XMFLOAT3 rotation) { transform->rotation = rotation; }
	void SetRotation(float x, float y, float z) { transform->rotation.x = x; transform->rotation.y = y; transform->rotation.z = z; }
	XMFLOAT3 GetRotation() const { return transform->rotation; }
	void ChangeRotation(XMFLOAT3 rot);

	void SetScale(XMFLOAT3 scale) { transform->scale = scale; }
	void SetScale(float x, float y, float z) { transform->scale.x = x; transform->scale.y = y; transform->scale.z = z; }
	XMFLOAT3 GetScale() const { return transform->scale; }
private:
	Transform* transform;
};