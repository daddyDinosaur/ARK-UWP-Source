#include "Cheat.h"
#include <d3d11.h>
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"
#include "imgui\imgui_internal.h"

void InitCheat()
{
    RenderMenu();
}


//void InitCheat()
//{
//    KeyAuthApp.init();
//
//    std::string key;
//
//    key = "[CCH]-M57ALX-3WFRR3";
//
//    KeyAuthApp.license(key);
//
//    RenderMenu();
//}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitCheat, 0, 0, NULL));
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

