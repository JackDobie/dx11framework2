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
void Debug::Print(XMFLOAT3 output)
{
	string x = to_string(output.x);
	string y = to_string(output.y);
	string z = to_string(output.z);
	string out = "X:" + x + ", Y:" + y + ", Z:" + z + "\n";
	OutputDebugStringA(out.c_str());
}
//void Debug::Print(bool output)
//{
//	string out = output == true ? "true\n" : "false\n";
//	OutputDebugStringA(out.c_str());
//}