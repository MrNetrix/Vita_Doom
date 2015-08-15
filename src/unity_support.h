#ifdef __UNITY_SUPPORT_H__
#error unity_support.h must only be included once.
#endif

#define __UNITY_SUPPORT_H__

#include <psp2/moduleinfo.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>

typedef int PsvUID;            ///< PsvUID type
typedef long long int PsvOff;       ///< PsvOff type
typedef int PsvSSize;          ///< PsvSSize type

typedef unsigned char u8_t;             ///< Unsigned 8-bit type
typedef unsigned short int u16_t;       ///< Unsigned 16-bit type
typedef unsigned int u32_t;             ///< Unsigned 32-bit type
typedef unsigned long long int u64_t;        ///< Unsigned 64-bit type
typedef char s8_t;             ///< Signed 8-bit type
typedef short int s16_t;       ///< Signed 16-bit type
typedef int s32_t;             ///< Signed 32-bit type
typedef long long int s64_t;        ///< Signed 64-bit type

typedef struct segment_info
{
    u32_t           size;   // this structure size (0x18)
    u32_t           perms;  // probably rwx in low bits
    void            *vaddr; // address in memory
    u32_t           memsz;  // size in memory
    u32_t           flags;  // meanig unknown
    u32_t           res;    // unused?
} segment_info_t;

typedef struct loaded_module_info
{
    u32_t           size;               // 0x1B8 for Vita 1.x
    u32_t           handle;             // kernel module handle?
    u32_t           flags;              // some bits. could be priority or whatnot
    char            module_name[28];
    u32_t           unkn_28;
    void            *module_start;
    u32_t           unkn_30;
    void            *module_stop;
    void            *exidx_start;
    void            *exidx_end;
    u32_t           unkn_40;
    u32_t           unkn_44;
    void            *tls_init_data;
    u32_t           tls_init_size;
    u32_t           tls_area_size;
    char            file_path[256];
    segment_info_t  segments[4];
    u32_t           type;       // 6 = user-mode PRX?
} loaded_module_info_t;

#define MAX_LOADED_MODS 128

static loaded_module_info_t m_mod_infos[MAX_LOADED_MODS];

int
uvl_resolve_add_module(PsvUID modid, ///< UID of the module
int index)  ///< An OR combination of flags (see defined "Search flags for importing loaded modules") directing the search
{
    loaded_module_info_t m_mod_info;
    m_mod_info.size = sizeof (loaded_module_info_t); // should be 440

    if (sceKernelGetModuleInfo(modid, &m_mod_info) < 0)
    {
        printf("Error getting info for mod 0x%08X", modid);
        return -1;
    }

    memcpy(m_mod_infos + index, &m_mod_info, sizeof(loaded_module_info_t));

    return 0;
}

int
uvl_resolve_add_all_modules() ///< An OR combination of flags (see defined "Search flags for importing loaded modules") directing the search
{
    PsvUID mod_list[MAX_LOADED_MODS];
    u32_t num_loaded = MAX_LOADED_MODS;
    int i;

    //printf("Getting list of loaded modules.");
    if (sceKernelGetModuleList(0xFF, mod_list, &num_loaded) < 0)
    {
        printf("Failed to get module list.");
        return -1;
    }

    //printf("Found %u loaded modules.", num_loaded);

    for (i = 0; i < num_loaded; i++)
    {
        if (uvl_resolve_add_module(mod_list[i], i) < 0)
        {
            printf("Failed to add module %u: 0x%08X. Continuing.", i, mod_list[i]);
            continue;
        }
    }

    return 0;
}


typedef SceUInt32* (*getGraphicsClassPtr_func)();
typedef SceUInt32(*destroyGraphics_func)(void*);

static SceUInt32 newGraphicsClassVtable[250];
static destroyGraphics_func destroyGraphics;
static int sGraphicsCleanedUp = 0;

void CleanUpGraphicsHook(void* r0)
{
    printf("Hooked a thread! 0x%x", r0);

    destroyGraphics(r0);

    printf("Graphics destroyed! 0x%x", r0);
    
    sGraphicsCleanedUp = 1;

    sceKernelExitDeleteThread(0);
}

#define GET_FUNC(name, base, version) name ## Syscall = (name ## Syscall_func) (base + name ## Offset ## version);
#define DEF_FUNC(name) name ## Syscall_func name ## Syscall
#define DEF_FUNC_OFFSET(name, offset, version) SceUInt32 name ## Offset ## version = offset;

typedef SceUInt32(*sceDisplayWaitVblankStartSyscall_func)();
DEF_FUNC(sceDisplayWaitVblankStart);

typedef SceUInt32(*sceCtrlPeekBufferPositiveSyscall_func)();
DEF_FUNC(sceCtrlPeekBufferPositive);

typedef SceUInt32(*sceCtrlSetSamplingModeSyscall_func)();
DEF_FUNC(sceCtrlSetSamplingMode);

DEF_FUNC_OFFSET(sceDisplayWaitVblankStart, 0x9EEC6C, 105);
DEF_FUNC_OFFSET(sceCtrlPeekBufferPositive, 0x9EEC8C, 105);
DEF_FUNC_OFFSET(sceCtrlSetSamplingMode, 0x9EEC7C, 105);

DEF_FUNC_OFFSET(sceDisplayWaitVblankStart, 0x9E91BC, 106);
DEF_FUNC_OFFSET(sceCtrlPeekBufferPositive, 0x9E91CC, 106);

SceUInt32 mainThreadOffset105 = 0x0;
SceUInt32 graphicsClassVtableOffset105 = 0xB92B00;

SceUInt32 mainThreadOffset106 = 0x1C46C;
SceUInt32 graphicsClassVtableOffset106 = 0xB63A30;

SceUInt32 graphicsClassPtrOffset = 0xEB8; // Same for both
SceUInt32 graphicsClassVtableOffset = 0x0;

SceUInt32 kernelbase = 0x0;
SceUInt32 unitybaseseg0 = 0x0;
SceUInt32 unitybaseseg1 = 0x0;
int unity_version = 0x0;

void GetFunctions()
{
	printf("unity_version: 0x%x", unity_version);
	
    if (unity_version == 0x105)
    {
        GET_FUNC(sceDisplayWaitVblankStart, unitybaseseg0, 105);
        GET_FUNC(sceCtrlPeekBufferPositive, unitybaseseg0, 105);
        GET_FUNC(sceCtrlSetSamplingMode, unitybaseseg0, 105);
        graphicsClassVtableOffset = graphicsClassVtableOffset105;
    }
    else
    {
        GET_FUNC(sceDisplayWaitVblankStart, unitybaseseg0, 106);
        GET_FUNC(sceCtrlPeekBufferPositive, unitybaseseg0, 106);
        graphicsClassVtableOffset = graphicsClassVtableOffset106;
    }
}

int unity_cleanup()
{
    printf("Cleaning Unity");
    
    memset(m_mod_infos, 0, sizeof(loaded_module_info_t) * MAX_LOADED_MODS);

    uvl_resolve_add_all_modules();

    int i;
    for (i = 0; i < MAX_LOADED_MODS; ++i)
    {
        if ((m_mod_infos + i)->size == 0)
        {
            continue;
        }

        if (strcmp((m_mod_infos + i)->module_name, "SceLibKernel") == 0)
        {
            kernelbase = (SceUInt32) (m_mod_infos + i)->segments[0].vaddr;
        }
        else if (strcmp((m_mod_infos + i)->module_name, "UnityPlayer") == 0)
        {
            unity_version = 0x105;
            unitybaseseg0 = (SceUInt32) (m_mod_infos + i)->segments[0].vaddr;
            unitybaseseg1 = (SceUInt32) (m_mod_infos + i)->segments[1].vaddr;
        }
        else if (strcmp((m_mod_infos + i)->module_name, "UnityPlayer_4370_Develop") == 0)
        {
            unity_version = 0x106;
            unitybaseseg0 = (SceUInt32) (m_mod_infos + i)->segments[0].vaddr;
            unitybaseseg1 = (SceUInt32) (m_mod_infos + i)->segments[1].vaddr;
        }
    }

    //printf("libkernel: 0x%x", kernelbase);
    //printf("unitybaseseg0 base: 0x%x", unitybaseseg0);
    //printf("unitybaseseg1 base: 0x%x", unitybaseseg1);

    GetFunctions();
    
    //SceUInt32* mainthreadptr = (SceUInt32*)(unitybaseseg1 + mainThreadOffset);
    //printf("Target addr: 0x%x", mainthreadptr);
    
    if (unity_version != 0x0)
    {
        SceUInt32* classonheap = *(SceUInt32**) (unitybaseseg1 + graphicsClassPtrOffset);

        //printf("classonheap val: 0x%x", (SceUInt32) classonheap);

        SceUInt32* vtable = (SceUInt32*) *classonheap;
        destroyGraphics = (destroyGraphics_func) (*(vtable + 1));

        //printf("destroyGraphics: 0x%x", (SceUInt32) destroyGraphics);

        SceUInt32 cleanUpHook = (SceUInt32)&CleanUpGraphicsHook;
        for (i = 0; i < 250; ++i)
        {
            newGraphicsClassVtable[i] = cleanUpHook;
        }

        printf("classonheap will be val: 0x%x is 0x%x", (SceUInt32) classonheap, newGraphicsClassVtable);

        *classonheap = (SceUInt32)newGraphicsClassVtable; // Overwrite the graphics class vtable.

        while (sGraphicsCleanedUp == 0) // Give the graphics thread time to be hooked and exit.
        {
            sceKernelDelayThread(1000 * 500);
        }
    }

	return 0;
}
