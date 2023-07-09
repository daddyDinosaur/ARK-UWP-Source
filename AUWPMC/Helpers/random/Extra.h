#pragma once
#include "Cheat.h"
#include "Helpers/Logger/Logger.h"
#include <d3d11.h>
#include "Helpers/imgui\imgui.h"
#include "Helpers/imgui\imgui_impl_win32.h"
#include "Helpers/imgui\imgui_impl_dx11.h"
#include "Helpers/imgui\imgui_internal.h"

float CachedFramerates[1000];
int AimBone = 8; // Default Head
int PossibleDinos = 1;

static int tabs;
static int subtabs;

ImFont* zzzz = nullptr;
ImVec2 pos;
ImDrawList* draw;


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