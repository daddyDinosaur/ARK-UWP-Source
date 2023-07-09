#include "Cheat.h"
#include "SDK.h"

bool InitSDK(const std::string& moduleName, const uintptr_t gObjectsOffset, const uintptr_t gNamesOffset)
{
	auto mBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleA(moduleName.c_str()));
	if (mBaseAddress == 0x00)
		return false;

	UObject::GObjects = reinterpret_cast<decltype(UObject::GObjects)>(mBaseAddress + gObjectsOffset);
	FName::GNames = *reinterpret_cast<decltype(FName::GNames)*>(mBaseAddress + gNamesOffset);
	return true;
}

bool InitSDK()
{
	return InitSDK("ShooterGame.exe", Settings.objects, Settings.names);
}

std::string UObject::GetName() const
{
	std::string name(Name.GetName());
	if (Name.Number > 0)
	{
		name += '_' + std::to_string(Name.Number);
	}
	auto pos = name.rfind('/');
	if (pos == std::string::npos)
	{
		return name;
	}
	return name.substr(pos + 1);
}

std::string UObject::GetFullName() const
{
	std::string name;
	if (Class != nullptr)
	{
		std::string temp;
		for (auto p = Outer; p; p = p->Outer)
		{
			temp = p->GetName() + "." + temp;
		}

		name = Class->GetName();
		name += " ";
		name += temp;
		name += GetName();
	}
	return name;
}

bool UObject::IsA(UClass* cmp) const
{
	for (auto super = Class; super; super = static_cast<UClass*>(super->SuperField))
	{
		if (super == cmp)
		{
			return true;
		}
	}
	return false;
}