#include "Vector.h"

Vector::Vector()
{
	x = 0;
	y = 0;
	z = 0;
}
Vector::Vector(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

float Vector::Magnitude()
{
	float mag = sqrtf((x * x) + (y * y) + (z * z));
	return mag;
}

Vector* Vector::Normalise()
{
	float m = Magnitude();
	return &Vector((x / m), (y / m), (z / m));
}