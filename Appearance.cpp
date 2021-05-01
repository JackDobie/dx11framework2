#include "Appearance.h"

Appearance::Appearance(Geometry geo, Material mat, ID3D11ShaderResourceView* texRV)
{
	geometry = geo;
	material = mat;
	textureRV = texRV;
}