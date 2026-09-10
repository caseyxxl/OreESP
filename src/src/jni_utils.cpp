#include "main.h"
#include <cstdio>

// ==== 1.8.9 vanilla obfuscated isimleri ====
// ave = Minecraft
// adm = World
// bew = EntityPlayerSP
// pk  = Entity
// cj  = BlockPos
// afh = Block
// alz = IBlockState

static jclass    c_Minecraft = nullptr;
static jmethodID m_getMinecraft = nullptr;
static jfieldID  f_theWorld  = nullptr;
static jfieldID  f_thePlayer = nullptr;
static jfieldID  f_posX      = nullptr;
static jfieldID  f_posY      = nullptr;
static jfieldID  f_posZ      = nullptr;
static jclass    c_BlockPos  = nullptr;
static jmethodID m_bpCtor    = nullptr;
static jclass    c_World     = nullptr;
static jmethodID m_getBlockState = nullptr;
static jclass    c_IBlockState = nullptr;
static jmethodID m_getBlock  = nullptr;
static jclass    c_Block     = nullptr;
static jmethodID m_getIdFromBlock = nullptr;

static bool try_class(JNIEnv* env, const char* name, jclass& out) {
    jclass c = env->FindClass(name);
    if (!c) { env->ExceptionClear(); return false; }
    out = (jclass)env->NewGlobalRef(c);
    env->DeleteLocalRef(c);
    return true;
}

bool InitJNI() {
    jsize count = 0;
    if (JNI_GetCreatedJavaVMs(&g_jvm, 1, &count) != JNI_OK || count == 0) return false;
    if (g_jvm->AttachCurrentThread((void**)&g_env, nullptr) != JNI_OK) return false;

    if (!try_class(g_env, "ave", c_Minecraft)) return false;
    if (!try_class(g_env, "adm", c_World))     return false;
    if (!try_class(g_env, "bew", c_BlockPos))  return false; // geçici, sonra değiştirilecek

    // Gerçek BlockPos
    c_BlockPos = nullptr;
    if (!try_class(g_env, "cj", c_BlockPos)) return false;

    jclass c_Entity = nullptr;
    if (!try_class(g_env, "pk", c_Entity)) return false;

    jclass c_IBS = nullptr;
    if (!try_class(g_env, "alz", c_IBS)) c_IBS = nullptr;

    jclass c_Blk = nullptr;
    if (!try_class(g_env, "afh", c_Blk)) c_Blk = nullptr;

    // Metot/alan lookup
    m_getMinecraft = g_env->GetStaticMethodID(c_Minecraft, "z", "()Lave;");
    if (!m_getMinecraft) { g_env->ExceptionClear(); return false; }

    f_theWorld  = g_env->GetFieldID(c_Minecraft, "f", "Ladm;");
    f_thePlayer = g_env->GetFieldID(c_Minecraft, "h", "Lbew;");
    if (!f_theWorld || !f_thePlayer) { g_env->ExceptionClear(); return false; }

    f_posX = g_env->GetFieldID(c_Entity, "s", "D");
    f_posY = g_env->GetFieldID(c_Entity, "t", "D");
    f_posZ = g_env->GetFieldID(c_Entity, "u", "D");
    if (!f_posX || !f_posY || !f_posZ) { g_env->ExceptionClear(); return false; }

    m_bpCtor = g_env->GetMethodID(c_BlockPos, "<init>", "(III)V");
    if (!m_bpCtor) { g_env->ExceptionClear(); return false; }

    m_getBlockState = g_env->GetMethodID(c_World, "p", "(Lcj;)Lalz;");
    if (!m_getBlockState) { g_env->ExceptionClear(); return false; }

    if (c_IBS) {
        m_getBlock = g_env->GetMethodID(c_IBS, "c", "()Lafh;");
        if (!m_getBlock) g_env->ExceptionClear();
    }

    if (c_Blk) {
        m_getIdFromBlock = g_env->GetStaticMethodID(c_Blk, "a", "(Lafh;)I");
        if (!m_getIdFromBlock) g_env->ExceptionClear();
    }

    return m_getMinecraft && f_theWorld && f_thePlayer
        && f_posX && m_bpCtor && m_getBlockState
        && m_getBlock && m_getIdFromBlock;
}

void ShutdownJNI() {
    if (g_jvm) g_jvm->DetachCurrentThread();
}

void UpdateOreList() {
    if (!g_env || !m_getMinecraft || !m_getBlock) return;

    jobject mc = g_env->CallStaticObjectMethod(c_Minecraft, m_getMinecraft);
    if (!mc) return;

    jobject world  = g_env->GetObjectField(mc, f_theWorld);
    jobject player = g_env->GetObjectField(mc, f_thePlayer);
    if (!world || !player) {
        if (world)  g_env->DeleteLocalRef(world);
        if (player) g_env->DeleteLocalRef(player);
        g_env->DeleteLocalRef(mc);
        return;
    }

    double px = g_env->GetDoubleField(player, f_posX);
    double py = g_env->GetDoubleField(player, f_posY);
    double pz = g_env->GetDoubleField(player, f_posZ);

    jobject bp = g_env->NewObject(c_BlockPos, m_bpCtor, 0, 0, 0);
    if (!bp) {
        g_env->DeleteLocalRef(player); g_env->DeleteLocalRef(world);
        g_env->DeleteLocalRef(mc); return;
    }

    std::lock_guard<std::mutex> lk(g_oresMutex);
    g_ores.clear();

    const int R = 16;
    int ox = (int)px, oy = (int)py, oz = (int)pz;

    for (int x = ox - R; x <= ox + R; x++) {
        for (int y = (oy - R < 0 ? 0 : oy - R); y <= (oy + R > 255 ? 255 : oy + R); y++) {
            for (int z = oz - R; z <= oz + R; z++) {
                // BlockPos alanları final, bu yüzden her seferinde yeni obje lazım
                // Ama performans için bu döngüyü chunk-based yapmak gerekir.
                // Şimdilik basit tutalım:
                // (İleride optimize edilecek)
            }
        }
    }

    // NOT: Yukarıdaki döngü performans için boş bırakıldı.
    // İlk hedef DLL'in derlenmesi. Sonra burayı dolduracağız.

    g_env->DeleteLocalRef(bp);
    g_env->DeleteLocalRef(player);
    g_env->DeleteLocalRef(world);
    g_env->DeleteLocalRef(mc);
}
