#pragma once


#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? (x - '0') : 0))
#define GET_BYTE(x) ((GET_BITS(x[0]) << 4) | GET_BITS(x[1]))


template <typename R, typename... Args>
R CallFunc(void* func, Args... args)
{
    return ((R(*)(Args...))func)(args...);
}

template <typename T>
bool HookFunction(void* pTarget, T pDetour, void* pOriginal, const char* name) {
    c_log("Info: Creating hook %s...\n", name);

    if (MH_CreateHook(pTarget, pDetour, (LPVOID*)pOriginal) != MH_OK)
    {
        c_log("Error: Failed to create hook (%s, %p)\n", name, pTarget);
        return false;
    }

    if (MH_EnableHook(pTarget) != MH_OK)
    {
        c_log("Error: Failed to enable (%s, %p)\n", name, pTarget);
        return false;
    }

    c_log("Success: Enabled hook for %s (%p)\n", name, pTarget);

    return true;
}


static void* findSig(const char* sig) {
    const char* pattern = sig;
    uintptr_t first_match = 0;
    static const uintptr_t range_start = reinterpret_cast<uintptr_t>(GetModuleHandleA("Minecraft.Windows.exe"));
    static MODULEINFO mi_mod_info;
    static bool init = false;

    if (!init) {
        init = true;
        GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(range_start), &mi_mod_info, sizeof(MODULEINFO));
    }

    static const uintptr_t range_end = range_start + mi_mod_info.SizeOfImage;
    BYTE pat_byte = GET_BYTE(pattern);
    const char* old_pat = pattern;

    for (uintptr_t p_cur = range_start; p_cur < range_end; p_cur++) {
        if (!*pattern) {
            return reinterpret_cast<void*>(first_match);
        }

        while (*(PBYTE)pattern == ' ') {
            pattern++;
        }

        if (!*pattern) {
            return reinterpret_cast<void*>(first_match);
        }

        if (old_pat != pattern) {
            old_pat = pattern;
            if (*(PBYTE)pattern != '\?') {
                pat_byte = GET_BYTE(pattern);
            }
        }

        if (*(PBYTE)pattern == '\?' || *(BYTE*)p_cur == pat_byte) {
            if (!first_match) {
                first_match = p_cur;
            }

            if (!pattern[2]) {
                return reinterpret_cast<void*>(first_match);
            }

            pattern += 2;
        }
        else {
            pattern = sig;
            first_match = 0;
        }
    }
}