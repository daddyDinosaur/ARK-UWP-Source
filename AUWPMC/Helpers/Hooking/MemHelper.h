#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <string>
#include <optional>
#include <Psapi.h>
#include "Helpers/Hooking/magic_enum.hpp"
#include "Helpers/Hooking/HookLib.h"
#include "Helpers/Hooking/D3D11Hooking.hpp"
#include "Helpers/imgui/imgui.h"
#include "Helpers/Logger/Logger.h"
#include "MemHelper.h"

#define M_PI       3.14159265358979323846

uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature);