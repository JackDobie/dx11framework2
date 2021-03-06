#pragma once

#include <directxmath.h>
#include <d3d11_1.h>
#include <string>

#include "Debug.h"
#include "Transform.h"
#include "ParticleModel.h"
#include "Appearance.h"
#include "Structures.h"
#include "Quaternion.h"
#include "Rigidbody.h"

using namespace DirectX;
using namespace std;

class GameObject
{
public:
	GameObject(string type, Geometry geometry, Material material, Transform* _transform, bool _useConstAccel, float mass, bool gravity, bool enableCollision);
	~GameObject();

	// Setters and Getters for position/rotation/scale
	void SetPosition(XMFLOAT3 position) { transform->position = position; }
	void SetPosition(float x, float y, float z) { transform->position.x = x; transform->position.y = y; transform->position.z = z; }
	XMFLOAT3 GetPosition() const { return transform->position; }
	void AddPosition(XMFLOAT3 pos);
	void ResetPosition();

	void SetRotation(XMFLOAT3 rotation) { transform->rotation = rotation; }
	void SetRotation(float x, float y, float z) { transform->rotation.x = x; transform->rotation.y = y; transform->rotation.z = z; }
	XMFLOAT3 GetRotation() const { return transform->rotation; }
	void AddRotation(float x, float y, float z);

	void SetScale(XMFLOAT3 scale) { transform->scale = scale; }
	void SetScale(float x, float y, float z) { transform->scale.x = x; transform->scale.y = y; transform->scale.z = z; }
	XMFLOAT3 GetScale() const { return transform->scale; }

	string GetType() const { return _type; }

	Geometry GetGeometryData() const { return appearance->geometry; }

	Material GetMaterial() const { return appearance->material; }

	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&_world); }

	void SetTextureRV(ID3D11ShaderResourceView * textureRV) { appearance->textureRV = textureRV; }
	ID3D11ShaderResourceView * GetTextureRV() const { return appearance->textureRV; }
	bool HasTexture() const { return appearance->textureRV ? true : false; }

	Transform* GetTransform() { return transform; }
	void SetTransform(Transform* newTra) { transform = newTra; }

	Rigidbody* GetRigidbody() { return rbd; }
	void SetRigidbody(Rigidbody* newRbd) { rbd = newRbd; }

	Appearance* GetAppearance() { return appearance; }
	void SetAppearance(Appearance* newAppearance) { appearance = newAppearance; }

	void Update(float t);
	void Draw(ID3D11DeviceContext * pImmediateContext);

	XMMATRIX mRotation;
private:
	Transform* transform;

	Rigidbody* rbd;
	Appearance* appearance;

	string _type;

	XMFLOAT4X4 _world;

	float deltaTime;

	XMFLOAT3 originalPos;
};

