#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* _transform)
{
	transform = _transform;
}
ParticleModel::~ParticleModel()
{
	if (transform == nullptr) delete(transform);
}

void ParticleModel::Update(float t)
{

}

void ParticleModel::MovePosition(XMFLOAT3 pos)
{
	transform->position = XMFLOAT3(transform->position.x + pos.x, transform->position.y + pos.y, transform->position.z + pos.z);
}
void ParticleModel::ChangeRotation(XMFLOAT3 rot)
{
	transform->rotation = XMFLOAT3(transform->rotation.x + rot.x, transform->rotation.y + rot.y, transform->rotation.z + rot.z);
}