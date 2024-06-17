#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <chrono>

#include <Global.h>

// original funcs
void* onSendKey;
void* fovBack;
void* sensBack;
void* onChat;
void* onSendMouse;
void* oPresent;

std::chrono::system_clock::time_point mEnabledStart;
bool mEnabled = false;

bool pToggle = false;
bool initContext = false;
int Width = 0;
int Height = 0;
float currentFOV = -1.f;

ID3D11Device* d3d11Device = nullptr;
ID3D12Device5* d3d12Device = nullptr;
static ImFont* DefaultFont = nullptr;


void MouseDetourHook(void* _this, __int8 mouseButton, bool isDown, __int16 mouseX, __int16 mouseY, __int16 relX, __int16 relY, int a8)
{
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        Global::MousePos = ImVec2(mouseX, mouseY);

        switch (mouseButton)
        {
            case 1:
                io.MouseDown[0] = isDown;
                break;
        
            case 2:
                io.MouseDown[1] = isDown;
                break;
        
            case 3:
                io.MouseDown[2] = isDown;
                break;

            case 4:
                io.MouseWheel = isDown > 136 ? -0.5f : 0.5f;

                if (Global::Keymap['C'])
                    Global::ZoomMag += isDown < 136 ? -2.f : 2.f;

                break;
        }

        if (io.WantCaptureMouse)
            return;
    }

    CallFunc<void*, void*, __int8, bool, __int16, __int16, __int16, __int16, char>(
        onSendMouse, _this, mouseButton, isDown, mouseX, mouseY, relX, relY, a8
    );
}

float fovHook(LevelRendererPlayer* self, float a, bool a2) {
    float read = ((float(*)(LevelRendererPlayer*, float, bool))fovBack)(self, a, a2);
    
    if (!mEnabled) {
        currentFOV = -1.f;
        return read;
    }

    if (currentFOV == -1.f)
        currentFOV = read;

    Global::DefFOV = read;

    float targetFOV = (Global::Mode == 0) ? Global::ZoomMag : Global::DefFOV;

    if (Global::Mode == 1)
        currentFOV = targetFOV;

    else if (Global::Mode == 0)
        currentFOV += (targetFOV - currentFOV) * Global::ZoomSpeed * ImGui::GetIO().DeltaTime;

    return max(currentFOV, read);
}

float sensitivityHook(BaseOptions* self, unsigned int inputMode) {
    if (!mEnabled)
        return ((float(*)(BaseOptions*, unsigned int))sensBack)(self, inputMode);

    float currentSensitivity = ((float(*)(BaseOptions*, unsigned int))sensBack)(self, inputMode);

    return currentSensitivity * (Global::ZoomMag / Global::DefFOV);
}

void KeymapDetourHook(uint64_t key, bool held)
{
    Global::Keymap[key] = held;

    CallFunc<void*, uint64_t, bool>(
        onSendKey, key, held
    );
}

// UI


void callRender()
{
    if (!Global::RenderWindow)
        return;

    ImGui::Text(Global::header);

    ImGui::SliderFloat("Zoom FOV", &Global::ZoomMag, 0.f, 180.f);
    ImGui::SliderFloat("Zoom Speed", &Global::ZoomSpeed, 0.1f, 5.f);
    ImGui::SliderFloat("Sens. Scale", &Global::SensScale, 10.f, 100.f);

    ImGui::Combo("Mode", &Global::Mode, Global::Modes, IM_ARRAYSIZE(Global::Modes));

    ImGui::Checkbox("Dev Info", &Global::DevInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Hold Zoom", &Global::HoldZoom);
    ImGui::SameLine();
    ImGui::Checkbox("Uncap FPS", &Global::NoVsync);
    ImGui::SameLine();

    if (ImGui::Button("Eject"))
        Global::isRunning = false;

    if (Global::HoldZoom)
        mEnabled = true;

    if (Global::DevInfo)
    {
        ImGui::Text("FPS: %.1f", round(ImGui::GetIO().Framerate));
        ImGui::Text("Zoom: %.3f", currentFOV);
        ImGui::Text("Loaded Hooks:");

        ImGui::BulletText("%s FOV (Minecraft+%p)"        , Sigs::aFOV != 0x0 ? "[ERR]" : "[OK]", shiftAddress(Sigs::aFOV, Global::base));
        ImGui::BulletText("%s Sensitivity (Minecraft+%p)", Sigs::aSens != 0x0 ? "[ERR]" : "[OK]", shiftAddress(Sigs::aSens, Global::base));
        ImGui::BulletText("%s Mouse (Minecraft+%p)"      , Sigs::aMouse != 0x0 ? "[ERR]" : "[OK]", shiftAddress(Sigs::aMouse, Global::base));
        ImGui::BulletText("%s Keymap (Minecraft+%p)"     , Sigs::aKeymap != 0x0 ? "[ERR]" : "[OK]", shiftAddress(Sigs::aKeymap, Global::base));
    }
}


void loadFonts() {
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    DefaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ImGuiStyle& style = ImGui::GetStyle();

    {
        // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
        style.Alpha = 1.0f;
        style.FrameRounding = 3.0f;
        style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    }

    float _alpha = 0.5f;

    for (int i = 0; i <= ImGuiCol_COUNT; i++)
    {
        ImVec4& col = style.Colors[i];
        float H, S, V;

        ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);
        if (S < 0.1f)
            V = 1.0f - V;

        ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
        if (col.w < 1.00f)
            col.w *= _alpha;
    }
}

HRESULT D3D12_PresentDetour(IDXGISwapChain3* swapchain, UINT syncInterval, UINT flags) {
    Global::Window = FindWindowA(nullptr, "Minecraft");

    if (!Global::Window) Global::Window = FindWindowA(nullptr, "Minecraft: Developer Edition");
    if (!Global::Window) return CallFunc<HRESULT, IDXGISwapChain3*, UINT, UINT>(oPresent, swapchain, syncInterval, flags);

    if (!SUCCEEDED(swapchain->GetDevice(IID_PPV_ARGS(&d3d11Device)))) {
        if (SUCCEEDED(swapchain->GetDevice(IID_PPV_ARGS(&d3d12Device)))) {
            swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            static_cast<ID3D12Device5*>(d3d12Device)->RemoveDevice();
        }

        return CallFunc<HRESULT, IDXGISwapChain3*, UINT, UINT>(oPresent, swapchain, syncInterval, flags);
    }

    ID3D11DeviceContext* ppContext = nullptr;
    d3d11Device->GetImmediateContext(&ppContext);
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    ID3D11RenderTargetView* mainRenderTargetView;
    d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();

    if (!initContext) {
        ImGui::CreateContext();
        loadFonts();
        ImGui_ImplWin32_Init(Global::Window);
        ImGui_ImplDX11_Init(d3d11Device, ppContext);
        initContext = true;
    };

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetMousePos(Global::MousePos);

    if (Global::Keymap[VK_INSERT] && Global::Keymap[VK_INSERT] != pToggle)
        Global::RenderWindow = !Global::RenderWindow;

    if (!Global::HoldZoom)
        mEnabled = Global::Keymap['C'];

    pToggle = Global::Keymap[VK_INSERT];

    callRender();

    ImGui::EndFrame();
    ImGui::Render();

    ppContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    mainRenderTargetView->Release();
    d3d11Device->Release();

    if (Global::NoVsync)
        return CallFunc<HRESULT, IDXGISwapChain3*, UINT, UINT>(oPresent, swapchain, syncInterval, DXGI_PRESENT_DO_NOT_WAIT);

    else
        return CallFunc<HRESULT, IDXGISwapChain3*, UINT, UINT>(oPresent, swapchain, syncInterval, flags);
}


void EnableAll() {
    if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
        c_log("Loaded Kiero D3D12!\n");
    else if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        c_log("Loaded Kiero D3D11!\n");

    const int index = kiero::getRenderType() == kiero::RenderType::D3D12 ? 140 : 8;
    kiero::bind(index, (void**)&oPresent, D3D12_PresentDetour);

    Sigs::aFOV = findSig(Sigs::GetFOV);
    Sigs::aSens = findSig(Sigs::GetSens);
    Sigs::aMouse = findSig(Sigs::Mouse);
    Sigs::aKeymap = findSig(Sigs::Keymap);

    HookFunction(Sigs::aKeymap, (void*)&KeymapDetourHook, &onSendKey, "onKey");
    HookFunction(Sigs::aMouse, (void*)&MouseDetourHook, &onSendMouse, "onMouse");
    HookFunction(Sigs::aFOV, (void*)&fovHook, &fovBack, "getFOV");
    HookFunction(Sigs::aSens, (void*)&sensitivityHook, &sensBack, "getSensitivity");
}