#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Transform
{
public:
	Transform();
	Transform(XMFLOAT3 _position);
	Transform(XMFLOAT3 _position, XMFLOAT3 _rotation);
	Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale);

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
};