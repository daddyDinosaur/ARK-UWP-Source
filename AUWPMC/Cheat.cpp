#include "Menu.h"
#include "Helpers/random/byte.h"
#include "cheat.h"

bool NearbyNoglin = false;
std::string CompareName;


void Renderer::RemoveInput()
{
    if (D3D.WndProcOriginal)
    {
        SetWindowLongPtrA(D3D.GameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(D3D.WndProcOriginal));
        D3D.WndProcOriginal = nullptr;
    }
}

void Renderer::HookInput()
{
    Renderer::RemoveInput();
    D3D.WndProcOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(D3D.GameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
}

bool Renderer::Remove()
{
    Renderer::RemoveInput();
    if (!RemoveHook(D3D.OriginalPresent) || !RemoveHook(D3D.SetCursorPosOriginal) || !RemoveHook(D3D.SetCursorOriginal))
    {
        return false;
    }
    if (D3D.RenderTargetView)
    {
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
        D3D.RenderTargetView->Release();
        D3D.RenderTargetView = nullptr;
    }
    if (D3D.Ctx)
    {
        D3D.Ctx->Release();
        D3D.Ctx = nullptr;
    }
    if (D3D.Device)
    {
        D3D.Device->Release();
        D3D.Device = nullptr;
    }
    return true;
}

BOOL WINAPI Renderer::SetCursorPosHook(int X, int Y)
{
    if (Settings.IsMenuOpen) return FALSE;
    return D3D.SetCursorPosOriginal(X, Y);
}

HCURSOR WINAPI Renderer::SetCursorHook(HCURSOR hCursor)
{
    if (Settings.IsMenuOpen) return 0;
    return D3D.SetCursorOriginal(hCursor);
}

bool Renderer::CreateView()
{
    ID3D11Texture2D* Buffer = nullptr;
    if (FAILED(D3D.SwapChain->GetBuffer(0, __uuidof(Buffer), reinterpret_cast<PVOID*>(&Buffer)))) return false;
    if (FAILED(D3D.Device->CreateRenderTargetView(Buffer, nullptr, &D3D.RenderTargetView))) return false;
    Buffer->Release();
    return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI Renderer::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }
    switch (msg)
    {
    case WM_SIZE:
        if (D3D.Device != nullptr && wParam != SIZE_MINIMIZED)
        {
            if (D3D.RenderTargetView)
            {
                D3D.RenderTargetView->Release();
                D3D.RenderTargetView = nullptr;
            }
            D3D.SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
            CreateView();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

inline bool Renderer::Init()
{

    D3D.PresentFunc = GetD3D11PresentFunction();
    SetHook(D3D.PresentFunc, D3D_HOOK, reinterpret_cast<void**>(&D3D.OriginalPresent));
    SetHook(SetCursorPos, SetCursorPosHook, reinterpret_cast<void**>(&D3D.SetCursorPosOriginal));
    SetHook(SetCursor, SetCursorHook, reinterpret_cast<void**>(&D3D.SetCursorOriginal));
    SetHook(reinterpret_cast<void*>(Cache.GameBase + 0x173CA60), Utils::PE_HOOK, reinterpret_cast<PVOID*>(&Settings.OriginalPE));
    SetHook(reinterpret_cast<void*>(Cache.GameBase + 0x866D50), Utils::GetAdjustedAim, reinterpret_cast<PVOID*>(&Cache.OriginalGetAdjustedAim));
    return true;
}

inline void Renderer::Drawing::RenderText(ImVec2 ScreenPosition, ImColor Color, const char* Text, int WidthText)
{
    bool Center = true;
    std::stringstream Stream(Text);
    std::string Line;
    float Y = 0.0f;
    int Index = 0;
    auto FontSize = ImGui::GetFontSize();
    auto Font = ImGui::GetFont();
    while (std::getline(Stream, Line))
    {
        ImVec2 TextSize = ImVec2(0, 0);
        TextSize = ImGui::GetFont()->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Line.c_str());
        if (Font) TextSize = Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Line.c_str());
        if (Center)
        {
            ImGui::GetBackgroundDrawList()->AddText(Font, FontSize, ImVec2((ScreenPosition.x - TextSize.x / 2.0f) + 1, (ScreenPosition.y + TextSize.y * Index) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), Line.c_str());
            ImGui::GetBackgroundDrawList()->AddText(Font, FontSize, ImVec2((ScreenPosition.x - TextSize.x / 2.0f) - 1, (ScreenPosition.y + TextSize.y * Index) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), Line.c_str());
            ImGui::GetBackgroundDrawList()->AddText(Font, FontSize, ImVec2(ScreenPosition.x - TextSize.x / 2.0f, ScreenPosition.y + TextSize.y * Index), Color, Line.c_str());
        }
        else
        {
            ImGui::GetBackgroundDrawList()->AddText(Font, FontSize, ImVec2((ScreenPosition.x) + 1, (ScreenPosition.y + TextSize.y * Index) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), Line.c_str());
            ImGui::GetBackgroundDrawList()->AddText(Font, FontSize, ImVec2((ScreenPosition.x) - 1, (ScreenPosition.y + TextSize.y * Index) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), Line.c_str());
            ImGui::GetBackgroundDrawList()->AddText(Font, FontSize, ImVec2(ScreenPosition.x, ScreenPosition.y + TextSize.y * Index), Color, Line.c_str());
        }
        Y = ScreenPosition.y + TextSize.y * (Index + 1);
        Index++;
    }
}

inline void Renderer::Drawing::RenderText2(ImVec2 ScreenPosition, ImColor Color, const char* Text, int WidthText)
{
    if (!Text) return;
    auto ImScreen = *reinterpret_cast<const ImVec2*>(&ScreenPosition);
    if (ScreenPosition.x > 0 && ScreenPosition.y > 0 && Cache.WindowSizeX > ScreenPosition.x && Cache.WindowSizeY > ScreenPosition.y)
    {
        auto Size = ImGui::CalcTextSize(Text);
        ImScreen.x -= Size.x * 0.5f;
        ImScreen.y -= Size.y;
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 20, ImScreen, Color, Text, Text + strlen(Text), WidthText);
    }
}

inline void Renderer::Drawing::RenderCollapseFriendlyDisplayList(ImVec2 ScreenStartPosition, ImColor Color, std::vector<std::string> DisplayArray, float HeightFactor)
{
    auto StartPosition = *reinterpret_cast<const ImVec2*>(&ScreenStartPosition);
    for (size_t s = 0; s < DisplayArray.size(); s++)
    {
        auto& DisplayString = DisplayArray[s];
        if (DisplayString.length() < 1) continue;
        auto CurrentPosition = ImVec2(StartPosition.x, StartPosition.y);
        auto StringSize = ImGui::CalcTextSize(DisplayString.c_str());
        CurrentPosition.y += StringSize.y;
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(CurrentPosition.x, StartPosition.y), Color, DisplayString.c_str(), DisplayString.c_str() + strlen(DisplayString.c_str()), 500);
        StartPosition.y += HeightFactor;
    }
}

inline void Renderer::Drawing::RenderCrosshair(ImColor Color, int Thickness)
{
    auto DrawList = ImGui::GetBackgroundDrawList();
    auto WinSizeX = ImGui::GetWindowSize().x;
    auto WinSizeY = ImGui::GetWindowSize().y;
    DrawList->AddLine(ImVec2(WinSizeX / 2, WinSizeY / 2), ImVec2(WinSizeX / 2, WinSizeY / 2 - Settings.Visuals.CrosshairSize), Color, Thickness);
    DrawList->AddLine(ImVec2(WinSizeX / 2, WinSizeY / 2), ImVec2(WinSizeX / 2, WinSizeY / 2 + Settings.Visuals.CrosshairSize), Color, Thickness);
    DrawList->AddLine(ImVec2(WinSizeX / 2, WinSizeY / 2), ImVec2(WinSizeX / 2 - Settings.Visuals.CrosshairSize, WinSizeY / 2), Color, Thickness);
    DrawList->AddLine(ImVec2(WinSizeX / 2, WinSizeY / 2), ImVec2(WinSizeX / 2 + Settings.Visuals.CrosshairSize, WinSizeY / 2), Color, Thickness);
}

void Utils::PE_HOOK(void* obj, UFunction* fn, void* params)
{
    if (GetAsyncKeyState(VK_DELETE) & 0x0001) Settings.IsMenuOpen = !Settings.IsMenuOpen, Renderer::RemoveInput();
    return Settings.OriginalPE(obj, fn, params);
}

FVector* Utils::GetAdjustedAim(AShooterWeapon* Weapon, FVector* Result) {
    if (Cache.AimbotTarget && Settings.Aimbot.SilentAim)
    {
        FVector BoneLocation;
        Settings.GetBoneLocation(Cache.AimbotTarget->MeshComponent, &BoneLocation, Cache.AimbotTarget->MeshComponent->GetBoneName(AimBone), 0);
        FVector AimDirection = Cache.LocalPlayer->GetDirectionVector(Cache.LPC->PlayerCameraManager->GetCameraLocation(), BoneLocation);
        *Result = AimDirection;
        if (Result->X == 0 || Result->Y == 0 || Result->Z == 0) return Cache.OriginalGetAdjustedAim(Weapon, Result);
        return Result;
    }
    return Cache.OriginalGetAdjustedAim(Weapon, Result);
}

inline void Renderer::Drawing::RenderAimFOV(ImColor Color)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(Cache.WindowSizeX / 2, Cache.WindowSizeY / 2), Settings.Visuals.FOVSize, Color, 100, 1.0f);
}

inline int Renderer::Drawing::ReturnDistance(int X1, int Y1, int X2, int Y2) { return sqrt(pow(X2 - X1, 2) + pow(Y2 - Y1, 2) * 1); }

inline bool Renderer::Drawing::WithinAimFOV(int CircleX, int CircleY, int R, int X, int Y)
{
    int Dist = (X - CircleX) * (X - CircleX) + (Y - CircleY) * (Y - CircleY);
    if (Dist <= R * R) return true;
    else return false;
}

inline bool Renderer::Drawing::RenderPlayerSkeleton(USkeletalMeshComponent* Mesh, int Gender, ImColor Color)
{
    FVector BoneWorldLocation;
    // Spine
    FVector2D Head;
    FVector2D Neck;
    FVector2D Spine;
    FVector2D Pelvis;
    // Left Arm
    FVector2D LeftShoulder;
    FVector2D LeftElbow;
    FVector2D LeftWrist;
    // Right Arm
    FVector2D RightShoulder;
    FVector2D RightElbow;
    FVector2D RightWrist;
    // Left Leg
    FVector2D LeftKnee;
    FVector2D LeftAnkle;
    // Right Leg
    FVector2D RightKnee;
    FVector2D RightAnkle;

    // Spine
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(8), 0);
    if (!W2S(BoneWorldLocation, Head)) return false;
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(6), 0);
    if (!W2S(BoneWorldLocation, Neck)) return false;
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(4), 0);
    if (!W2S(BoneWorldLocation, Spine)) return false;
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(1), 0);
    if (!W2S(BoneWorldLocation, Pelvis)) return false;
    // Left Arm
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(33), 0);
    if (!W2S(BoneWorldLocation, LeftShoulder)) return false;
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(36), 0);
    if (!W2S(BoneWorldLocation, LeftElbow)) return false;
    Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(38), 0);
    if (!W2S(BoneWorldLocation, LeftWrist)) return false;
    // Right Arm
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(57), 0); } // Male
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(59), 0); } // Female
    if (!W2S(BoneWorldLocation, RightShoulder)) return false;
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(60), 0); }
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(62), 0); }
    if (!W2S(BoneWorldLocation, RightElbow)) return false;
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(62), 0); }
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(64), 0); }
    if (!W2S(BoneWorldLocation, RightWrist)) return false;
    // Left Leg
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(82), 0); } // Male
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(84), 0); } // Female
    if (!W2S(BoneWorldLocation, LeftKnee)) return false;
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(84), 0); } // Male
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(86), 0); } // Female
    if (!W2S(BoneWorldLocation, LeftAnkle)) return false;
    // Right Leg
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(88), 0); } // Male
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(90), 0); } // Female
    if (!W2S(BoneWorldLocation, RightKnee)) return false;
    if (Gender == 1) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(90), 0); } // Male
    else if (Gender == 2) { Settings.GetBoneLocation(Mesh, &BoneWorldLocation, Mesh->GetBoneName(92), 0); } // Female
    if (!W2S(BoneWorldLocation, RightAnkle)) return false;

    auto DrawList = ImGui::GetBackgroundDrawList();
    DrawList->AddLine(ImVec2(LeftAnkle.X, LeftAnkle.Y), ImVec2(LeftKnee.X, LeftKnee.Y), Color);
    DrawList->AddLine(ImVec2(RightAnkle.X, RightAnkle.Y), ImVec2(RightKnee.X, RightKnee.Y), Color);
    DrawList->AddLine(ImVec2(LeftKnee.X, LeftKnee.Y), ImVec2(Pelvis.X, Pelvis.Y), Color);
    DrawList->AddLine(ImVec2(RightKnee.X, RightKnee.Y), ImVec2(Pelvis.X, Pelvis.Y), Color);
    DrawList->AddLine(ImVec2(Pelvis.X, Pelvis.Y), ImVec2(Spine.X, Spine.Y), Color);
    DrawList->AddLine(ImVec2(Spine.X, Spine.Y), ImVec2(Neck.X, Neck.Y), Color);
    DrawList->AddLine(ImVec2(Neck.X, Neck.Y), ImVec2(Head.X, Head.Y), Color);
    DrawList->AddLine(ImVec2(LeftWrist.X, LeftWrist.Y), ImVec2(LeftElbow.X, LeftElbow.Y), Color);
    DrawList->AddLine(ImVec2(LeftElbow.X, LeftElbow.Y), ImVec2(LeftShoulder.X, LeftShoulder.Y), Color);
    DrawList->AddLine(ImVec2(LeftShoulder.X, LeftShoulder.Y), ImVec2(Neck.X, Neck.Y), Color);
    DrawList->AddLine(ImVec2(RightWrist.X, RightWrist.Y), ImVec2(RightElbow.X, RightElbow.Y), Color);
    DrawList->AddLine(ImVec2(RightElbow.X, RightElbow.Y), ImVec2(RightShoulder.X, RightShoulder.Y), Color);
    DrawList->AddLine(ImVec2(RightShoulder.X, RightShoulder.Y), ImVec2(Neck.X, Neck.Y), Color);
    return true;
}

struct COMDeleter {
    template <typename T>
    void operator()(T* ptr) const {
        if (ptr) ptr->Release();
    }
};

template <typename T>
using COMPtr = std::unique_ptr<T, COMDeleter>;

void Renderer::setupImGui() {
    IMGUI_CHECKVERSION();
    auto Window = FindWindowA("Windows.UI.Core.CoreWindow", "ARK: Survival Evolved");
    if (!Window) {
        HWND ParentWindow = FindWindowA("ApplicationFrameWindow", "ARK: Survival Evolved");
        HWND ChildWindow = FindWindowExA(ParentWindow, NULL, "Windows.UI.Core.CoreWindow", "ARK: Survival Evolved");
        D3D.GameWindow = ChildWindow;
    }
    else {
        D3D.GameWindow = Window;
    }
    ImGui::CreateContext();
    ImGuiIO& IO = ImGui::GetIO();
    ImFontConfig Config;
    ImGuiStyle* Style = &ImGui::GetStyle();
    bool show_demo_window = true, loader_window = false;
    bool show_another_window = false;
    ImGui::StyleColorsDark();
    setupImGuiStyle(Style, IO, Config);
}

void Renderer::setupImGuiStyle(ImGuiStyle* Style, ImGuiIO& IO, ImFontConfig& Config) {
    Style->Alpha = 1.f;
    Style->WindowRounding = 12.f;
    Style->FramePadding = ImVec2(4, 3);
    Style->WindowPadding = ImVec2(8, 8);
    Style->ItemInnerSpacing = ImVec2(4, 4);
    Style->ItemSpacing = ImVec2(8, 5);
    Style->FrameRounding = 4.f;
    Style->ScrollbarSize = 2.f;
    Style->ScrollbarRounding = 12.f;
    Style->PopupRounding = 5.f;
    ImVec4* colors = ImGui::GetStyle().Colors;
    setColorSettings(colors);
    Config.OversampleH = 1;
    Config.OversampleV = 1;
    Config.PixelSnapH = 1;
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF,
        0x0400, 0x044F,
        0,
    };
    zzzz = IO.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 13.0f, &Config);
    icons = IO.Fonts->AddFontFromMemoryTTF((void*)iconfont, sizeof(iconfont), 50.f, &Config);
    Config.GlyphRanges = IO.Fonts->GetGlyphRangesCyrillic();
    Config.RasterizerMultiply = 1.125f;
    IO.IniFilename = nullptr;
}

void Renderer::setColorSettings(ImVec4* colors) {
    colors[ImGuiCol_ChildBg] = ImColor(24, 29, 59, 0);
    colors[ImGuiCol_Border] = ImVec4(255, 255, 255, 0);
    colors[ImGuiCol_FrameBg] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_Header] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_HeaderActive] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_HeaderHovered] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_PopupBg] = ImColor(162, 112, 191, 255);
    colors[ImGuiCol_Button] = ImColor(162, 112, 191, 255);
    colors[ImGuiCol_ButtonHovered] = ImColor(155, 90, 191, 255);
    colors[ImGuiCol_ButtonActive] = ImColor(152, 75, 191, 255);
    colors[ImGuiCol_TitleBgActive] = ImColor(24, 29, 59, 0);
    colors[ImGuiCol_WindowBg] = ImColor(24, 29, 59, 0);
    colors[ImGuiCol_Border] = ImColor(24, 29, 59, 0);
}

void Renderer::renderFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("##ESP", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    auto& io = ImGui::GetIO();
    Cache.WindowSizeX = io.DisplaySize.x;
    Cache.WindowSizeY = io.DisplaySize.y;
    ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    if (Settings.IsMenuOpen)
    {
        Renderer::HookInput();

        ImGui::Begin("[Ching Chong Hook]", &Settings.IsMenuOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowSize(ImVec2(Settings.MenuSizeX, Settings.MenuSizeY));

        decorations();
        tabss();

        ImGui::End();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::End();
}

HRESULT Renderer::D3D_HOOK(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags) {
    COMPtr<ID3D11Texture2D> Surface;

    if (!D3D.Device) {
        if (FAILED(SwapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&Surface)))) {
            Logger::Log("Failed to get buffer from SwapChain");
            return D3D.PresentFunc(SwapChain, SyncInterval, Flags);
        }

        Logger::Log("[ID3D11Texture2D]: %p\n", Surface.get());

        if (FAILED(SwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<PVOID*>(&D3D.Device)))) {
            Logger::Log("Failed to get device from SwapChain");
            return D3D.PresentFunc(SwapChain, SyncInterval, Flags);
        }

        Logger::Log("[ID3D11Device]: %p\n", D3D.Device);

        if (FAILED(D3D.Device->CreateRenderTargetView(Surface.get(), nullptr, &D3D.RenderTargetView))) {
            Logger::Log("Failed to create render target view");
            return D3D.PresentFunc(SwapChain, SyncInterval, Flags);
        }

        Logger::Log("[ID3D11RenderTargetView]: %p\n", D3D.RenderTargetView);
        Surface.reset();
        D3D.Device->GetImmediateContext(&D3D.Ctx);
        Logger::Log("[ID3D11DeviceContext]: %p\n", D3D.Ctx);

        setupImGui();

        if (!ImGui_ImplDX11_Init(D3D.Device, D3D.Ctx)) {
            Logger::Log("Failed to initialize ImGui_DX11");
            return D3D.PresentFunc(SwapChain, SyncInterval, Flags);
        }

        if (!ImGui_ImplDX11_CreateDeviceObjects()) {
            Logger::Log("Failed to create ImGui DX11 device objects");
            return D3D.PresentFunc(SwapChain, SyncInterval, Flags);
        }

        Logger::Log("[IMGUI]: Initialized Successfully!\n");
    }

    renderFrame();

    D3D.Ctx->OMSetRenderTargets(1, &D3D.RenderTargetView, nullptr);
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return D3D.OriginalPresent(SwapChain, SyncInterval, Flags);
}

//HRESULT Renderer::D3D_HOOK(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags)
//{
//    if (!D3D.Device)
//    {
//        ID3D11Texture2D * Surface = nullptr;
//        goto Init;
//    Cleanup:
//        Logger::Log("[IMGUI]: Initializing ImGui Cleanup!\n");
//        if (Surface) Surface->Release();
//        return D3D.PresentFunc(SwapChain, SyncInterval, Flags);
//    Init:
//        if (FAILED(SwapChain->GetBuffer(0, _uuidof(Surface), reinterpret_cast<PVOID*>(&Surface)))) { goto Cleanup; }
//        Logger::Log("[ID3D11Texture2D]: %p\n", Surface);
//        if (FAILED(SwapChain->GetDevice(__uuidof(D3D.Device), reinterpret_cast<PVOID*>(&D3D.Device)))) goto Cleanup;
//        Logger::Log("[ID3D11Device]: %p\n", D3D.Device);
//        if (FAILED(D3D.Device->CreateRenderTargetView(Surface, nullptr, &D3D.RenderTargetView))) goto Cleanup;
//        Logger::Log("[ID3D11RenderTargetView]: %p\n", D3D.RenderTargetView);
//        Surface->Release();
//        Surface = nullptr;
//        D3D.Device->GetImmediateContext(&D3D.Ctx);
//        Logger::Log("[ID3D11DeviceContext]: %p\n", D3D.Ctx);
//        IMGUI_CHECKVERSION();
//        auto Window = FindWindowA("Windows.UI.Core.CoreWindow", "ARK: Survival Evolved");
//        if (!Window)
//        {
//            HWND ParentWindow = FindWindowA("ApplicationFrameWindow", "ARK: Survival Evolved");
//            HWND ChildWindow = FindWindowExA(ParentWindow, NULL, "Windows.UI.Core.CoreWindow", "ARK: Survival Evolved");
//            D3D.GameWindow = ChildWindow;
//        }
//        else { D3D.GameWindow = Window; }
//        ImGui::CreateContext();
//        {
//            ImGuiIO& IO = ImGui::GetIO();
//            ImFontConfig Config;
//            ImGuiStyle* Style = &ImGui::GetStyle();
//            bool show_demo_window = true, loader_window = false;
//            bool show_another_window = false;
//            ImGui::StyleColorsDark();
//            Style->Alpha = 1.f;
//            Style->WindowRounding = 12.f;
//            Style->FramePadding = ImVec2(4, 3);
//            Style->WindowPadding = ImVec2(8, 8);
//            Style->ItemInnerSpacing = ImVec2(4, 4);
//            Style->ItemSpacing = ImVec2(8, 5);
//            Style->FrameRounding = 4.f;
//            Style->ScrollbarSize = 2.f;
//            Style->ScrollbarRounding = 12.f;
//            Style->PopupRounding = 5.f;
//            ImVec4* colors = ImGui::GetStyle().Colors;
//
//            colors[ImGuiCol_ChildBg] = ImColor(24, 29, 59, 0);
//            colors[ImGuiCol_Border] = ImVec4(255, 255, 255, 0);
//            colors[ImGuiCol_FrameBg] = ImColor(25, 25, 33, 255);
//            colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 33, 255);
//            colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 33, 255);
//            colors[ImGuiCol_Header] = ImColor(25, 25, 33, 255);
//            colors[ImGuiCol_HeaderActive] = ImColor(25, 25, 33, 255);
//            colors[ImGuiCol_HeaderHovered] = ImColor(25, 25, 33, 255);
//            colors[ImGuiCol_PopupBg] = ImColor(162, 112, 191, 255);
//            colors[ImGuiCol_Button] = ImColor(162, 112, 191, 255);
//            colors[ImGuiCol_ButtonHovered] = ImColor(155, 90, 191, 255);
//            colors[ImGuiCol_ButtonActive] = ImColor(152, 75, 191, 255);
//            colors[ImGuiCol_TitleBgActive] = ImColor(24, 29, 59, 0);
//            colors[ImGuiCol_WindowBg] = ImColor(24, 29, 59, 0);
//            colors[ImGuiCol_Border] = ImColor(24, 29, 59, 0);
//
//            Config.OversampleH = 1; 
//            Config.OversampleV = 1;
//            Config.PixelSnapH = 1;
//
//            static const ImWchar ranges[] =
//            {
//                0x0020, 0x00FF, 
//                0x0400, 0x044F, 
//                0,
//            };
//            zzzz = IO.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 13.0f, &Config);
//            icons = IO.Fonts->AddFontFromMemoryTTF((void*)iconfont, sizeof(iconfont), 50.f, &Config);
//            Config.GlyphRanges = IO.Fonts->GetGlyphRangesCyrillic();
//            Config.RasterizerMultiply = 1.125f;
//            IO.IniFilename = nullptr;
//        }
//        Logger::Log("[GameWindow]: %p\n", D3D.GameWindow);
//        ImGui_ImplWin32_Init(D3D.GameWindow);
//        if (!ImGui_ImplDX11_Init(D3D.Device, D3D.Ctx)) goto Cleanup;
//        if (!ImGui_ImplDX11_CreateDeviceObjects()) goto Cleanup;
//        Logger::Log("[IMGUI]: Initialized Successfully!\n");
//    }
//    ImGui_ImplDX11_NewFrame();
//    ImGui_ImplWin32_NewFrame();
//    ImGui::NewFrame();
//    ImGui::Begin("##ESP", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
//    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
//    auto& io = ImGui::GetIO();
//    Cache.WindowSizeX = io.DisplaySize.x;
//    Cache.WindowSizeY = io.DisplaySize.y;
//    ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
//    auto DrawList = ImGui::GetCurrentWindow()->DrawList;
//
//    std::array<std::string, (151)> RealDinoNames = { "Achatina", "Allo", "Ammonite", "Angler", "Anky", "Ant", "Archa", "Argent", "Arthro", "Astrodelphis", "Baryonyx", "Basilisk", "Basilosaurus", "Bat", "Beaver", "Beetle", "Bigfoot", "Bloodstalker", "BrainSlug", "Bronto", "Camelsaurus", "Carno", "CaveCrab", "CaveWolf", "Chalico", "Cnidaria", "Compy", "Daeodon", "Deathworm", "Default", "Deinonychus", "DesertTitan", "Dilo", "Dimetro", "Dimorph", "Diplo", "Diplocaulus", "Direbear", "Direwolf", "Dodo", "Doed", "Dolphin", "Dragon", "Dragonfly", "Dunkle", "Eel", "Elite Carno", "Elite Raptor", "Elite Rex", "Elite Mega", "Enforcer", "Equus", "Euryp", "Flock", "ForestTitan", "Gacha", "Galli", "GasBags", "GiantTurtle", "Gigant", "Gremlin", "Griffin", "Hesperornis", "Hyaenodon", "IceTitan", "Ichthyornis", "Iguanodon", "Jerboa", "Jugbug", "Kairu", "Kangaroo", "Kaprosuchus", "Kentro", "Lamprey", "Lantern Bird", "Lantern Goat", "Lantern Lizard", "LavaLizard", "Leech", "Leedsichthys", "Lionfish Lion", "Liopleurodon", "Lystro", "Maewing", "Mammoth", "Managarmr", "Manta", "Mantis", "Megalania", "Megalosaurus", "Megapithecus", "Megatherium", "Microraptor", "MoleRat", "Monkey", "Mosasaur", "Mega", "Moschops", "Moth", "Otter", "Ovi", "Owl", "Pachy", "Para", "Paracer", "Pegomastax", "Pela", "Phiomia", "Phoenix", "Piranha", "Plesiosaur", "Ptera", "Pug", "Purlovia", "Quetz", "Raptor", "Rex", "Rhino", "RockDrake", "RockElemental", "Sabertooth", "Sarco", "Scorpion", "Scout", "Sheep", "Space Whale", "Spider", "SpineyLizard", "Spino", "Stag", "Stego", "Summoner", "Tapejara", "TekStrider", "TekWyvern", "TerrorBird", "Therizinosaurus", "Thylacoleo", "Titan", "Titanboa", "Toad", "Trike", "Troodon", "Tropeognathus", "Turtle", "Tusoteuthis", "Velonasaur", "Vulture", "Wyvern", "Xenomorph", "Yutyrannus"};
//
//    try
//    {
//        UWorld::GWorld = *reinterpret_cast<decltype(UWorld::GWorld)*>(Cache.GameBase + Settings.uworld);
//        do
//        {
//            UWorld* GWorld = UWorld::GWorld;
//            if (!GWorld) break;
//            void* GameState = GWorld->GameState;
//            if (!GameState) break;
//            UPlayer* LocalPlayer = GWorld->OwningGameInstance->LocalPlayers[0];
//            if (!LocalPlayer) break;
//            Cache.LocalPlayer = GWorld->OwningGameInstance->LocalPlayers[0];
//            Cache.LPC = Cache.LocalPlayer->PlayerController;
//            Cache.LPFOV = Cache.LPC->PlayerCameraManager->DefaultFOV;
//            Cache.LocalLocation = Cache.LPC->PlayerCameraManager->GetCameraLocation();
//            Cache.NearbyEnemies = 0;
//
//            auto Actors = UWorld::GWorld->PersistentLevel->Actors;
//            for (int i = 0; i < Actors.Count; i++)
//            {
//                auto Actor = Actors[i];
//                if (Actor)
//                {
//                    bool isDead = false;
//                    bool isSleeping = false;
//                    bool isTribeDinoOrPlayer = false;
//                    auto TargetableActor = (APrimalTargetableActor*)Actor;
//                    if (Actor->IsPlayer() && Settings.Visuals.DrawPlayers)
//                    {
//                        std::string GamerTag;
//                        std::string PlayerDisplayString;
//                        FVector2D ActorScreenLocation;
//                        float TorpOffset = 13.f;
//                        float DistanceOffset = -13.f;
//
//                        ImColor PlayerColor;
//                        int NewR = (int)PlayerColor1[0] * 255;
//                        int NewG = (int)PlayerColor1[1] * 255;
//                        int NewB = (int)PlayerColor1[2] * 255;
//                        PlayerColor = ImColor(NewR, NewG, NewB, 255);
//
//                        float HealthPercent = Actor->ReplicatedCurrentHealth / Actor->ReplicatedMaxHealth;
//                        std::string PlayerHP = std::string("[" + std::to_string((int)Actor->ReplicatedCurrentHealth) + "/" + std::to_string((int)Actor->ReplicatedMaxHealth) + " HP]");
//                        std::string PlayerTorp = std::string("[" + std::to_string((int)Actor->ReplicatedCurrentTorpor) + "/" + std::to_string((int)Actor->ReplicatedMaxTorpor) + " Torp]");
//
//                        if (Actor == Cache.LocalActor && Settings.Visuals.HideSelf) continue;
//
//                        if (Settings.Visuals.DrawPlayerDistance && Settings.Visuals.PlayerTorp) 
//                        { 
//                            TorpOffset = TorpOffset * 2;
//                        }
//                        else if (Settings.Visuals.DrawSleepingPlayers && Settings.Visuals.DrawPlayerHP)
//                        {
//                            DistanceOffset = DistanceOffset * 2;
//                        }
//                        else if (Settings.Visuals.DrawPlayerHP && Settings.Visuals.DrawPlayerDistance)
//                        {
//                            DistanceOffset = DistanceOffset * 2;
//                        }
//
//                        if (Actor->IsLocalPlayer()) 
//                        {
//                            Cache.LocalActor = Actor; 
//                        }
//
//                        if (Actor->PlatformProfileName.Data) 
//                        { 
//                            GamerTag = Actor->PlatformProfileName.ToString(); 
//                        }
//
//                        if (Actor->IsDead())
//                        {
//                            isDead = true;
//                            if (Settings.Visuals.DrawDeadPlayers)
//                            {
//                                PlayerColor = ImColor(235, 119, 0, 255);
//                            }
//                        }
//
//                        if (!Actor->IsConscious() && !isDead)
//                        {
//                            isSleeping = true;
//                            if (Settings.Visuals.DrawSleepingPlayers)
//                            {
//                                PlayerColor = ImColor(255, 237, 237, 255);
//                            }
//                        }
//
//                        if (Actor->IsPrimalCharFriendly((APrimalCharacter*)Cache.LocalActor))
//                        {
//                            isTribeDinoOrPlayer = true;
//                            if (!isDead && !isSleeping)
//                            {
//                                PlayerColor = ImColor(100, 119, 0, 255);
//                            }
//                        }
//
//                        if (isTribeDinoOrPlayer && isSleeping && !isDead)
//                        {
//                            PlayerColor = ImColor(154, 161, 119, 255);
//                        }
//
//                        if (Actor->PlayerName.Data && Actor->MyCharacterStatusComponent) 
//                        { 
//                            PlayerDisplayString = "(" + GamerTag + ") " + Actor->PlayerName.ToString() + " [lvl: " +
//                                std::to_string(Actor->MyCharacterStatusComponent->BaseCharacterLevel + Actor->
//                                    MyCharacterStatusComponent->ExtraCharacterLevel) + "]";
//                        }
//                        if (Actor->IsPlayer() && Actor->IsConscious() && !Actor->IsDead() && Actor != Cache.LocalActor)
//                        {
//                            if (!isTribeDinoOrPlayer) {
//                                std::string NewNumbPlayers;
//                                Cache.NearbyEnemies += 1;
//                                NewNumbPlayers = Cache.NearbyEnemies;
//                            }
//                        }
//
//                        if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                        {
//                            if (!Settings.Visuals.DrawDeadPlayers && isDead) continue;
//                            if (!Settings.Visuals.DrawSleepingPlayers && isSleeping) continue;
//                            if (Settings.Visuals.HideTeamPlayers && isTribeDinoOrPlayer) continue;
//                            if (Settings.Visuals.DrawDeadPlayers && isDead)
//                            {
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y - 13), PlayerColor, "Dead", 500);
//                            }
//                            if (Settings.Visuals.DrawSleepingPlayers && isSleeping)
//                            {
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y - DistanceOffset + 13), PlayerColor, "Sleeping", 500);
//                            }
//                            if (Settings.Visuals.DrawPlayerDistance || Settings.Visuals.ShowWeapons)
//                            {
//                                int Distance = Cache.LocalActor->RootComponent->GetWorldLocation().DistTo(Actor->RootComponent->GetWorldLocation()) * 0.01f;
//                                std::string DistanceString = "[" + std::to_string(Distance) + "m]";
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 13), PlayerColor, DistanceString.c_str(), 500);
//                            }
//                            if (!isDead && isSleeping && Settings.Visuals.PlayerTorp)
//                            {
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + TorpOffset), PlayerColor, PlayerTorp.c_str(), 500);
//                            }
//                            if (Settings.Visuals.RenderPlayerName)
//                            {
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), PlayerColor, PlayerDisplayString.c_str(), 500);
//                            }
//                            if (Settings.Visuals.DrawPlayerBones)
//                            {
//                                Renderer::Drawing::RenderPlayerSkeleton(Actor->MeshComponent, Actor->RetrievePlayerGender(Actor->Name.GetName()), PlayerColor);
//                            }
//                            if (isDead) continue;
//                            if (!Settings.Visuals.DrawPlayerHP) continue;
//                            Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y - DistanceOffset), PlayerColor, PlayerHP.c_str(), 500);
//                        }
//                    }
//                    if (Actor->IsDino() && Settings.Visuals.DrawWildCreatures || Actor->IsDino() && Settings.Visuals.DrawTamedCreatures) {
//                        bool IsTamed = Actor->IsTamed();
//
//                        if (Settings.Visuals.HideFish)
//                        {
//                            if (Actor->IsFish(Actor->DinoNameTag.GetName())) continue;
//                        }
//                        if (Settings.Visuals.isBee)
//                        {
//                            if (Actor->IsDino() && Actor->DinoNameTag.GetName() == "Bee") continue;
//                        }
//                        if (Settings.Visuals.hideManta)
//                        {
//                            if (Actor->IsManta(Actor->DinoNameTag.GetName())) continue;
//                        }
//
//                        if (Settings.Misc.NoglinAlert && Actor->IsTamed() && Actor->DinoNameTag.GetName() == "Gremlin")
//                        {
//                            NearbyNoglin = true;
//                        }
//                        else
//                        {
//                            NearbyNoglin = false;
//                        }
//
//                        if (IsTamed && Settings.Visuals.DrawTamedCreatures)
//                        {
//                            FVector2D ActorScreenLocation;
//                            auto DinoChar = (APrimalDinoCharacter*)Actor;
//                            auto PlayerData = (FPrimalPlayerDataStruct*)Actor;
//                            auto PlayerD = Cache.LocalActor->GetPlayerData();
//                            float TorpOffset = 33.f;
//
//                            ImColor DinoColor;
//                            int NewR = TamedDinoColor1[0] * 255;
//                            int NewG = TamedDinoColor1[1] * 255;
//                            int NewB = TamedDinoColor1[2] * 255;
//                            DinoColor = ImColor(NewR, NewG, NewB, 255);
//
//                            std::string DinoDatazz = Actor->DinoNameTag.GetName() + " [lvl: " + std::to_string(Actor->MyCharacterStatusComponent->BaseCharacterLevel + Actor->MyCharacterStatusComponent->ExtraCharacterLevel) + "]";
//                            std::string DinoHP = std::string("[" + std::to_string((int)Actor->ReplicatedCurrentHealth) + "/" + std::to_string((int)Actor->ReplicatedMaxHealth) + " HP]");;
//                            std::string DinoTorp1;
//
//                            if (Settings.Visuals.TamedDinoTorp)
//                            {
//                                DinoTorp1 = std::string("[" + std::to_string((int)Actor->ReplicatedCurrentTorpor) + "/" + std::to_string((int)Actor->ReplicatedMaxTorpor) + " Torp]");
//                            }
//
//                            bool TeamDino = false;
//
//                            if (Actor->IsPrimalCharFriendly((APrimalCharacter*)Cache.LocalActor))
//                            {
//                                isTribeDinoOrPlayer = true;
//                            }
//
//                            if (isTribeDinoOrPlayer && !isDead && !isSleeping)
//                            {
//                                DinoColor = ImColor(2, 125, 0, 255);
//                            }
//
//                            if (Actor->IsDead())
//                            {
//                                isDead = true;
//                                if (Settings.Visuals.DinoDead)
//                                {
//                                    DinoColor = ImColor(235, 119, 0, 255);
//                                }
//                            }
//
//                            if (!Actor->IsConscious() && !isDead)
//                            {
//                                isSleeping = true;
//                                if (Settings.Visuals.DrawSleepingDinos)
//                                {
//                                    DinoColor = ImColor(255, 237, 237, 255);
//                                }
//                            }
//
//                            if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                            {
//                                if (!Settings.Visuals.DinoDead && isDead) continue;
//                                if (Settings.Visuals.HideTeamDinos && isTribeDinoOrPlayer) continue;
//                                if (isDead && Settings.Visuals.DinoDead)
//                                {
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 15), DinoColor, "Dead", 500);
//                                }
//                                if (isSleeping && Settings.Visuals.DrawSleepingDinos && !isDead)
//                                {
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 23), DinoColor, "Sleeping", 500);
//                                }
//                                if (Settings.Visuals.DrawDinoDistance && !isSleeping)
//                                {
//                                    int Distance = Cache.LocalActor->RootComponent->GetWorldLocation().DistTo(Actor->RootComponent->GetWorldLocation()) * 0.01f;
//                                    std::string DistanceString = "[" + std::to_string(Distance) + "m]";
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 23), DinoColor, DistanceString.c_str(), 500);
//                                }
//                                if (Settings.Visuals.TamedDinoTorp && !isSleeping && !isDead)
//                                {
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + TorpOffset), DinoColor, DinoTorp1.c_str(), 500);
//                                }
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), DinoColor, DinoDatazz.c_str(), 500);
//                                if (isDead) continue;
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 15), DinoColor, DinoHP.c_str(), 500);
//                                
//                            }
//                        } 
//                        if (!IsTamed && Settings.Visuals.DrawWildCreatures)
//                        {
//                            FVector2D ActorScreenLocation;
//                            ImColor DinoColorWild;
//                            ImColor FilteredDino;
//                            std::string DinoTorp1;
//
//                            FilteredDino = ImColor(184, 0, 131, 255);
//
//                            int NewR = WildDinoColor1[0] * 255;
//                            int NewG = WildDinoColor1[1] * 255;
//                            int NewB = WildDinoColor1[2] * 255;
//                            DinoColorWild = ImColor(NewR, NewG, NewB, 255);
//
//                            int NewR1 = FilteredDinoColor1[0] * 255;
//                            int NewG1 = FilteredDinoColor1[1] * 255;
//                            int NewB1 = FilteredDinoColor1[2] * 255;
//                            FilteredDino = ImColor(NewR1, NewG1, NewB1, 255);
//
//                            std::string DinoDatazz = Actor->DinoNameTag.GetName() + " [lvl: " + std::to_string(Actor->MyCharacterStatusComponent->BaseCharacterLevel + Actor->MyCharacterStatusComponent->ExtraCharacterLevel) + "]";
//
//                            std::string DinoHP = std::string("[" + std::to_string((int)Actor->ReplicatedCurrentHealth) + "/" + std::to_string((int)Actor->ReplicatedMaxHealth) + " HP]");
//                                
//                            if (Settings.Visuals.WildDinoTorp && Settings.Visuals.DrawWildCreatures)
//                            {
//                                DinoTorp1 = std::string("[" + std::to_string((int)Actor->ReplicatedCurrentTorpor) + "/" + std::to_string((int)Actor->ReplicatedMaxTorpor) + " Torp]");
//                            }
//
//                            if (Settings.Visuals.WildDinoFilter)
//                            {
//                                CompareName = RealDinoNames[PossibleDinos];
//                                if (Actor->DinoNameTag.GetName() == CompareName)
//                                {
//                                    if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                                    {
//                                        Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y - 13), FilteredDino, DinoHP.c_str(), 500);
//                                        Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), FilteredDino, DinoDatazz.c_str(), 500);
//                                        Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 13), FilteredDino, DinoTorp1.c_str(), 500);
//                                    }
//                                }
//                            }
//                            else if (Settings.Visuals.AlphaFilter && Actor->IsAlpha(Actor->DinoNameTag.GetName()))
//                            {
//                                if (Actor->DinoNameTag.GetName() == "Elite Raptor" || Actor->DinoNameTag.GetName() == "Elite Mega" || Actor->DinoNameTag.GetName() == "Elite Rex" || Actor->DinoNameTag.GetName() == "Elite Carno")
//                                {
//                                    if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                                    {
//                                        Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y - 13), FilteredDino, DinoHP.c_str(), 500);
//                                        Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), FilteredDino, DinoDatazz.c_str(), 500);
//                                        Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 13), FilteredDino, DinoTorp1.c_str(), 500);
//                                    }
//                                }
//                            }
//                            else
//                            {
//                                if (Settings.Visuals.AlphaFilter) continue;
//                                if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                                {
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y - 13), DinoColorWild, DinoHP.c_str(), 500);
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), DinoColorWild, DinoDatazz.c_str(), 500);
//                                    Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 13), DinoColorWild, DinoTorp1.c_str(), 500);
//                                }
//                            }
//                        }
//                       
//                         
//                    }
//                    std::string TurretShortName;
//                    if (Actor->IsTurret(Actor->Name.GetName(), TurretShortName) && Settings.Visuals.DrawTurrets)
//                    {
//                        ImColor TurretColor;
//                        FVector2D ActorScreenLocation;
//
//                        int NewR = TurretColor1[0] * 255;
//                        int NewG = TurretColor1[1] * 255;
//                        int NewB = TurretColor1[2] * 255;
//                        TurretColor = ImColor(NewR, NewG, NewB, 255);
//
//                        if (Settings.Visuals.HideTeamTurrets && isTribeDinoOrPlayer) continue;
//                        if (!Settings.Visuals.ShowEmptyTurrets && Actor->NumBullets == 0) continue;
//                        if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                        {
//                            Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), TurretColor, TurretShortName.c_str(), 500);
//                            if (Settings.Visuals.ShowBulletCount && Actor->NumBullets != 0)
//                            {
//                                std::string BulletCount = " [" + std::to_string(Actor->NumBullets) + "]";
//                                Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y + 12), TurretColor, BulletCount.c_str(), 500);
//                            }
//                        }
//                    }
//                    if (Actor->IsItemContainer() && Settings.Visuals.DrawContainers && Actor->DescriptiveName.Data)
//                    {
//                        FVector2D ActorScreenLocation;
//
//                        ImColor ContainerColor;
//                        ImColor FilteredContainer;
//
//                        int NewR = ContainerColor1[0] * 255;
//                        int NewG = ContainerColor1[1] * 255;
//                        int NewB = ContainerColor1[2] * 255;
//                        ContainerColor = ImColor(NewR, NewG, NewB, 255);
//
//                        FilteredContainer = ImColor(70, 30, 40, 255);
//
//                        if (Settings.Visuals.HideTeamContainers && isTribeDinoOrPlayer) continue;
//                        if (Actor->IsExcludedContainer(Actor->DescriptiveName.ToString())) continue;
//                        if (!Settings.Visuals.ShowEmptyContainers && Actor->CurrentItemCount == 0) continue;
//
//                        std::string ContainerString = Actor->DescriptiveName.ToString();
//
//                        if (Settings.Visuals.ContainerFilter)
//                        {
//                            if (ContainerString != CurrentContainer) continue;
//                        }
//
//                        if (W2S(Actor->RootComponent->GetWorldLocation(), ActorScreenLocation))
//                        {
//                            if (Actor->CurrentItemCount != 0) { ContainerString += " [" + std::to_string(Actor->CurrentItemCount) + "/" + std::to_string(Actor->MaxItemCount) + "]"; }
//                            Renderer::Drawing::RenderText(ImVec2(ActorScreenLocation.X, ActorScreenLocation.Y), ContainerColor, ContainerString.c_str(), 500);
//                        }
//                    }
//                }
//            }
//
//            if (Settings.Aimbot.EnableAimbot)
//            {
//                AimbotThread();
//            }
//
//            if (Settings.Misc.ExtraInfo)
//            {
//                std::vector<std::string> DisplayStrings{};
//
//                if (true)
//                {
//                    DisplayStrings.push_back("[*] Ching Chong Hook");
//                    std::string AimbBindZz = Settings.Keybinds.AimbotKey;
//                    DisplayStrings.push_back("[>] Aim Key: " + AimbBindZz);
//                }
//
//                if (Settings.Misc.ShowPlayers)
//                {
//                    auto Players = GWorld->GameState->NumPlayerConnected;
//                    DisplayStrings.push_back("[>] Connected Players: " + std::to_string((int)Players));
//                }
//
//                if (Settings.Misc.ShowFPS)
//                {
//                    auto IO = ImGui::GetIO();
//                    DisplayStrings.push_back("[>] FPS: " + std::to_string((int)IO.Framerate));
//                }
//
//                if (Settings.Misc.ShowXYZ && Cache.LocalActor)
//                {
//                    auto XYZ = Cache.LocalActor->RootComponent->GetWorldLocation();
//                    DisplayStrings.push_back("[X]: " + std::to_string(XYZ.X) + " | [Y]: " + std::to_string(XYZ.Y) + " | [Z]: " + std::to_string(XYZ.Z));
//                }
//
//                if (NearbyNoglin && Settings.Misc.NoglinAlert)
//                {
//                    DisplayStrings.push_back("[!] Nearby Noglin");
//                }
//
//                if (Settings.Visuals.ContainerFilter && Settings.Visuals.DrawContainers)
//                {
//                    DisplayStrings.push_back("[X] Container Filter: " + std::string(CurrentContainer));
//                }
//
//                if (Settings.Visuals.WildDinoFilter && Settings.Visuals.DrawWildCreatures)
//                {
//                    DisplayStrings.push_back("[X] Wild Dino Filder: " + std::string(CompareName));
//                }
//
//                Renderer::Drawing::RenderCollapseFriendlyDisplayList(ImVec2(10, 10), ImColor(255, 255, 255, 255), DisplayStrings, 12.f);
//            }
//
//            if (Settings.Misc.NoSway && Cache.LocalActor)
//            {
//                if (Cache.LocalActor->CurrentWeapon)
//                {
//                    if (Cache.LocalActor->CurrentWeapon->AimDriftPitchFrequency) { Cache.LocalActor->CurrentWeapon->AimDriftPitchFrequency = 0; }
//                    if (Cache.LocalActor->CurrentWeapon->AimDriftYawFrequency) { Cache.LocalActor->CurrentWeapon->AimDriftYawFrequency = 0; }
//                }
//            }
//
//            if (Settings.Misc.NoSpread && Cache.LocalActor)
//            {
//                if (Cache.LocalActor->CurrentWeapon)
//                {
//                    Cache.LocalActor->CurrentWeapon->InstantConfig.WeaponSpread = 0;
//                    Cache.LocalActor->CurrentWeapon->InstantConfig.TargetingSpreadMod = 0;
//                }
//            }
//
//            if (Settings.Misc.NoShake && Cache.LocalActor)
//            {
//                if (Cache.LocalActor->CurrentWeapon)
//                {
//                    Cache.LocalActor->CurrentWeapon->bUseFireCameraShakeScale = 0;
//                    Cache.LocalActor->CurrentWeapon->GlobalFireCameraShakeScale = 0;
//                    Cache.LocalActor->CurrentWeapon->ReloadCameraShakeSpeedScale = 0;
//                    Cache.LocalActor->CurrentWeapon->GlobalFireCameraShakeScaleTargeting = 0;
//                }
//            }
//
//            if (Settings.Misc.SpeedHacks)
//            {
//                auto CharMove = Cache.LocalActor->CharacterMovement;
//                auto DinoChar = (APrimalDinoCharacter*)Cache.LocalActor;
//                auto PrimalChar = (APrimalCharacter*)Cache.LocalActor;
//                auto RiddenDino = Cache.LocalActor->GetBasedOrSeatingOnDino();
//
//                PrimalChar->ExtraMaxSpeedModifier = Settings.Misc.NewSpeed;
//                PrimalChar->RunningSpeedModifier = Settings.Misc.NewSpeed;
//                CharMove->Acceleration = Settings.Misc.NewSpeed;
//                CharMove->MaxFlySpeed = Settings.Misc.NewSpeed;
//            }
//
//            if (Settings.Misc.RapidFire)
//            {
//                auto Zoom = (AShooterWeapon_Rapid*)Cache.LocalActor->CurrentWeapon;
//                Zoom->WeaponConfig.TimeBetweenShots = 0.01;
//                if (Zoom->LastFireTime == 0) Zoom->LastFireTime = Zoom->LastFireTime + Zoom->LastNotifyShotTime - Zoom->WeaponConfig.TimeBetweenShots;
//            }
//
//            if (Settings.Misc.InfiniteOrbit && Cache.LocalActor)
//            {
//                Cache.LocalActor->OrbitCamMaxZoomLevel = 5000;
//            }
//
//            if (Settings.Misc.LongArms)
//            {
//                auto PrimalChar = (APrimalCharacter*)Cache.LocalActor;
//                if (Settings.Misc.InfiniteArms)
//                {
//                    PrimalChar->AdditionalMaxUseDistance = 2500000000000000000;
//                }
//                else
//                {
//                    PrimalChar->AdditionalMaxUseDistance = 250;
//                }
//            }
//
//            if (Settings.Visuals.DrawCrosshair) {
//                ImColor CrosshairColor;
//
//                int NewR = CrosshairColor1[0] * 255;
//                int NewG = CrosshairColor1[1] * 255;
//                int NewB = CrosshairColor1[2] * 255;
//                CrosshairColor = ImColor(NewR, NewG, NewB, 255);
//                if (CrosshairColor && Settings.Visuals.DrawCrosshair)
//                {
//                    Renderer::Drawing::RenderCrosshair(CrosshairColor, Settings.Visuals.CrosshairWidth);
//                }
//            }
//
//            if (Settings.Visuals.DrawAimFOV) {
//
//                ImColor CrosshairFOVcolor;
//
//                int NewR = CrosshairColor1[0] * 255;
//                int NewG = CrosshairColor1[1] * 255;
//                int NewB = CrosshairColor1[2] * 255;
//                CrosshairFOVcolor = ImColor(NewR, NewG, NewB, 255);
//
//                Renderer::Drawing::RenderAimFOV(CrosshairFOVcolor);
//            }
//
//            if (Cache.NearbyEnemies > 0) {
//                if (Cache.NearbyEnemies)
//                {
//                    std::string NearbyPlayers;
//                    NearbyPlayers = " [Nearby Enemies: " + std::to_string(Cache.NearbyEnemies) + "]";
//                    Renderer::Drawing::RenderText2(ImVec2(925, 65), ImColor(245, 158, 66, 255), NearbyPlayers.c_str(), 10000);
//                }
//            }
//
//            if (Settings.Misc.AutoArmor)
//            {
//                auto Inventory = Cache.LocalActor->MyInventoryComponent;
//                if (Inventory)
//                {
//                    Cache.AutoArmor.FlakHelmets.clear(), Cache.AutoArmor.FlakChestplates.clear(), Cache.AutoArmor.FlakGloves.clear(), Cache.AutoArmor.FlakPants.clear(), Cache.AutoArmor.FlakBoots.clear();
//                    Cache.AutoArmor.TekHelmets.clear(), Cache.AutoArmor.TekChestplates.clear(), Cache.AutoArmor.TekGloves.clear(), Cache.AutoArmor.TekPants.clear(), Cache.AutoArmor.TekBoots.clear();
//                    Cache.AutoArmor.RiotHelmets.clear(), Cache.AutoArmor.RiotChestplates.clear(), Cache.AutoArmor.RiotGloves.clear(), Cache.AutoArmor.RiotPants.clear(), Cache.AutoArmor.RiotBoots.clear();
//                    for (int i = 0; i < Inventory->InventoryItems.Count; i++)
//                    {
//                        auto Item = Inventory->InventoryItems[i];
//                        auto ItemName = Item->DescriptiveNameBase.ToString();
//                        if (Item->IsBroken()) continue;
//                        if (!Cache.LocalActor->MyInventoryComponent->InventoryItems[i]->CanDrop()) continue;
//                        if (Item->MyEquipmentType == Item->Hat)
//                        {
//                            if (ItemName == "Flak Helmet") Cache.AutoArmor.FlakHelmets.push_back((uintptr_t)Item);
//                            if (ItemName == "Tek Helmet") Cache.AutoArmor.TekHelmets.push_back((uintptr_t)Item);
//                            if (ItemName == "Riot Helmet") Cache.AutoArmor.RiotHelmets.push_back((uintptr_t)Item);
//                        }
//                        if (Item->MyEquipmentType == Item->Shirt)
//                        {
//                            if (ItemName == "Flak Chestpiece") Cache.AutoArmor.FlakChestplates.push_back((uintptr_t)Item);
//                            if (ItemName == "Tek Chestpiece") Cache.AutoArmor.TekChestplates.push_back((uintptr_t)Item);
//                            if (ItemName == "Riot Chestpiece") Cache.AutoArmor.RiotChestplates.push_back((uintptr_t)Item);
//                        }
//                        if (Item->MyEquipmentType == Item->Gloves)
//                        {
//                            if (ItemName == "Flak Gauntlets") Cache.AutoArmor.FlakGloves.push_back((uintptr_t)Item);
//                            if (ItemName == "Tek Gauntlets") Cache.AutoArmor.TekGloves.push_back((uintptr_t)Item);
//                            if (ItemName == "Riot Gauntlets") Cache.AutoArmor.RiotGloves.push_back((uintptr_t)Item);
//                        }
//                        if (Item->MyEquipmentType == Item->Pants)
//                        {
//                            if (ItemName == "Flak Leggings") Cache.AutoArmor.FlakPants.push_back((uintptr_t)Item);
//                            if (ItemName == "Tek Leggings") Cache.AutoArmor.TekPants.push_back((uintptr_t)Item);
//                            if (ItemName == "Riot Leggings") Cache.AutoArmor.RiotPants.push_back((uintptr_t)Item);
//                        }
//                        if (Item->MyEquipmentType == Item->Boots)
//                        {
//                            if (ItemName == "Flak Boots") Cache.AutoArmor.FlakBoots.push_back((uintptr_t)Item);
//                            if (ItemName == "Tek Boots") Cache.AutoArmor.TekBoots.push_back((uintptr_t)Item);
//                            if (ItemName == "Riot Boots") Cache.AutoArmor.RiotBoots.push_back((uintptr_t)Item);
//                        }
//                    }
//                    bool ReplaceHelmet = false;
//                    bool ReplaceChest = false;
//                    bool ReplaceGloves = false;
//                    bool ReplaceLegs = false;
//                    bool ReplaceBoots = false;
//                    if (!Inventory->GetEquippedItemOfType(UPrimalItem::Hat)) { ReplaceHelmet = true; }
//                    if (!Inventory->GetEquippedItemOfType(UPrimalItem::Shirt)) { ReplaceChest = true; }
//                    if (!Inventory->GetEquippedItemOfType(UPrimalItem::Gloves)) { ReplaceGloves = true; }
//                    if (!Inventory->GetEquippedItemOfType(UPrimalItem::Pants)) { ReplaceLegs = true; }
//                    if (!Inventory->GetEquippedItemOfType(UPrimalItem::Boots)) { ReplaceBoots = true; }
//                    for (int i = 0; i < Inventory->EquippedItems.Count; i++)
//                    {
//                        auto Item = Inventory->EquippedItems[i];
//                        if (Item->GetDurabilityPercentage() < Settings.Misc.AutoArmorPercent)
//                        {
//                            if (Item->MyEquipmentType == Item->Hat) { ReplaceHelmet = true; }
//                            if (Item->MyEquipmentType == Item->Shirt) { ReplaceChest = true; }
//                            if (Item->MyEquipmentType == Item->Gloves) { ReplaceGloves = true; }
//                            if (Item->MyEquipmentType == Item->Pants) { ReplaceLegs = true; }
//                            if (Item->MyEquipmentType == Item->Boots) { ReplaceBoots = true; }
//                        }
//                    }
//                    if (ReplaceHelmet)
//                    {
//                        if (Cache.AutoArmor.TekHelmets.size() > 0)
//                        {
//                            auto Helmet = (UPrimalItem*)Cache.AutoArmor.TekHelmets[0];
//                            if (Helmet->CanUse(false)) { Cache.LPC->EquipPawnItem(Helmet->ItemId); }
//                            else
//                            {
//                                if (Cache.AutoArmor.FlakHelmets.size() != 0)
//                                {
//                                    Helmet = (UPrimalItem*)Cache.AutoArmor.FlakHelmets[0];
//                                    Cache.LPC->EquipPawnItem(Helmet->ItemId);
//                                    ReplaceHelmet = false;
//                                }
//                                else if (Cache.AutoArmor.RiotHelmets.size() != 0)
//                                {
//                                    Helmet = (UPrimalItem*)Cache.AutoArmor.FlakHelmets[0];
//                                    Cache.LPC->EquipPawnItem(Helmet->ItemId);
//                                    ReplaceHelmet = false;
//                                }
//                            }
//                        }
//                        if (Cache.AutoArmor.FlakHelmets.size() > 0 && ReplaceHelmet)
//                        {
//                            auto Helmet = (UPrimalItem*)Cache.AutoArmor.FlakHelmets[0];
//                            Cache.LPC->EquipPawnItem(Helmet->ItemId);
//                            ReplaceHelmet = false;
//                            //Logger::Log("Flak %f\n", Helmet->GetDurabilityPercentage());
//                        }
//                        else if (Cache.AutoArmor.RiotHelmets.size() > 0 && ReplaceHelmet)
//                        {
//                            auto Helmet = (UPrimalItem*)Cache.AutoArmor.RiotHelmets[0];
//                            Cache.LPC->EquipPawnItem(Helmet->ItemId);
//                            ReplaceHelmet = false;
//                            Logger::Log("Riot %f\n", Helmet->GetDurabilityPercentage());
//                        }
//                        //Cache.AutoArmor.BestHelmet = nullptr;
//                        //float LastDurability = 0.05f;
//                        //for (int i = 0; i < Cache.AutoArmor.RiotHelmets.size(); i++)
//                        //{
//                        //    auto Helmet = (UPrimalItem*)Cache.AutoArmor.RiotHelmets[i];
//                        //    Logger::Log("Riot Helmet Durability : %s\n", Helmet->DurabilityStringShort.ToString().c_str());
//                        //    if (Helmet->GetItemStatModifier(Helmet->EPrimalItemStats::MaxDurability) > LastDurability) { Cache.AutoArmor.BestHelmet = Helmet; }
//                        //}
//                        //for (int i = 0; i < Cache.AutoArmor.FlakHelmets.size(); i++)
//                        //{
//                        //    auto Helmet = (UPrimalItem*)Cache.AutoArmor.FlakHelmets[i];
//                        //    Logger::Log("Flak Helmet Durability : %s\n", Helmet->DurabilityStringShort.ToString().c_str());
//                        //    if (Helmet->GetItemStatModifier(Helmet->EPrimalItemStats::MaxDurability) > LastDurability) { Cache.AutoArmor.BestHelmet = Helmet; }
//                        //}
//                        //for (int i = 0; i < Cache.AutoArmor.TekHelmets.size(); i++)
//                        //{
//                        //    auto Helmet = (UPrimalItem*)Cache.AutoArmor.TekHelmets[i];
//                        //    Logger::Log("Tek Helmet Durability : %s\n", Helmet->DurabilityStringShort.ToString().c_str());
//                        //    if (Helmet->GetItemStatModifier(Helmet->EPrimalItemStats::MaxDurability) > LastDurability) { Cache.AutoArmor.BestHelmet = Helmet; }
//                        //}
//                    }
//                    if (ReplaceChest)
//                    {
//                        if (Cache.AutoArmor.TekChestplates.size() > 0)
//                        {
//                            auto Chest = (UPrimalItem*)Cache.AutoArmor.TekChestplates[0];
//                            if (Chest->CanUse(false)) { Cache.LPC->EquipPawnItem(Chest->ItemId); }
//                            else
//                            {
//                                if (Cache.AutoArmor.FlakChestplates.size() != 0)
//                                {
//                                    Chest = (UPrimalItem*)Cache.AutoArmor.FlakChestplates[0];
//                                    Cache.LPC->EquipPawnItem(Chest->ItemId);
//                                    ReplaceChest = false;
//                                }
//                                else if (Cache.AutoArmor.RiotChestplates.size() != 0)
//                                {
//                                    Chest = (UPrimalItem*)Cache.AutoArmor.RiotChestplates[0];
//                                    Cache.LPC->EquipPawnItem(Chest->ItemId);
//                                    ReplaceChest = false;
//                                }
//                            }
//                        }
//                        if (Cache.AutoArmor.FlakChestplates.size() > 0 && ReplaceChest)
//                        {
//                            auto Chest = (UPrimalItem*)Cache.AutoArmor.FlakChestplates[0];
//                            Cache.LPC->EquipPawnItem(Chest->ItemId);
//                            ReplaceChest = false;
//                        }
//                        else if (Cache.AutoArmor.RiotChestplates.size() > 0 && ReplaceChest)
//                        {
//                            auto Chest = (UPrimalItem*)Cache.AutoArmor.RiotChestplates[0];
//                            Cache.LPC->EquipPawnItem(Chest->ItemId);
//                            ReplaceChest = false;
//                        }
//                    }
//                    if (ReplaceGloves)
//                    {
//                        if (Cache.AutoArmor.TekGloves.size() > 0)
//                        {
//                            auto Gloves = (UPrimalItem*)Cache.AutoArmor.TekGloves[0];
//                            if (Gloves->CanUse(false)) { Cache.LPC->EquipPawnItem(Gloves->ItemId); }
//                            else
//                            {
//                                if (Cache.AutoArmor.FlakGloves.size() != 0)
//                                {
//                                    Gloves = (UPrimalItem*)Cache.AutoArmor.FlakGloves[0];
//                                    Cache.LPC->EquipPawnItem(Gloves->ItemId);
//                                    ReplaceGloves = false;
//                                }
//                                else if (Cache.AutoArmor.RiotGloves.size() != 0)
//                                {
//                                    Gloves = (UPrimalItem*)Cache.AutoArmor.RiotGloves[0];
//                                    Cache.LPC->EquipPawnItem(Gloves->ItemId);
//                                    ReplaceGloves = false;
//                                }
//                            }
//                        }
//                        if (Cache.AutoArmor.FlakGloves.size() > 0 && ReplaceGloves)
//                        {
//                            auto Gloves = (UPrimalItem*)Cache.AutoArmor.FlakGloves[0];
//                            Cache.LPC->EquipPawnItem(Gloves->ItemId);
//                            ReplaceGloves = false;
//                        }
//                        else if (Cache.AutoArmor.RiotGloves.size() > 0 && ReplaceGloves)
//                        {
//                            auto Gloves = (UPrimalItem*)Cache.AutoArmor.RiotGloves[0];
//                            Cache.LPC->EquipPawnItem(Gloves->ItemId);
//                            ReplaceGloves = false;
//                        }
//                    }
//                    if (ReplaceLegs)
//                    {
//                        if (Cache.AutoArmor.TekPants.size() > 0)
//                        {
//                            auto Pants = (UPrimalItem*)Cache.AutoArmor.TekPants[0];
//                            if (Pants->CanUse(false)) { Cache.LPC->EquipPawnItem(Pants->ItemId); }
//                            else
//                            {
//                                if (Cache.AutoArmor.FlakPants.size() != 0)
//                                {
//                                    Pants = (UPrimalItem*)Cache.AutoArmor.FlakPants[0];
//                                    Cache.LPC->EquipPawnItem(Pants->ItemId);
//                                    ReplaceLegs = false;
//                                }
//                                else if (Cache.AutoArmor.RiotPants.size() != 0)
//                                {
//                                    Pants = (UPrimalItem*)Cache.AutoArmor.RiotPants[0];
//                                    Cache.LPC->EquipPawnItem(Pants->ItemId);
//                                    ReplaceLegs = false;
//                                }
//                            }
//                        }
//                        if (Cache.AutoArmor.FlakPants.size() > 0 && ReplaceLegs)
//                        {
//                            auto Pants = (UPrimalItem*)Cache.AutoArmor.FlakPants[0];
//                            Cache.LPC->EquipPawnItem(Pants->ItemId);
//                            ReplaceLegs = false;
//                        }
//                        else if (Cache.AutoArmor.RiotPants.size() > 0 && ReplaceLegs)
//                        {
//                            auto Pants = (UPrimalItem*)Cache.AutoArmor.RiotPants[0];
//                            Cache.LPC->EquipPawnItem(Pants->ItemId);
//                            ReplaceLegs = false;
//                        }
//                    }
//                    if (ReplaceBoots)
//                    {
//                        if (Cache.AutoArmor.TekBoots.size() > 0)
//                        {
//                            auto Boots = (UPrimalItem*)Cache.AutoArmor.TekBoots[0];
//                            if (Boots->CanUse(false)) { Cache.LPC->EquipPawnItem(Boots->ItemId); }
//                            else
//                            {
//                                if (Cache.AutoArmor.FlakBoots.size() != 0)
//                                {
//                                    Boots = (UPrimalItem*)Cache.AutoArmor.FlakBoots[0];
//                                    Cache.LPC->EquipPawnItem(Boots->ItemId);
//                                    ReplaceBoots = false;
//                                }
//                                else if (Cache.AutoArmor.RiotBoots.size() != 0)
//                                {
//                                    Boots = (UPrimalItem*)Cache.AutoArmor.RiotBoots[0];
//                                    Cache.LPC->EquipPawnItem(Boots->ItemId);
//                                    ReplaceBoots = false;
//                                }
//                            }
//                        }
//                        if (Cache.AutoArmor.FlakBoots.size() > 0 && ReplaceBoots)
//                        {
//                            auto Boots = (UPrimalItem*)Cache.AutoArmor.FlakBoots[0];
//                            Cache.LPC->EquipPawnItem(Boots->ItemId);
//                            ReplaceBoots = false;
//                        }
//                        else if (Cache.AutoArmor.RiotBoots.size() > 0 && ReplaceBoots)
//                        {
//                            auto Boots = (UPrimalItem*)Cache.AutoArmor.RiotBoots[0];
//                            Cache.LPC->EquipPawnItem(Boots->ItemId);
//                            ReplaceBoots = false;
//                        }
//                    }
//                }
//            }
//
//            if (Settings.Misc.InstantDinoTurn && Cache.LocalActor)
//            {
//                auto RiddenDino = Cache.LocalActor->GetBasedOrSeatingOnDino();
//                if (RiddenDino)
//                {
//                    RiddenDino->RiderMovementSpeedScalingRotationRatePowerMultiplier = 10000;
//                    RiddenDino->WalkingRotationRateModifier = 10000;
//                    RiddenDino->RiderFlyingRotationRateModifier = 10000;
//                }
//            }
//
//        } while (false);
//    }
//    catch (std::exception E)
//    {
//        Logger::Log("[EXECUTION_LOOP]: Execution Loop Failed! [EXCEPTION]: %s\n", E);
//    }
//    catch (...)
//    {
//    }
//    ImGui::End();
//    ImGui::PopStyleVar(2);
//    ImGui::PopStyleColor(1);
//    if (Settings.IsMenuOpen)
//    {
//        Renderer::HookInput();
//
//        ImGui::Begin("[Ching Chong Hook]", &Settings.IsMenuOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
//        //ImGui::SetWindowPos(ImVec2(Cache.WindowSizeX / 2 - Settings.MenuSizeX / 2, Cache.WindowSizeY / 2.5 - Settings.MenuSizeY / 2));
//        ImGui::SetWindowSize(ImVec2(Settings.MenuSizeX, Settings.MenuSizeY));
//
//        decorations();
//        tabss();
//
//        ImGui::End();
//    }
//    D3D.Ctx->OMSetRenderTargets(1, &D3D.RenderTargetView, nullptr);
//    ImGui::Render();
//    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
//    return D3D.OriginalPresent(SwapChain, SyncInterval, Flags);
//}

std::wstring RetrieveUWPFolder(const wchar_t* fileName)
{
    wchar_t szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath)))
    {
        std::wstringstream data;
        data << szPath << L"\\" << fileName;
        return data.str();
    }
    else
    {
        throw std::runtime_error("Could not retrieve folder");
    }
}

std::wstring InsertDoubleSlashes(std::wstring CurrentPath)
{
    std::wstringstream Stream;
    for (const auto& ch : CurrentPath)
    {
        if (ch == '\\') Stream << L"\\\\";
        else Stream << ch;
    }
    return Stream.str();
}

void InitCheat()
{
    try
    {
        std::wstring loggerFile = InsertDoubleSlashes(RetrieveUWPFolder(L"logger.gc"));

        uintptr_t GameBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
        UWorld::GWorld = *reinterpret_cast<decltype(UWorld::GWorld)*>(GameBase + Settings.uworld);
        Cache.GameBase = GameBase;

        Settings.OriginalGetBoneLocation = reinterpret_cast<decltype(Settings.OriginalGetBoneLocation)>(PatternScan(GameBase, "40 57 48 83 EC 70 48 C7 44 24 ? ? ? ? ? 48 89 9C 24 ? ? ? ? 48 8B DA 48 8B F9 49 8B D0 E8 ? ? ? ? 83 F8 FF"));
        Settings.GetBoneLocation = reinterpret_cast<decltype(Settings.GetBoneLocation)>(Settings.OriginalGetBoneLocation);

        Settings.OriginalInputKey = reinterpret_cast<decltype(Settings.OriginalInputKey)>(PatternScan(GameBase, "48 8B C4 48 89 50 10 56 57 41 56 48 81 EC ? ? ? ? 48 C7 44 24 ? ? ? ? ? 48 89 58 18 48 89 68 20 0F 29 70 D8"));
        Settings.InputKey = reinterpret_cast<decltype(Settings.InputKey)>(Settings.OriginalInputKey);

        if (!Logger::Init(loggerFile.c_str()))
            throw std::runtime_error("[LOGGER]: Failed to initialize logger!");

        if (!Renderer::Init())
            throw std::runtime_error("[RENDERER]: Renderer Failed To Initialize!");

        if (!InitSDK())
            throw std::runtime_error("[SDK]: SDK Failed To Initialize!");

        Logger::Log("[GObjects]: %p\n", UObject::GObjects);
        Logger::Log("[GNames]: %p\n", FName::GNames);
        Logger::Log("[UWorld]: %p\n", UWorld::GWorld);
        Logger::Log("[PersistentLevel]: %p\n", UWorld::GWorld->PersistentLevel);
    }
    catch (const std::exception& ex)
    {
        Logger::Log(ex.what());
    }
}

void AimbotThread() 
{
    do
    {
        try
        {
            UWorld* GWorld = UWorld::GWorld;
            void* GameState = GWorld->GameState;
            UPlayer* LocalPlayer = GWorld->OwningGameInstance->LocalPlayers[0];
            auto PC = LocalPlayer->PlayerController;
            FVector2D ActorScreenLocation;
            auto WinSizeX = ImGui::GetWindowSize().x;
            auto WinSizeY = ImGui::GetWindowSize().y;
            FVector BoneWorldLocation;
            FVector2D BoneScreenLocation;
            int LastDistance = Cache.WindowSizeX;
            bool AimKeyDown = Cache.LPC->IsInputKeyDown(Settings.Keybinds.AimbotKey);
            if (!AimKeyDown) { Cache.LPC->IgnoreLookInput(false), Settings.Aimbot.AimLocked = false, Cache.AimbotTarget = nullptr; }


            if (!Settings.Aimbot.AimLocked)
            {
                auto Actors = UWorld::GWorld->PersistentLevel->Actors;
                for (int i = 0; i < Actors.Count; i++)
                {
                    auto Actor = Actors[i];
                    if (Actor && Actor->IsPlayer())
                    {
                        Settings.GetBoneLocation(Actor->MeshComponent, &BoneWorldLocation, Actor->MeshComponent->GetBoneName(AimBone), 0);
                        if (!W2S(BoneWorldLocation, BoneScreenLocation) || !Renderer::Drawing::WithinAimFOV(Cache.WindowSizeX / 2, Cache.WindowSizeY / 2, Settings.Visuals.FOVSize, BoneScreenLocation.X, BoneScreenLocation.Y)) continue;
                        if (Actor->IsDead() || Actor->IsLocalPlayer()) continue;
                        if (Settings.Aimbot.TargetTribe)
                        {
                            if (Actor->TribeName.IsValid() && Actor->TribeName.ToString() == Cache.LocalActor->TribeName.ToString()) continue;
                        }
                        if (!Settings.Aimbot.TargetSleepers && !Actor->IsConscious()) continue;
                        if (Cache.AimbotTarget) {
                            FVector TheirVelocity = Cache.AimbotTarget->CharacterMovement->Velocity;
                            FVector MyVelocity = Cache.LocalActor->CharacterMovement->Velocity;
                            if (TheirVelocity != MyVelocity)
                            {
                                FVector TheSpeed = MyVelocity - TheirVelocity;
                                BoneWorldLocation += BoneWorldLocation + (TheSpeed / TheirVelocity);
                            }
                        }
                        if (Settings.Aimbot.VisibleOnly && !Cache.LPC->LineOfSightTo(Actor, Cache.LPC->PlayerCameraManager->GetCameraLocation(), false)) continue;
                        int Distance = Renderer::Drawing::ReturnDistance(Cache.WindowSizeX / 2, Cache.WindowSizeY / 2, BoneScreenLocation.X, BoneScreenLocation.Y);
                        if (Distance < LastDistance) { LastDistance = Distance, Cache.AimbotTarget = Actor; }
                    }
                }
                if (AimKeyDown && Cache.AimbotTarget)
                {
                    Cache.LPC->IgnoreLookInput(true), Settings.Aimbot.AimLocked = true;
                }
            }

            if (Settings.Aimbot.AimLocked && Cache.AimbotTarget)
            {
                auto Target = Cache.AimbotTarget;
                FRotator ControlRotation = Cache.LPC->PlayerCameraManager->GetCameraRotation();
                FVector CameraLoc = Cache.LPC->PlayerCameraManager->GetCameraLocation();
                if (Target)
                {
                    if (Target->IsDead()) { Cache.LPC->IgnoreLookInput(false), Settings.Aimbot.AimLocked = false, Cache.AimbotTarget = nullptr; }
                    if (Target->MeshComponent)
                    {
                        Settings.GetBoneLocation(Target->MeshComponent, &BoneWorldLocation, Target->MeshComponent->GetBoneName(AimBone), 0);
                        auto Rotator = Cache.LocalPlayer->FindLookAtRotation(Cache.LPC->PlayerCameraManager->GetCameraLocation(), BoneWorldLocation);
                        if (Rotator.hasValue() && Rotator.Yaw != 0 && Rotator.Pitch != 0)
                        {
                            Rotator.Roll = 0;
                            //Cache.LPC->SetControlRotation(Rotator);
                            Cache.LPC->ControlRotation = Rotator;
                        }
                    }
                }
            }
        }
        catch (const std::exception&)
        {
                
        }
    } while (false);
}
