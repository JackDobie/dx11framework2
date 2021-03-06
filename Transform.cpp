#include "Transform.h"

Transform::Transform()
{
	position = XMFLOAT3();
	rotation = XMFLOAT3();
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}
Transform::Transform(XMFLOAT3 _position)
{
	position = _position;
	rotation = XMFLOAT3();
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