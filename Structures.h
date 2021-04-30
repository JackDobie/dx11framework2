#pragma once
#include <cstring>
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

// This could be a class however its functionality is going to tie heavily into the HMD
//struct Camera
//{
//	XMFLOAT4	eye;
//	XMFLOAT4	at;	
//	XMFLOAT4	up;
//	
//	XMFLOAT4X4				view;
//	XMFLOAT4X4              projection;
//};
//
//struct RenderImage
//{
//	ID3D11RenderTargetView* _pRenderTargetView;
//	ID3D11Texture2D* _RenderTargetTexture;
//	ID3D11ShaderResourceView* _shaderResourceView;
//	ID3D11Buffer* _RenderImageVertexBuffer;
//	ID3D11Buffer* _RenderImageIndexBuffer;
//};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

//struct ConstantBuffer
//{
//	XMMATRIX mWorld;
//	XMMATRIX mView;
//	XMMATRIX mProjection;
//	XMFLOAT4 DiffuseMtrl;
//	XMFLOAT4 DiffuseLight;
//	XMFLOAT4 AmbientMtrl;
//	XMFLOAT4 AmbientLight;
//	XMFLOAT4 SpecularMtrl;
//	XMFLOAT4 SpecularLight;
//	XMFLOAT3 LightVecW;
//	float gTime;
//	XMFLOAT3 EyePosW;
//	float SpecularPower;
//	unsigned int activeViewport;
//};

struct MeshData
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
	XMFLOAT3 CentreOfMass;
	XMFLOAT3 ModelDimensions;
};

struct Geometry
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;

	XMFLOAT3 centreOfMass;

	XMFLOAT3 modelDimensions;
};

struct Material
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	float specularPower;
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(XMFLOAT3 float3)
	{
		this->x = float3.x;
		this->y = float3.y;
		this->z = float3.z;
	}
};

struct AABBFace
{
	float minX;
	float maxX;

	float minY;
	float maxY;

	float minZ;
	float maxZ;
};