#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "injector.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// -- UI HELPERS --
void Checkboxes(const char* label, bool* v) { ImGui::Checkbox(label, v); }
void DrawButton(const char* label) { ImGui::Button(label, ImVec2(150, 30)); }

// -- GLOBALS --
HWND g_hWnd = NULL;
bool g_IsInjected = false;
bool g_ShouldClose = false;

// -- CHEAT STATE (100 Hacks) --
bool godmode[50] = {true, false, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
const char* hack_names[] = {
    "GodMode", "Invis", "Bring Freeze", "Unfreeze", "Auto Gun", "Unlock Cam",
    "ESP Players", "ESP Items", "Skeleton", "Chams", "Fly", "NoFall", "Sprint", "NoClip", "Teleport",
    "Aimbot", "Auto Fire", "Rapid Fire", "No Recoil", "Hitbox", "Speed Hack", "Walk Speed", "Super Jump"
};

// -- WINDOW PROC --
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) return true;
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// -- MAIN ENTRY POINT --
int main() {
    WNDCLASSEXW wc = { sizeof(wc), CS_HREDRAW | CS_VREDRAW, WindowProc, 0L, 0L,
                       GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                       L"Foxname", NULL };
    RegisterClassExW(&wc);

    g_hWnd = CreateWindowExW(0, wc.lpszClassName, L"Foxname - Universal Framework",
                            WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, wc.hInstance, NULL);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(g_hWnd); // Init Win32

    // THEME - Dark/Glass/Red
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.95f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.2f, 0.2f, 0.8f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);

    // LOOP
    MSG msg; ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (g_ShouldClose) break;
        } else {
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // DRAW UI
            ImGui::SetNextWindowPos(ImVec2(50, 50));
            ImGui::SetNextWindowSize(ImVec2(550, 750));
            if (ImGui::Begin("Foxname - Universal Framework", NULL, ImGuiWindowFlags_NoCollapse)) {
                
                // HEADER
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "🦊 Foxname - Roblox Framework");
                ImGui::Separator();

                // INJECT BUTTON
                if (!g_IsInjected) {
                    if (ImGui::Button(g_IsInjected ? "Injected ✓" : "Inject Game", ImVec2(-1, 30))) {
                        InjectGameLogic(g_hWnd);
                    }
                } else {
                    ImGui::BeginDisabled();
                    ImGui::Button("Injected ✓");
                    ImGui::EndDisabled();
                    // VIBRANT TEXT TO SHOW VIRUS IS COUNTING DOWN
                    if (g_IsVirusActive) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        ImGui::Text("Payload Active: 5 Minute Timer Started...");
                        ImGui::PopStyleColor();
                    }
                }

                // HACKS TABS
                static int tab = 0;
                if (ImGui::BeginTabBar("Hacks")) {
                    if (ImGui::BeginTabItem("General")) {
                        Checkboxes("GodMode", &godmode[0]);
                        Checkboxes("Invisibility", &godmode[1]);
                        Checkboxes("Bring + Freeze", &godmode[2]);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Movement")) {
                        Checkboxes("Fly Mode", &godmode[10]);
                        Checkboxes("NoFall", &godmode[11]);
                        Checkboxes("SpeedHack", &godmode[20]);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::End();

            // RENDER
            ImGui::Render();
            ImGui_ImplWin32_RenderDrawData(ImGui::GetDrawData());
        }
    }

    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
