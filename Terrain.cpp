#include "Terrain.h"

Terrain::Terrain(ID3D11Device* d3dDevice, Geometry* _geometry)
{
	pd3dDevice = d3dDevice;
	geometry = _geometry;
	//pVertexBuffer = vertexBuffer;
	//pIndexBuffer = indexBuffer;
}

void Terrain::MakePlane(UINT rows, UINT columns, UINT width, UINT depth)
{
	UINT vertexCount = rows * columns;
	UINT indexCount = (rows) * (columns) * 2;
	geometry->numberOfIndices = indexCount;

	vector<SimpleVertex> verts;
	vector<WORD> indices;
	
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;
	
	// calculates distance between each row and column
	float dx = width / (columns - 1);
	float dz = depth / (rows - 1);

	verts.resize(vertexCount);

	float x = 0;
	float y = 0;
	float z = 0;
	for (UINT i = 0; i < rows; i++)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < columns; j++)
		{
			float x = -halfWidth + j * dx;
			SimpleVertex vertex;
			vertex.Pos = XMFLOAT3(x, y, z);
			vertex.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex.TexC.x = j * (columns - 1);
			vertex.TexC.y = i * (rows - 1);
	
			verts[i * columns + j] = vertex;
	
			//x += dx;
		}
		//z += dz;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verts.data();

	pd3dDevice->CreateBuffer(&bd, &InitData, &geometry->vertexBuffer);

	// resize to faces * 3 because 3 indices per face
	indices.resize(indexCount * 3);
	//abc = (i * n + j, i * n + j + 1, (i + 1) * n + j)
	//cbd = ((i + 1) * n + j, i * n + j + 1, (i + 1) * n + j + 1)
	//abc = XMFLOAT3(i * columns + j, i * columns + j + 1, (i + 1) * columns + j);
	//cbd = XMFLOAT3((i + 1) * columns + j, i * columns + j + 1, (i + 1) * columns + j + 1);

	// make tris from quads
	UINT k = 0;
	for (UINT i = 0; i < rows; i++)
	{
		for (UINT j = 0; j < columns; j++)
		{
			//abc
			indices[k] = i * columns + j;
			indices[k + 1] = i * columns + j + 1;
			indices[k + 2] = (i + 1) * columns + j;
			//cbd
			indices[k + 3] = (i + 1) * columns + j;
			indices[k + 4] = i * columns + j + 1;
			indices[k + 5] = (i + 1) * columns + j + 1;
			k += 6; // go to next quad
		}
	}

	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices.data();
	pd3dDevice->CreateBuffer(&bd, &InitData, &geometry->indexBuffer);
}

//MeshData Application::CreateGrid(UINT rows, UINT columns, UINT width, UINT depth)
//{
//	UINT vertexCount = rows * columns;
//
//	vector<SimpleVertex> verts;
//	vector<WORD> indices;
//
//	verts.resize(vertexCount);
//
//	// calculates distance between each row and column
//	float dx = width / (columns - 1);
//	float dz = depth / (rows - 1);
//
//	float x = 0;
//	float y = 0;
//	float z = 0;
//	for (UINT i = 0; i < rows; i++)
//	{
//		for (UINT j = 0; j < columns; j++)
//		{
//			SimpleVertex vertex;
//			vertex.Pos = XMFLOAT3(x, y, z);
//			vertex.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
//			vertex.TexC.x = j * (columns - 1);
//			vertex.TexC.y = i * (rows - 1);
//
//			verts[i * columns + j] = vertex;
//
//			x += dx;
//		}
//		z += dz;
//	}
//
//	
//
//	//auto verts = new XMFLOAT3[rows * columns];
//	////vector<XMFLOAT3> verts;
//	//float x = 0;
//	//float z = 0;
//	//for (int i = 0; i <= rows; i++)
//	//{
//	//	for (int j = 0; j <= columns; j++)
//	//	{
//	//		verts[i * columns + j] = XMFLOAT3(x, 0, z);
//	//		//verts.push_back(XMFLOAT3(x, 0, z));
//	//		x += width / (columns - 1);
//	//	}
//	//	x = 0;
//	//	z += depth / (rows - 1);
//	//}
//
//	//vector<XMFLOAT3> indices;
//	//XMFLOAT3 abc;
//	//XMFLOAT3 cbd;
//	//for (int i = 0; i <= rows; i++)
//	//{
//	//	for (int j = 0; j <= columns; j++)
//	//	{
//	//		//verts[i * columns + j]
//	//		//abc = (i * n + j, i * n + j + 1, (i + 1) * n + j)
//	//		//cbd = ((i + 1) * n + j, i * n + j + 1, (i + 1) * n + j + 1)
//	//		abc = XMFLOAT3(i * columns + j, i * columns + j + 1, (i + 1) * columns + j);
//	//		cbd = XMFLOAT3((i + 1) * columns + j, i * columns + j + 1, (i + 1) * columns + j + 1);
//	//		//indices.push_back(abc);
//	//		//indices.push_back(cbd);
//	//	}
//
//	//}
//
//	return MeshData();
//}