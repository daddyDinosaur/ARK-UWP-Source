#include "Cheat.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitCheat, 0, 0, NULL));
        break;
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

