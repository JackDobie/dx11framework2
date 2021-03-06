#include "GameObject.h"

GameObject::GameObject(string type, Geometry geometry, Material material) : _geometry(geometry), _type(type), _material(material)
{
	_parent = nullptr;
	_textureRV = nullptr;
	transform = Transform();
}

GameObject::~GameObject()
{
}

void GameObject::Update(float t)
{
	// Calculate world matrix
	XMMATRIX mScale = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
	XMMATRIX mRotation = XMMatrixRotationX(transform.rotation.x) * XMMatrixRotationY(transform.rotation.y) * XMMatrixRotationZ(transform.rotation.z);
	XMMATRIX mTranslation = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

	XMStoreFloat4x4(&_world, mScale * mRotation * mTranslation);

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_world, this->GetWorldMatrix() * _parent->GetWorldMatrix());
	}

	if(_type.find("Cube") != string::npos)
	{
		Debug::Print(t);
	}
	/*static int x = 0;
	Debug::Print(x);
	x++;*/
}

void GameObject::Draw(ID3D11DeviceContext * pImmediateContext)
{
	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place

	// Set vertex and index buffers
	pImmediateContext->IASetVertexBuffers(0, 1, &_geometry.vertexBuffer, &_geometry.vertexBufferStride, &_geometry.vertexBufferOffset);
	pImmediateContext->IASetIndexBuffer(_geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(_geometry.numberOfIndices, 0, 0);
}
