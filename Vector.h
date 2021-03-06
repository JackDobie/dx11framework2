#pragma once
#include <math.h>

class Vector
{
public:
	Vector();
	Vector(float _x, float _y, float _z);

	Vector* DotProd(Vector* otherVec);
	Vector* CrossProd(Vector* otherVec);
	float Magnitude();
	Vector* Normalise();

	float x;
	float y;
	float z;
};