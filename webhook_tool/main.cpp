#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <TlHelp32.h>

#include "guidev.h"
#include "Theme/Theme.h"
#include "imgui/imgui_impl_win32.h"
#include "menu.h"

#pragma comment(lib, "d3d9.lib")

bool reset_size = true;
bool ncmenu = true;
int tabs = 1;
Theme theme;

const std::string cmd1p1 = "curl -i -H \"Accept: application/json\" -H \"Content-Type:application/json\" -X POST --data \"{\\\"content\\\": \\\"";
const std::string cmd1p2 = "\\\"}\" ";

const std::string embed_cmd1p1 = "curl -i -H \"Accept: application/json\" -H \"Content-Type:application/json\" -X POST --data \"{\\\"content\\\": null, \\\"embeds\\\": [";
const std::string embed_cmd1p2 = "]}\" ";

std::vector<std::string> log_buffer;

bool spammer_active = false;
int spam_interval = 500;
int spam_count = 15;
int messages_sent = 0;
bool stop_requested = false;

void AddLog(const std::string& message) {
    log_buffer.push_back(message);
}

std::string GetISO8601Timestamp() {
    std::time_t t = std::time(nullptr);
    std::tm* tm_ptr = std::gmtime(&t); // utc time
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", tm_ptr);
    return std::string(buffer);
}

void SendText(std::string text, std::string webhook) {
    std::string cmd = cmd1p1 + text + cmd1p2 + webhook;
    WinExec(cmd.c_str(), SW_HIDE);

    AddLog(" [+] Sent Msg : " + text);
}

void SendEmbed(const std::string& embed_json, const std::string& webhook) {
    std::string escaped_embed_json;
    escaped_embed_json.reserve(embed_json.size() * 2); 

    for (char c : embed_json) {
        if (c == '"') {
            escaped_embed_json += "\\\"";
        }
        else if (c == '\\') {
            escaped_embed_json += "\\\\";
        }
        else {
            escaped_embed_json += c;
        }
    }

    std::string cmd = embed_cmd1p1 + escaped_embed_json + embed_cmd1p2 + webhook;
    WinExec(cmd.c_str(), SW_HIDE);
    AddLog(" [+] Sent Embed: " + embed_json);
}

void ExportLogToFile(const std::string& filename) {
    std::ofstream logFile(filename);
    if (logFile.is_open()) {
        for (const auto& log : log_buffer) {
            logFile << log << std::endl;
        }
        logFile.close();
        AddLog(" [!] Log exported to : " + filename); 
    }
    else {
        AddLog(" [X] Failed to open log file : " + filename);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"debug", NULL };
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"debug", WS_OVERLAPPEDWINDOW, 0, 0, 50, 50, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    theme.Register(io);

    /*ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 4.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }*/

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    //

    char message[500] = "";
    char webhook[500] = "";

    bool show_log_window = false;
    bool show_embed_builder = false;

    char embed_title[256] = "";
    char embed_description[1024] = "";
    char embed_footer[256] = "";
    char embed_footer_image_url[512] = "";
    char embed_image_url[512] = "";
    char embed_thumbnail_url[512] = "";
    char embed_field_name[256] = "";
    char embed_field_value[512] = "";
    bool field_inline = false;
    struct EmbedField {
        std::string name;
        std::string value;
        bool inline_field;
    };
    std::vector<EmbedField> embed_fields;
    bool include_timestamp = false;
    int embed_color_decimal = 16777215;

    //

    bool done = true;

    while (done) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = false;
        }
        if (!done)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RenderMenu(theme);
        ApplyDefaultColors();

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}
