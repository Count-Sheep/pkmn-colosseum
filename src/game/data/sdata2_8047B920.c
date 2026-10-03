#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Name-entry, worldmap and shop .sdata2 constants after gs_title_80023DA8.c's
 * pool (split from sdata2_8047B8A0.c at 0x8047B920): the list-id table at
 * B920, packed route-id words at B928/B92C, and the map animation/phase
 * constants.
 */
SDATA2 const u32 lbl_8047B920[2] = { 0, 0x00002EF7u };
SDATA2 const u32 lbl_8047B928 = 0x040A040Bu;
SDATA2 const u32 lbl_8047B92C = 0x040B040Cu;
SDATA2 const f32 lbl_8047B930 = 0.0f;
SDATA2 const f32 lbl_8047B934 = 1.0f;
SDATA2 const f32 lbl_8047B938 = 255.0f;
SDATA2 const f32 lbl_8047B93C = 0.25f;
SDATA2 const f32 lbl_8047B940 = 0.5f;
SDATA2 const f64 lbl_8047B948 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047B950 = 0.033333335f;
SDATA2 const f32 lbl_8047B954 = 128.0f;
SDATA2 const f32 lbl_8047B958 = 0.1f;
SDATA2 const f32 lbl_8047B95C = 640.0f;
SDATA2 const f32 lbl_8047B960 = 480.0f;
SDATA2 const f32 lbl_8047B964 = 0.001f;
SDATA2 const f32 lbl_8047B968 = 1.999f;
SDATA2 const f32 lbl_8047B96C = 120.0f;
SDATA2 const f32 lbl_8047B970[2] = { 60.0f, 0.0f };
SDATA2 const f32 lbl_8047B978 = 255.0f;
SDATA2 const f32 lbl_8047B97C = 1.0f;
SDATA2 const f32 lbl_8047B980 = 0.0f;
SDATA2 const f32 lbl_8047B984 = 0.0506708547f;
SDATA2 const f32 lbl_8047B988 = 6.28318548f;
SDATA2 const f32 lbl_8047B98C = 3.14159274f;
SDATA2 const f32 lbl_8047B990 = -3.14159274f;
SDATA2 const f64 lbl_8047B998 = 4.503601774854144e+15;
