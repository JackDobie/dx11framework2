#include "Rigidbody.h"

XMVECTOR Rigidbody::Torque(XMVECTOR force, XMVECTOR point)
{
	return XMVector3Cross(point, force);
}