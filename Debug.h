#pragma once

#include <string>
#include <Windows.h>
using namespace std;

class Debug
{
public:
	Debug();
	static void Print(string output);
	static void Print(int output);
	static void Print(float output);
};

