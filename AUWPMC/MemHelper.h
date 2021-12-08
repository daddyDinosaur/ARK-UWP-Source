#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <string>
#include <optional>
#include <Psapi.h>
#include "Hooking/magic_enum.hpp"
#include "Hooking/HookLib.h"
#include "Hooking/D3D11Hooking.hpp"
#include "imgui/imgui.h"
#include "Logger.h"
#include "MemHelper.h"

#define M_PI       3.14159265358979323846

uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature);