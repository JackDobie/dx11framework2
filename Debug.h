#pragma once

#include <string>
#include <Windows.h>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

class Debug
{
public:
	Debug();
	static void Print(string output);
	static void Print(int output);
	static void Print(float output);
	static void Print(XMFLOAT3 output);
	//static void Print(bool output);
};

