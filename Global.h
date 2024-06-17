#pragma once
#define DEBUG

void dummy_fnc(...) {};
bool isRunning = true;

#ifdef DEBUG
#define c_log(...) printf_s(__VA_ARGS__)
#else
#define c_log(...) dummy_fnc()
#endif

#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <Psapi.h>
#include <map>

#include <d2d1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <dxgi1_4.h>
#include <d2d1_3.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "Dependencies/minhook/Minhook.h"
#include "Dependencies/kiero/kiero.h"
#include "Dependencies/imgui/imgui.h"
#include "Dependencies/imgui/imgui_impl_dx9.h"
#include "Dependencies/imgui/imgui_impl_dx11.h"
#include "Dependencies/imgui/imgui_impl_dx12.h"
#include "Dependencies/imgui/imgui_impl_win32.h"

#include <Utils.h>
#include <SDK.h>


namespace Global {
	inline ImVec2 MousePos = { 0, 0 };
	inline HWND Window = nullptr;
	inline std::map<uint64_t, bool> Keymap = {};
	inline uintptr_t base = 0x0;

	const char* header = "BedrockZoom 2 - PyNami";

	inline bool  isRunning = true;
	inline bool  RenderWindow = true;
	inline bool  NoVsync = false;
	inline bool  DevInfo = false;
	inline bool  HoldZoom = false;
	inline int   TitleBarHeight;
	inline float DefFOV = -1.f;
	inline float ZoomMag = 30.f;
	inline float ZoomSpeed = 4.f;
	inline float SensScale = 90.f;
	inline int   Mode = 0;
	inline char* Modes[2] = {
		(char*)"Smooth",
		(char*)"Instant"
	};
}

namespace Sigs {
	const char* GetFOV = "48 8B C4 48 89 58 ? 48 89 70 ? 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 48 8B 05";
	const char* Mouse = "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 44 0F B7 BC 24";
	const char* GetSens = "40 53 48 83 EC ? 80 B9 ? ? ? ? ? 8B DA";
	const char* Keymap = "48 83 EC ? ? ? C1 4C 8D 05";

	void* aFOV = 0x0;
	void* aMouse = 0x0;
	void* aSens = 0x0;
	void* aKeymap = 0x0;
}

