#include "GameObject.h"

GameObject::GameObject(string type, Geometry geometry, Material material, Transform* _transform, bool _useConstAccel, float mass, bool gravity, bool enableCollision)
{
	_type = type;
	appearance = new Appearance(geometry, material, nullptr);

	transform = _transform;
	rbd = new Rigidbody(geometry.modelDimensions, transform, _useConstAccel, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), mass, gravity, deltaTime, enableCollision, geometry);
	mRotation = XMMatrixRotationRollPitchYaw(transform->rotation.x, transform->rotation.y, transform->rotation.z);
	originalPos = transform->position;
	rbd->orientation = mRotation;
}

GameObject::~GameObject()
{
	if (transform)
	{
		delete transform;
		transform = nullptr;
	}
	if (rbd)
	{
		delete rbd;
		rbd = nullptr;
	}

	if (appearance)
	{
		delete appearance;
		appearance = nullptr;
	}
}

void GameObject::Update(float t)
{
	deltaTime = t;
	
	// Calculate world matrix
	XMMATRIX mScale = XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z);
	
	//create rotation matrix
	if (!(_type.find("Terrain") != string::npos))
	{
		if (*rbd->GetRotating())
		{
			rbd->Rotate(deltaTime);
		}
		mRotation =	rbd->CalcOrientation(deltaTime);
	}
	else
	{
		mRotation = XMMatrixRotationRollPitchYaw(transform->rotation.x, transform->rotation.y, transform->rotation.z);
		XMFLOAT4 fRotation;
		XMStoreFloat4(&fRotation, XMQuaternionRotationMatrix(mRotation));
		//turn matrix into quaternion
		Quaternion quatRotation = Quaternion(fRotation.w, fRotation.x, fRotation.y, fRotation.z);
		quatRotation.normalise();
		//apply transformation matrix
		CalculateTransformMatrixRowMajor(mRotation, transform->position, quatRotation);
	}

	XMMATRIX mTranslation = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);

	XMStoreFloat4x4(&_world, mScale * mRotation * mTranslation);

	rbd->Update(t);
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
void GameObject::AddRotation(float x, float y, float z)
{
	transform->rotation = XMFLOAT3(transform->rotation.x + (x * deltaTime), transform->rotation.y + (y * deltaTime), transform->rotation.z + (z * deltaTime));
	rbd->orientation = XMMatrixRotationRollPitchYaw(transform->rotation.x, transform->rotation.y, transform->rotation.z);
}

void GameObject::ResetPosition()
{
	transform->position = originalPos;
}