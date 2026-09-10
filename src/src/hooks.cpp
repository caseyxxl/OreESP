#include "main.h"
#include <MinHook.h>

typedef BOOL(WINAPI* wglSwapBuffers_t)(HDC);
static wglSwapBuffers_t o_wglSwapBuffers = nullptr;

static BOOL WINAPI hk_wglSwapBuffers(HDC hdc) {
    if (g_running) DrawESP();
    return o_wglSwapBuffers(hdc);
}

bool InstallHooks() {
    if (MH_Initialize() != MH_OK) return false;

    HMODULE gl = GetModuleHandleA("opengl32.dll");
    if (!gl) return false;
    void* target = (void*)GetProcAddress(gl, "wglSwapBuffers");
    if (!target) return false;

    if (MH_CreateHook(target, &hk_wglSwapBuffers,
                      (void**)&o_wglSwapBuffers) != MH_OK) return false;
    MH_EnableHook(target);
    return true;
}

void RemoveHooks() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
