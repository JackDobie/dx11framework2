#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "Structures.h"

using namespace DirectX;

class Appearance
{
public:
	//Appearance();
	//Appearance(Geometry geo, Material mat);
	Appearance(Geometry geo, Material mat, ID3D11ShaderResourceView* texRV);

	Geometry geometry;
	Material material;
	ID3D11ShaderResourceView* textureRV;
};