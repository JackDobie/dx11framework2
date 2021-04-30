#pragma once
#include <d3d11.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class ImGUIManager
{
public:
	ImGUIManager(ID3D11Device* device, ID3D11DeviceContext* context, HWND* hwnd);

	void Update();
	void Draw();
};