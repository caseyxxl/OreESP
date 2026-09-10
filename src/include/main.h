#pragma once
#include <Windows.h>
#include <jni.h>
#include <glad/glad.h>
#include <vector>
#include <mutex>

extern HMODULE       g_hModule;
extern JavaVM*       g_jvm;
extern thread_local JNIEnv* g_env;
extern volatile bool g_running;

struct OreEntry {
    double x, y, z;
    int    id;
};

extern std::vector<OreEntry> g_ores;
extern std::mutex            g_oresMutex;

bool InitJNI();
void ShutdownJNI();
void UpdateOreList();

bool InstallHooks();
void RemoveHooks();

void DrawESP();
bool IsOre(int id);
