#include <Global.h>
#include <Hooks.h>


void Main() {
    FILE* fDummy;

#ifdef DEBUG
    AllocConsole();

    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    printf_s("Loaded!\n");
#endif

    if (MH_Initialize() != MH_OK)
        c_log("Could not initialize MinHook\n");

    EnableAll();
}


DWORD APIENTRY Eject(HMODULE lpParam)
{

    while (Global::isRunning) {
        if ((Global::Keymap[VK_CONTROL] && Global::Keymap['L']))
            Global::isRunning = false;

        Sleep(0);
    }

    Sleep(50);
    
    kiero::unbind(kiero::getRenderType() == kiero::RenderType::D3D12 ? 140 : 8);
    kiero::shutdown();

    MH_DisableHook(MH_ALL_HOOKS); // Disable all Hooks.
    MH_RemoveHook(MH_ALL_HOOKS); // Remove all Hooks.
    
    FreeLibraryAndExitThread(lpParam, 1); // Uninject the client completly.
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, 0);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Eject, hModule, 0, 0);
    }
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
    }
    return TRUE;
}
