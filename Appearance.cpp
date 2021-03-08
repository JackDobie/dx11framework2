#include "Appearance.h"

//Appearance::Appearance()
//{
//	geometry = Geometry();
//	material = Material();
//	textureRV = nullptr;
//}
Appearance::Appearance(Geometry geo, Material mat)
{
	geometry = geo;
	material = mat;
	textureRV = nullptr;
}
Appearance::Appearance(Geometry geo, Material mat, ID3D11ShaderResourceView* texRV)
{
	geometry = geo;
	material = mat;
	textureRV = texRV;
}

Appearance::~Appearance()
{
	if (textureRV != nullptr) delete(textureRV);
}