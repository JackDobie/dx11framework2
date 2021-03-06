#include "Debug.h"

Debug::Debug()
{
}

void Debug::Print(string output)
{
	OutputDebugStringA(output.c_str());
}
void Debug::Print(int output)
{
	char sz[1024] = { 0 };
	sprintf_s(sz, "%d \n", output);
	OutputDebugStringA(sz);
}
void Debug::Print(float output)
{
	char sz[1024] = { 0 };
	sprintf_s(sz, "%f \n", output);
	OutputDebugStringA(sz);
}