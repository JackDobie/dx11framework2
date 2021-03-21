#include "Transform.h"

Transform::Transform()
{
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}
Transform::Transform(XMFLOAT3 _position)
{
	position = _position;
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}
Transform::Transform(XMFLOAT3 _position, XMFLOAT3 _rotation)
{
	position = _position;
	rotation = _rotation;
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}
Transform::Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale)
{
	position = _position;
	rotation = _rotation;
	scale = _scale;
}