#pragma once

// World-to-screen helpers (same offsets as TrueHUD)
static uintptr_t g_worldToCamMatrix = RELOCATION_ID(519579, 406126).address();
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)RELOCATION_ID(519618, 406160).address();
static float* g_fNear = (float*)(RELOCATION_ID(517032, 403540).address() + 0x40);
static float* g_fFar = (float*)(RELOCATION_ID(517032, 403540).address() + 0x44);

