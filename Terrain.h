#pragma once
#include <d3d11.h>
#include <vector>
#include <fstream>
#include "Structures.h"
#include "Debug.h"
using namespace std;

class Terrain
{
public:
	Terrain(ID3D11Device* d3dDevice, Geometry* _geometry);
	void MakePlane(UINT rows, UINT columns, UINT width, UINT depth, string heightmap);
	vector<float>* LoadHeightMap(UINT width, UINT height, string heightmap);
private:
	ID3D11Device* pd3dDevice;
	Geometry* geometry;
	vector<float> heightMapVec;
};