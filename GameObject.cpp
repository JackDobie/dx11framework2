#include "GameObject.h"

GameObject::GameObject(string type, Geometry geometry, Material material)
{
	_type = type;
	appearance = new Appearance(geometry, material, nullptr);

	_parent = nullptr;
	transform = new Transform();
	particleModel = new ParticleModel(transform, true, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, true, deltaTime);
}

GameObject::GameObject(string type, Geometry geometry, Material material, Transform* _transform, bool _useConstAccel, float mass, bool gravity)
{
	_type = type;
	appearance = new Appearance(geometry, material, nullptr);

	_parent = nullptr;
	transform = _transform;
	particleModel = new ParticleModel(transform, _useConstAccel, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), mass, gravity, deltaTime);
}

GameObject::~GameObject()
{
	if (transform != nullptr) delete(transform);
	if (particleModel != nullptr) delete(particleModel);
	if (appearance != nullptr) delete(appearance);
}

void GameObject::Update(float t)
{
	deltaTime = t;

	// Calculate world matrix
	XMMATRIX mScale = XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z);
	XMMATRIX mRotation = XMMatrixRotationX(transform->rotation.x) * XMMatrixRotationY(transform->rotation.y) * XMMatrixRotationZ(transform->rotation.z);
	XMMATRIX mTranslation = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);

	XMStoreFloat4x4(&_world, mScale * mRotation * mTranslation);

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_world, this->GetWorldMatrix() * _parent->GetWorldMatrix());
	}

	particleModel->Update(t);
}

void GameObject::Draw(ID3D11DeviceContext * pImmediateContext)
{
	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place

	// Set vertex and index buffers
	pImmediateContext->IASetVertexBuffers(0, 1, &appearance->geometry.vertexBuffer, &appearance->geometry.vertexBufferStride, &appearance->geometry.vertexBufferOffset);
	pImmediateContext->IASetIndexBuffer(appearance->geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(appearance->geometry.numberOfIndices, 0, 0);
}

void GameObject::AddPosition(XMFLOAT3 pos)
{
	transform->position = XMFLOAT3(transform->position.x + (pos.x * deltaTime), transform->position.y + (pos.y * deltaTime), transform->position.z + (pos.z * deltaTime));
}
void GameObject::AddRotation(XMFLOAT3 rot)
{
	transform->rotation = XMFLOAT3(transform->rotation.x + (rot.x * deltaTime), transform->rotation.y + (rot.y * deltaTime), transform->rotation.z + (rot.z * deltaTime));
}