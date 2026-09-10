#include "main.h"

HMODULE       g_hModule  = nullptr;
JavaVM*       g_jvm      = nullptr;
thread_local JNIEnv* g_env = nullptr;
volatile bool g_running  = false;

std::vector<OreEntry> g_ores;
std::mutex            g_oresMutex;

bool IsOre(int id) {
    switch (id) {
        case 14:  case 15:  case 16:  case 21:
        case 56:  case 73:  case 129:
            return true;
        default: return false;
    }
}

DWORD WINAPI MainThread(LPVOID) {
    if (!InitJNI()) {
        MessageBoxA(nullptr, "JVM bulunamadi. Minecraft calisiyor mu?", "OreESP", MB_ICONERROR);
        return 0;
    }
    if (!InstallHooks()) {
        MessageBoxA(nullptr, "Hook kurulumu basarisiz.", "OreESP", MB_ICONERROR);
        return 0;
    }
    g_running = true;
    while (g_running) {
        UpdateOreList();
        Sleep(100);
    }
    RemoveHooks();
    ShutdownJNI();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    } else if (reason == DLL_PROCESS_DETACH) {
        g_running = false;
    }
    return TRUE;
}
