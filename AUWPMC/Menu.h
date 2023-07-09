#pragma once
#include <map>
#include <string>
#include "Helpers/imgui\imgui.h"
#include "Helpers/imgui\imgui_impl_win32.h"
#include "Helpers/imgui\imgui_impl_dx11.h"
#include "Helpers/imgui\imgui_internal.h"
#include "Cheat.h"

float CachedFramerates[1000];                                               
int AimBone = 4; // Default chest, head is 8
int PossibleDinos = 1;
int PossibleAimKey = 6;
int PossibleMenuKey = 2;
const char* CurrentContainer = "Supply Crate";

static int tabs;
static int subtabs;

ImFont* zzzz = nullptr;
ImFont* icons = nullptr;
ImVec2 pos;
ImDrawList* draw;

std::map<std::string, int> bone_data = {
    {"Head", 8},
    {"Chest", 4},
    {"Pelvis", 1},
    {"Hands", 38},
    {"Legs", 82}
};

struct Tab {
    const char* label;
    const char* desc;
    ImVec2 size;
};

Tab tab_data[] = {
    {"A", "AIMBOT", ImVec2(91, 87)},
    {"C", "PLAYERS", ImVec2(91, 87)},
    {"D", "VISUALS", ImVec2(91, 87)},
    {"E", "MISC", ImVec2(91, 87)},
    {"F", "SETTINGS", ImVec2(91, 87)}
};

const char* subtab_data[] = { "HELMET POPPER", "EXTRA FEATURES", "PLAYER ESP", "EXTRA INFO", "DINOS", "OTHER ESP'S", "MISC FEATURES", "COLORS", "STATS", "BINDS & CONFIGS" };

static float PlayerColor1[4] = { 1.0f, 0.1f, 0.1f, 255 };
static float CrosshairColor1[4] = { 0.0f, 0.0f, 1.0f, 255 };
static float CrosshairColorFOV1[4] = { 0.0f, 0.0f, 1.0f, 255 };
static float TamedDinoColor1[4] = { 1.0f, 0.1f, 0.1f, 255 };
static float WildDinoColor1[4] = { 0.0f, 0.1f, 1.0f, 255 };
static float ContainerColor1[4] = { 0.9f, 1.0f, 0.0f, 255 };
static float TurretColor1[4] = { 0.4f, 0.0f, 1.0f, 255 };
static float FilteredDinoColor1[4] = { 0.9f, 0.0f, 1.0f, 255 };

float MaxArr(float arr[], int n)
{
    int i;
    float max = arr[0];
    for (i = 1; i < n; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
        return max;
    }
}

float MinArr(float arr[], int n)
{
    int i;
    float min = arr[0];
    for (i = 1; i < n; i++)
    {
        if (arr[i] < min)
        {
            min = arr[i];
        }
        return min;
    }
}

typedef struct _D3DXMATRIX
{
    union
    {
        struct
        {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DXMATRIX;

D3DXMATRIX Matrix(FRotator Rotation, FVector Origin = FVector(0, 0, 0))
{
    float radPitch = (Rotation.Pitch * M_PI / 180.f);
    float radYaw = (Rotation.Yaw * M_PI / 180.f);
    float radRoll = (Rotation.Roll * M_PI / 180.f);
    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);
    D3DXMATRIX matrix;
    matrix.m[0][0] = CP * CY;
    matrix.m[0][1] = CP * SY;
    matrix.m[0][2] = SP;
    matrix.m[0][3] = 0.f;
    matrix.m[1][0] = SR * SP * CY - CR * SY;
    matrix.m[1][1] = SR * SP * SY + CR * CY;
    matrix.m[1][2] = -SR * CP;
    matrix.m[1][3] = 0.f;
    matrix.m[2][0] = -(CR * SP * CY + SR * SY);
    matrix.m[2][1] = CY * SR - CR * SP * SY;
    matrix.m[2][2] = CR * CP;
    matrix.m[2][3] = 0.f;
    matrix.m[3][0] = Origin.X;
    matrix.m[3][1] = Origin.Y;
    matrix.m[3][2] = Origin.Z;
    matrix.m[3][3] = 1.f;
    return matrix;
}

bool W2S(FVector WorldLocation, FVector2D& ScreenLocation)
{
    if (WorldLocation == 0.f) return false;
    auto Location = Cache.LPC->PlayerCameraManager->GetCameraLocation();
    auto Rotation = Cache.LPC->PlayerCameraManager->GetCameraRotation();
    D3DXMATRIX tempMatrix = Matrix(Rotation);
    FVector vAxisX, vAxisY, vAxisZ;
    vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
    vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
    vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
    float w = tempMatrix.m[3][0] * WorldLocation.X + tempMatrix.m[3][1] * WorldLocation.Y + tempMatrix.m[3][2] * WorldLocation.Z + tempMatrix.m[3][3];
    if (w < 0.01) return false;
    FVector vDelta = WorldLocation - Location;
    FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
    if (vTransformed.Z < 1.0f) vTransformed.Z = 1.f;
    float fovAngle = Cache.LPFOV;
    float screenCenterX = Cache.WindowSizeX / 2;
    float screenCenterY = Cache.WindowSizeY / 2;
    ScreenLocation.X = (screenCenterX + vTransformed.X * (screenCenterX / (float)tan(fovAngle * M_PI / 360)) / vTransformed.Z);
    ScreenLocation.Y = (screenCenterY - vTransformed.Y * (screenCenterX / (float)tan(fovAngle * M_PI / 360)) / vTransformed.Z);
    if (ScreenLocation.X < -50 || ScreenLocation.X >(Cache.WindowSizeX + 250)) return false;
    if (ScreenLocation.Y < -50 || ScreenLocation.Y >(Cache.WindowSizeY + 250)) return false;
    return true;
}

void decorations()
{
    pos = ImGui::GetWindowPos();
    draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(ImVec2(pos), ImVec2(pos.x + 650, pos.y + 540), ImColor(18, 11, 18));
    draw->AddRectFilled(ImVec2(pos.x + 7, pos.y + 4), ImVec2(pos.x + 646, pos.y + 541), ImColor(14, 12, 15));
    draw->AddRect(ImVec2(pos.x + 7, pos.y + 4), ImVec2(pos.x + 646, pos.y + 541), ImColor(39, 33, 35));
    draw->AddLine(ImVec2(pos.x + 11, pos.y + 12.5f), ImVec2(pos.x + 11, pos.y + 535), ImColor(42, 42, 44), 1.5f);
    draw->AddRectFilledMultiColor(ImVec2(pos.x + 11, pos.y + 7), ImVec2(pos.x + 643, pos.y + 9), ImColor(109, 84, 90), ImColor(106, 81, 121), ImColor(109, 84, 90), ImColor(106, 81, 121));
    draw->AddRectFilled(ImVec2(pos.x + 102, pos.y + 14), ImVec2(pos.x + 643, pos.y + 535), ImColor(15, 15, 15));
    draw->AddRect(ImVec2(pos.x + 102, pos.y + 14), ImVec2(pos.x + 643, pos.y + 535), ImColor(42, 42, 44));
}

void tabss()
{
    ImGui::SetCursorPos(ImVec2(11, 13));
    ImGui::BeginGroup();

    for (int i = 0; i < sizeof(tab_data) / sizeof(Tab); i++) {
        if (ImGui::tab(tab_data[i].label, tab_data[i].desc, tab_data[i].size, tabs == i)) {
            tabs = i;
            subtabs = i * 2;
        }
    }

    ImGui::EndGroup();
    ImGui::PushFont(zzzz);

    switch (tabs) {
        case 0: {
            ImGui::SetCursorPos(ImVec2(102, 14));
            ImGui::BeginGroup();
            if (ImGui::sub(subtab_data[0], 0 == subtabs))
                subtabs = 0;
            ImGui::SameLine(0.f, 0.f);
            if (ImGui::sub(subtab_data[1], 1 == subtabs))
                subtabs = 1;
            ImGui::EndGroup();

            ImGui::SetCursorPos(ImVec2(125, 34));
            ImGui::BeginChild(subtabs == 0 ? "##HelmetPopper" : "##ExtraFeats", ImVec2(245, 491));
            ImGui::SetCursorPos(ImVec2(5, 10));

            switch (subtabs) {
            case 0: {
                ImGui::Checkbox2("->Enable Aimbot", &Settings.Aimbot.EnableAimbot);
                ImGui::Checkbox2(("     *Target Sleepers"), &Settings.Aimbot.TargetSleepers);
                ImGui::Checkbox2(("     *Target Visible Only"), &Settings.Aimbot.VisibleOnly);
                ImGui::Checkbox2(("     *Ignore Teamates"), &Settings.Aimbot.TargetTribe);
                ImGui::Checkbox2(("     *Silent Aim"), &Settings.Aimbot.SilentAim);
                for (const auto& [label, bone] : bone_data) {
                    if (ImGui::Button(label.c_str())) {
                        AimBone = bone;
                    }
                }
                break;
            }
            case 1: {
                ImGui::Checkbox2(("->Auto Armor"), &Settings.Misc.AutoArmor);
                if (Settings.Misc.AutoArmor)
                {
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::SliderFloat(("Auto Armor %"), &Settings.Misc.AutoArmorPercent, 0.f, 1.f);
                }
                ImGui::Checkbox2(("->Aim FOV"), &Settings.Visuals.DrawAimFOV);
                if (Settings.Visuals.DrawAimFOV)
                {
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::SliderFloat(("Aim FOV Size"), &Settings.Visuals.FOVSize, 0.f, 500.f);
                }
                break;
            }
            }

            ImGui::EndChild();
            break;
        }
        case 1: {
            ImGui::SetCursorPos(ImVec2(102, 14));
            ImGui::BeginGroup();
            if (ImGui::sub(subtab_data[0], 0 == subtabs))
                subtabs = 0;
            ImGui::SameLine(0.f, 0.f);
            if (ImGui::sub(subtab_data[1], 1 == subtabs))
                subtabs = 1;
            ImGui::EndGroup();

            ImGui::SetCursorPos(ImVec2(125, 34));
            ImGui::BeginChild(subtabs == 0 ? "##PlayerESP" : "##ExtraInfo", ImVec2(245, 491));
            ImGui::SetCursorPos(ImVec2(5, 10));

            switch (subtabs) {
            case 0: {
                ImGui::Checkbox2(("->Player ESP"), &Settings.Visuals.DrawPlayers);
                if (Settings.Visuals.DrawPlayers)
                {
                    ImGui::Checkbox2(("     *Show Players Skeletons"), &Settings.Visuals.DrawPlayerBones);
                    ImGui::Checkbox2(("     *Show Sleeping Players"), &Settings.Visuals.DrawSleepingPlayers);
                    ImGui::Checkbox2(("     *Show Dead Players"), &Settings.Visuals.DrawDeadPlayers);
                    ImGui::Checkbox2(("     *Show Player Torpor"), &Settings.Visuals.PlayerTorp);
                    ImGui::Checkbox2(("     *Show Player Distance"), &Settings.Visuals.DrawPlayerDistance);
                    ImGui::Checkbox2(("     *Hide Teamates"), &Settings.Visuals.HideTeamPlayers);
                    ImGui::Checkbox2(("     *Show Player HP"), &Settings.Visuals.DrawPlayerHP);
                    ImGui::Checkbox2(("     *Show Player Names"), &Settings.Visuals.RenderPlayerName);
                }
                break;
            }
            case 1: {
                ImGui::Checkbox2(("->Extra Info"), &Settings.Misc.ExtraInfo);
                if (Settings.Misc.ExtraInfo)
                {
                    ImGui::Checkbox2(("     *Show FPS"), &Settings.Misc.ShowFPS);
                    ImGui::Checkbox2(("     *Connect Players"), &Settings.Misc.ShowPlayers);
                    ImGui::Checkbox2(("     *XYZ"), &Settings.Misc.ShowXYZ);
                    ImGui::Checkbox2(("     *Tamed Noglin Alert"), &Settings.Misc.NoglinAlert);
                }
                break;
            }
            }

            ImGui::EndChild();
            break;
        }
        case 2: {
            ImGui::SetCursorPos(ImVec2(102, 14));
            ImGui::BeginGroup();
            if (ImGui::sub(subtab_data[0], 0 == subtabs))
                subtabs = 0;
            ImGui::SameLine(0.f, 0.f);
            if (ImGui::sub(subtab_data[1], 1 == subtabs))
                subtabs = 1;
            ImGui::EndGroup();

            ImGui::SetCursorPos(ImVec2(125, 34));
            ImGui::BeginChild(subtabs == 0 ? "##DinosESP" : "##OtherDinos", ImVec2(245, 491));
            ImGui::SetCursorPos(ImVec2(5, 10));

            switch (subtabs) {
            case 0: {
                ImGui::Checkbox2(("->Tamed Creatures ESP"), &Settings.Visuals.DrawTamedCreatures);
                if (Settings.Visuals.DrawTamedCreatures)
                {
                    ImGui::Checkbox2(("     *Show Tamed Dino Torpor"), &Settings.Visuals.TamedDinoTorp);
                    ImGui::Checkbox2(("     *Show Dead Dinos"), &Settings.Visuals.DinoDead);
                    ImGui::Checkbox2(("     *Hide Team Dinos"), &Settings.Visuals.HideTeamDinos);
                }
                break;
            }
            case 1: {

                break;
            }
            }

            ImGui::EndChild();
            break;
        }
        case 3: {
            ImGui::SetCursorPos(ImVec2(102, 14));
            ImGui::BeginGroup();
            if (ImGui::sub(subtab_data[0], 0 == subtabs))
                subtabs = 0;
            ImGui::SameLine(0.f, 0.f);
            if (ImGui::sub(subtab_data[1], 1 == subtabs))
                subtabs = 1;
            ImGui::EndGroup();

            ImGui::SetCursorPos(ImVec2(125, 34));
            ImGui::BeginChild(subtabs == 0 ? "##MiscFeats" : "##Colors", ImVec2(245, 491));
            ImGui::SetCursorPos(ImVec2(5, 10));

            switch (subtabs) {
            case 0: {

                break;
            }
            case 1: {

                break;
            }
            }

            ImGui::EndChild();
            break;
        }
        case 4: { 
            ImGui::SetCursorPos(ImVec2(102, 14));
            ImGui::BeginGroup();
            if (ImGui::sub(subtab_data[6], 0 == subtabs))
                subtabs = 0;
            ImGui::SameLine(0.f, 0.f);
            if (ImGui::sub(subtab_data[7], 1 == subtabs))
                subtabs = 1;
            ImGui::EndGroup();

            ImGui::SetCursorPos(ImVec2(125, 34));
            ImGui::BeginChild(subtabs == 0 ? "##Stats" : "##CFG", ImVec2(245, 491));
            ImGui::SetCursorPos(ImVec2(5, 10));

            switch (subtabs) {
            case 0: {

                break;
            }
            case 1: {

                break;
            }
            }

            ImGui::EndChild();
            break;
        }
    }

    ImGui::PopFont();
}

//void tabss()
//{
//    ImGui::SetCursorPos(ImVec2(11, 13));
//    ImGui::BeginGroup();
//
//    if (ImGui::tab("A", "AIMBOT", ImVec2(91, 87), tabs == 0))
//    {
//        tabs = 0;
//        subtabs = 0;
//    }
//    if (ImGui::tab("C", "PLAYERS", ImVec2(91, 87), tabs == 1))
//    {
//        tabs = 1;
//        subtabs = 2;
//    }
//    if (ImGui::tab("D", "VISUALS", ImVec2(91, 87), tabs == 2))
//    {
//        tabs = 2;
//        subtabs = 4;
//    }
//    if (ImGui::tab("E", "MISC", ImVec2(91, 87), tabs == 3))
//    {
//        tabs = 3;
//        subtabs = 6;
//    }
//    if (ImGui::tab("F", "SETTINGS", ImVec2(91, 87), tabs == 4))
//    {
//        tabs = 4;
//        subtabs = 8;
//    }
//
//    ImGui::EndGroup();
//
//    ImGui::PushFont(zzzz);
//
//    if (tabs == 0)
//    {
//        ImGui::SetCursorPos(ImVec2(102, 14));
//        ImGui::BeginGroup();
//        if (ImGui::sub("HELMET POPPER", 0 == subtabs))
//            subtabs = 0;
//        ImGui::SameLine(0.f, 0.f);
//        if (ImGui::sub("EXTRA FEATURES", 1 == subtabs))
//            subtabs = 1;
//        ImGui::EndGroup();
//    }
//
//    if (tabs == 0)
//    {
//        if (subtabs == 0)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1dsfddfdddsda", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Aimbot"), &Settings.Aimbot.EnableAimbot);
//                if (Settings.Aimbot.EnableAimbot)
//                {
//                    ImGui::Checkbox2(("     *Target Sleepers"), &Settings.Aimbot.TargetSleepers);
//                    ImGui::Checkbox2(("     *Target Visible Only"), &Settings.Aimbot.VisibleOnly);
//                    ImGui::Checkbox2(("     *Ignore Teamates"), &Settings.Aimbot.TargetTribe);
//                    ImGui::Checkbox2(("     *Silent Aim"), &Settings.Aimbot.SilentAim);
//                    for (const auto& [label, bone] : bone_data) {
//                        if (ImGui::Button(label.c_str())) {
//                            AimBone = bone;
//                        }
//                    }
//                }
//                
//            }ImGui::EndChild();
//        }
//        else if (subtabs == 1)
//        { 
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1z12522", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Auto Armor"), &Settings.Misc.AutoArmor);
//                if (Settings.Misc.AutoArmor)
//                {
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::SliderFloat(("Auto Armor %"), &Settings.Misc.AutoArmorPercent, 0.f, 1.f);
//                }
//                ImGui::Checkbox2(("->Aim FOV"), &Settings.Visuals.DrawAimFOV);
//                if (Settings.Visuals.DrawAimFOV)
//                {
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::SliderFloat(("Aim FOV Size"), &Settings.Visuals.FOVSize, 0.f, 500.f);
//                }
//            }
//            ImGui::EndChild();
//        }
//    }
//
//    if (tabs == 1)
//    {
//        ImGui::SetCursorPos(ImVec2(102, 14));
//        ImGui::BeginGroup();
//        if (ImGui::sub("PLAYER ESP", 2 == subtabs))
//            subtabs = 2;
//        ImGui::SameLine(0.f, 0.f);
//        if (ImGui::sub("EXTRA INFO", 3 == subtabs))
//            subtabs = 3;
//        ImGui::EndGroup();
//    }
//
//    if (tabs == 1)
//    {
//        if (subtabs == 2)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1shgwdasdawdhda", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Player ESP"), &Settings.Visuals.DrawPlayers);
//                if (Settings.Visuals.DrawPlayers)
//                {
//                    ImGui::Checkbox2(("     *Show Players Skeletons"), &Settings.Visuals.DrawPlayerBones);
//                    ImGui::Checkbox2(("     *Show Sleeping Players"), &Settings.Visuals.DrawSleepingPlayers);
//                    ImGui::Checkbox2(("     *Show Dead Players"), &Settings.Visuals.DrawDeadPlayers);
//                    ImGui::Checkbox2(("     *Show Player Torpor"), &Settings.Visuals.PlayerTorp);
//                    ImGui::Checkbox2(("     *Show Player Distance"), &Settings.Visuals.DrawPlayerDistance);
//                    ImGui::Checkbox2(("     *Hide Teamates"), &Settings.Visuals.HideTeamPlayers);
//                    ImGui::Checkbox2(("     *Show Player HP"), &Settings.Visuals.DrawPlayerHP);
//                    ImGui::Checkbox2(("     *Show Player Names"), &Settings.Visuals.RenderPlayerName);
//                }
//            }
//            ImGui::EndChild();
//        }
//        else if (subtabs == 3)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1sda", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Extra Info"), &Settings.Misc.ExtraInfo);
//                if (Settings.Misc.ExtraInfo)
//                {
//                    ImGui::Checkbox2(("     *Show FPS"), &Settings.Misc.ShowFPS);
//                    ImGui::Checkbox2(("     *Connect Players"), &Settings.Misc.ShowPlayers);
//                    ImGui::Checkbox2(("     *XYZ"), &Settings.Misc.ShowXYZ);
//                    ImGui::Checkbox2(("     *Tamed Noglin Alert"), &Settings.Misc.NoglinAlert);
//                }
//            }
//            ImGui::EndChild();
//        }
//    }
//
//    if (tabs == 2)
//    {
//        ImGui::SetCursorPos(ImVec2(102, 14));
//        ImGui::BeginGroup();
//        if (ImGui::sub("DINOS", 4 == subtabs))
//            subtabs = 4;
//        ImGui::SameLine(0.f, 0.f);
//        if (ImGui::sub("OTHER ESP'S", 5 == subtabs))
//            subtabs = 5;
//        ImGui::EndGroup();
//    }
//
//    if (tabs == 2)
//    {
//        if (subtabs == 4)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1shghda", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Tamed Creatures ESP"), &Settings.Visuals.DrawTamedCreatures);
//                if (Settings.Visuals.DrawTamedCreatures)
//                {
//                    ImGui::Checkbox2(("     *Show Tamed Dino Torpor"), &Settings.Visuals.TamedDinoTorp);
//                    ImGui::Checkbox2(("     *Show Dead Dinos"), &Settings.Visuals.DinoDead);
//                    ImGui::Checkbox2(("     *Hide Team Dinos"), &Settings.Visuals.HideTeamDinos);
//                }
//            }
//            ImGui::EndChild();
//            ImGui::SameLine(0.f, 7.f);
//            ImGui::BeginChild("##whfgsdda1", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Wild Creatures ESP"), &Settings.Visuals.DrawWildCreatures);
//                if (Settings.Visuals.DrawWildCreatures)
//                {
//                    ImGui::Checkbox2(("     *Show Wild Torpor"), &Settings.Visuals.WildDinoTorp);
//                    ImGui::Checkbox2(("     *Alpha Dino Filter"), &Settings.Visuals.AlphaFilter);
//                    ImGui::Checkbox2(("     *Wild Dino Filter"), &Settings.Visuals.WildDinoFilter);
//
//                    static const char* Dinoitems[] = { "Achatina", "Allo", "Ammonite", "Angler", "Anky", "Titanomyrma", "Archaeopteryx", "Argentavis", "Arthro", "Astrodelphis", "Baryonyx", "Basilisk", "Basilosaurus", "Onychonycteris", "Beaver", "Dung Beetle", "Gigantopithecus", "Bloodstalker", "Noglin", "Bronto", "Morellatops", "Carno", "Karkinos", "Ravager", "Chalicotherium", "Cnidaria", "Compy", "Daeodon", "Deathworm", "Reaper", "Deinonychus", "Desert Titan", "Dilo", "Dimetrodon", "Dimorphodon", "Diplodocus", "Diplocaulus", "Direbear", "Direwolf", "Dodo", "Doedicurus", "Ichthyosaurus", "Dragon Boss", "Meganeura", "Dunkleosteus", "Electrophorus", "Alpha Carno", "Alpha Raptor", "Alpha Rex", "Alpha Megladon", "Enforcer", "Equus", "Eurypterid", "Desert Titan Flock", "Forest Titan", "Gacha", "Gallimimus", "GasBags", "Megachelon", "Giganotosaurus", "Ferox", "Griffin", "Hesperornis", "Hyaenodon", "Ice Titan", "Ichthyornis", "Iguanodon", "Jerboa", "Jugbug", "Kairu", "Kangaroo", "Kaprosuchus", "Kentro", "Lamprey", "Featherlight", "Shinehorn", "Glowtail", "Magmasaur", "Leech", "Leedsichthys", "Shadowmane", "Liopleurodon", "Lystrosaurus", "Maewing", "Mammoth", "Managarmr", "Manta", "Mantis", "Megalania", "Megalosaurus", "Megapithecus", "Megatherium", "Microraptor", "MoleRat", "Mesopithecus", "Mosasaur", "Megalodon", "Moschops", "Oviraptor", "Otter", "Oviraptor", "Snow Owl", "Pachy", "Parasaurolophus", "Paracer", "Pegomastax", "Pelagornis", "Phiomia", "Phoenix", "Piranha", "Plesiosaur", "Pteranodon", "Bulbdog", "Purlovia", "Quetz", "Raptor", "Rex", "Rhino", "RockDrake", "RockElemental", "Sabertooth", "Sarco", "Scorpion", "Scout", "Ovis", "Astrocetus", "Araneo", "Thorny Dragon", "Spino", "Megaloceros", "Stego", "Summoner", "Tapejara", "TekStrider", "TekWyvern", "TerrorBird", "Therizinosaurus", "Thylacoleo", "Titan", "Titanboa", "Beelzebufo", "Trike", "Troodon", "Tropeognathus", "Turtle", "Tusoteuthis", "Velonasaur", "Vulture", "Wyvern", "Rockwell", "Yutyrannus" };
//                    static const char* Dinocurrent_item = "Achatina";
//
//                    if (ImGui::BeginCombo("##combodino", Dinocurrent_item))
//                    {
//                        for (int z = 0; z < IM_ARRAYSIZE(Dinoitems); z++)
//                        {
//                            bool is_selected = (Dinocurrent_item == Dinoitems[z]);
//                            if (ImGui::Selectable(Dinoitems[z], is_selected))
//                                Dinocurrent_item = Dinoitems[z];
//                            if (is_selected)
//                                ImGui::SetItemDefaultFocus();
//                        }
//                        ImGui::EndCombo();
//                    }
//
//                    for (int z = 0; z < 151; z++)
//                    {
//                        if (Dinocurrent_item == Dinoitems[z]) { PossibleDinos = z; }
//                    }
//                }
//            }
//            ImGui::EndChild();
//        }
//        else if (subtabs == 5)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1sda", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Container ESP"), &Settings.Visuals.DrawContainers);
//                if (Settings.Visuals.DrawContainers)
//                {
//                    ImGui::Checkbox2(("     *Empty Containers"), &Settings.Visuals.ShowEmptyContainers);
//                    ImGui::Checkbox2(("     *Containers Filter"), &Settings.Visuals.ContainerFilter);
//                    static const char* ContainerFilter[] = { "Supply Crate", "Bee Hive", "Artifact Container", "Loot Crate", "Tribute Terminal", "Tek Teleporter", "Tek Generator", "Bed", "Bunk Bed", "Sleeping Bag", "Tek Sleeping Pod", "Tek Replicator", "Tek Cloning Chamber", "Tek Trough", "Tek Forcefield", "Dedicated Storage", "Canoe", "Placed Taxidermy Base", "Storage Box", "Mortar and Pestle", "Cooking Pot", "Fireplace", "Loadout Manneqiun", "Smithy", "Refining Forge", "Bookshelf", "Preserving Bin", "Large Storage Box", "Beer Barrel", "Toilet", "Catapult Turret", "Cryofridge", "Minigun Turret", "Rocket-Launcher Turret", "Ammo Box", "Fabricator", "Power Generator", "Industrial Grill", "Vault", "Chemistry Bench", "Egg Incubator", "Industrial Cooker", "Item Cache", "Tek Sensor", "Vacuum Compartment Moonpool", "Vacuum Compartment", "Vessel", "Wardrums", "Water Well", "Delivery Crate", "Water Tank", "Wood Elevator Track", "Oil Pump", "Air Conditioner", "Tek Bridge", "Tek Jump Pad", "Tek Surveillance Console", "Elevator Track" };
//                    static const char* Container_Current = "Supply Crate";
//
//                    if (ImGui::BeginCombo("##comboddsidssno", Container_Current))
//                    {
//                        for (int h = 0; h < IM_ARRAYSIZE(ContainerFilter); h++)
//                        {
//                            bool is_selected = (Container_Current == ContainerFilter[h]);
//                            if (ImGui::Selectable(ContainerFilter[h], is_selected))
//                                Container_Current = ContainerFilter[h];
//                            if (is_selected)
//                                ImGui::SetItemDefaultFocus();
//                        }
//                        ImGui::EndCombo();
//                    }
//
//                    for (int h = 0; h < 151; h++)
//                    {
//                        if (Container_Current == ContainerFilter[h]) { CurrentContainer = ContainerFilter[h]; }
//                    }
//                }
//                ImGui::Checkbox2(("->Turret ESP"), &Settings.Visuals.DrawTurrets);
//                if (Settings.Visuals.DrawTurrets)
//                {
//                    ImGui::Checkbox2(("     *Bullet Count"), &Settings.Visuals.ShowBulletCount);
//                    ImGui::Checkbox2(("     *Empty Turrets"), &Settings.Visuals.ShowEmptyTurrets);
//                }
//            }
//            ImGui::EndChild();
//        }
//    }
//
//    if (tabs == 3)
//    {
//        ImGui::SetCursorPos(ImVec2(102, 14));
//        ImGui::BeginGroup();
//        if (ImGui::sub("MISC FEATURES", 6 == subtabs))
//            subtabs = 6;
//        ImGui::SameLine(0.f, 0.f);
//        if (ImGui::sub("COLORS", 7 == subtabs))
//            subtabs = 7;
//        ImGui::EndGroup();
//    }
//
//    if (tabs == 3)
//    {
//        if (subtabs == 6)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##wddfsaw", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->Crosshair"), &Settings.Visuals.DrawCrosshair);
//                if (Settings.Visuals.DrawCrosshair)
//                {
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::SliderFloat(("Crosshair Size"), &Settings.Visuals.CrosshairSize, 0.f, 30.f);
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::SliderFloat(("Crosshair Thikness"), &Settings.Visuals.CrosshairWidth, 0.f, 15.f);
//                }
//            }
//            ImGui::EndChild();
//            ImGui::SameLine(0.f, 7.f);
//            ImGui::BeginChild("##wdfw23231232da1", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Checkbox2(("->No Spread"), &Settings.Misc.NoSpread);
//                ImGui::Checkbox2(("->No Sway"), &Settings.Misc.NoSway);
//                ImGui::Checkbox2(("->No Shake"), &Settings.Misc.NoShake);
//                ImGui::Checkbox2(("->Infinite Orbit"), &Settings.Misc.InfiniteOrbit);
//                ImGui::Checkbox2(("->Insta Turn"), &Settings.Misc.InstantDinoTurn);
//                ImGui::Checkbox2(("->Long Arms"), &Settings.Misc.LongArms);
//                if (Settings.Misc.LongArms)
//                {
//                    ImGui::Checkbox2(("     *Infinite Arms"), &Settings.Misc.InfiniteArms);
//                }
//                ImGui::Checkbox2(("->Speed"), &Settings.Misc.SpeedHacks);
//                if (Settings.Misc.SpeedHacks)
//                {
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::Spacing();
//                    ImGui::SliderFloat(("Speed Hacks"), &Settings.Misc.NewSpeed, 0.f, 15.f);
//                }
//                ImGui::Checkbox2(("->Rapid Fire"), &Settings.Misc.RapidFire);
//                
//            }
//            ImGui::EndChild();
//        }
//        else if (subtabs == 7)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##wddf3423422saw", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::ColorEdit3("->Player ESP Color", PlayerColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Tamed Dino ESP Color", TamedDinoColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Wild Dino ESP Color", WildDinoColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Wild Dino Filter ESP Color", FilteredDinoColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Turret ESP Color", TurretColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Container ESP Color", ContainerColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Crosshair Color", CrosshairColor1, ImGuiColorEditFlags_NoInputs);
//                ImGui::ColorEdit3("->Crosshair FOV Color", CrosshairColorFOV1, ImGuiColorEditFlags_NoInputs);
//            }
//            ImGui::EndChild();
//        }
//    }
//
//    if (tabs == 4)
//    {
//        ImGui::SetCursorPos(ImVec2(102, 14));
//        ImGui::BeginGroup();
//        if (ImGui::sub("STATS", 8 == subtabs))
//            subtabs = 8;
//        ImGui::SameLine(0.f, 0.f);
//        if (ImGui::sub("BINDS & CONFIGS", 9 == subtabs))
//            subtabs = 9;
//        ImGui::EndGroup();
//    }
//
//    if (tabs == 4)
//    {
//        ImGuiIO io = ImGui::GetIO();
//        if (subtabs == 8)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1sgfgdda", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Text("Ching Chong Hook | V1 Beta");
//                ImGui::Spacing();
//                ImGui::Spacing();
//                ImGui::Separator();
//                ImGui::Spacing();
//                ImGui::Text("Last Update Was %s", __DATE__);
//                ImGui::Spacing();
//                ImGui::Text("Frame Time: %.3f ms", 1000 / io.Framerate);
//                ImGui::Text("%d Vertices, %d Indices", io.MetricsRenderVertices, io.MetricsRenderIndices);
//                ImGui::Text("%d Active Windows (%d Visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
//                ImGui::Text("%d Active Allocations", io.MetricsActiveAllocations);
//                ImGui::Text("Rendering at %.3f FPS", io.Framerate);
//            }
//            ImGui::EndChild();
//            ImGui::SameLine(0.f, 7.f);
//            ImGui::BeginChild("##wdfweda1", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Text("[!] Updates:");
//                ImGui::Spacing();
//                ImGui::Spacing();
//                ImGui::Separator();
//                ImGui::Text("[+] New Menu");
//                ImGui::Text("[-] Back end editing");
//                ImGui::Text("[-] Removed 'God' Bow");
//            }
//            ImGui::EndChild();
//        }
//        else if (subtabs == 9)
//        {
//            ImGui::SetCursorPos(ImVec2(125, 34));
//            ImGui::BeginChild("##1wdsaa", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Text("Aimbot Key");
//
//                const char* Items2[] = { FKeyNames.Delete, FKeyNames.Insert, FKeyNames.LeftClick, FKeyNames.RightClick, FKeyNames.MouseButton1, FKeyNames.MouseButton2, FKeyNames.B, FKeyNames.C, FKeyNames.E, FKeyNames.F, FKeyNames.G, FKeyNames.H, FKeyNames.I, FKeyNames.J, FKeyNames.K, FKeyNames.L, FKeyNames.M, FKeyNames.N,
// FKeyNames.O, FKeyNames.P, FKeyNames.Q, FKeyNames.T, FKeyNames.U, FKeyNames.V, FKeyNames.X, FKeyNames.Y, FKeyNames.Z, FKeyNames.Up, FKeyNames.Down, FKeyNames.Left, FKeyNames.Right }; // 28
//                static int Item2 = 5;
//                const char* Preview2 = Items2[Item2];
//                if (ImGui::BeginCombo("##Aim Bone Key", Preview2))
//                {
//                    for (int i = 0; i < IM_ARRAYSIZE(Items2); i++)
//                    {
//                        const bool IsSelected = (Item2 == i);
//                        if (ImGui::Selectable(Items2[i], &IsSelected)) Item2 = i;
//                        if (IsSelected) ImGui::SetItemDefaultFocus();
//                    }
//                    ImGui::EndCombo();
//                }
//                for (int i = 0; i < 32; i++)
//                {
//                    if (Preview2 == Items2[i]) { Settings.Keybinds.AimbotKey = Items2[i]; }
//                }
//            }
//            ImGui::EndChild();
//            ImGui::SameLine(0.f, 7.f);
//            ImGui::BeginChild("##1sdxz", ImVec2(245, 491));
//            {
//                ImGui::SetCursorPos(ImVec2(5, 10));
//                ImGui::Text("Configs Coming Soon!");
//            }
//            ImGui::EndChild();
//        }
//    }
//
//    ImGui::PopFont();
//}