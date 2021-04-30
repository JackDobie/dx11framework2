#include "ImGUIManager.h"

ImGUIManager::ImGUIManager(ID3D11Device* device, ID3D11DeviceContext* context, HWND* hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);
	ImGui::StyleColorsDark();
}

void ImGUIManager::Update()
{
    // Feed inputs to dear imgui, start new frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGUIManager::Draw()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}