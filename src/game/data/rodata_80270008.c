#include "dolphin/types.h"

extern u8 HSD_AObjGetAllocData[];
extern u8 HSD_ChanGetAllocData[];
extern u8 HSD_DListGetAllocData[];
extern u8 HSD_FObjGetAllocData[];
extern u8 HSD_IDGetAllocData[];
extern u8 HSD_MtxGetAllocData[];
extern u8 HSD_RObjGetAllocData[];
extern u8 HSD_RenderGetAllocData[];
extern u8 HSD_RvalueObjGetAllocData[];
extern u8 HSD_SListGetAllocData[];
extern u8 HSD_ShadowGetAllocData[];
extern u8 HSD_TevRegGetAllocData[];
extern u8 HSD_VecGetAllocData[];
extern u8 WS_ABARERU[];
extern u8 WS_AKUBI[];
extern u8 WS_ALERTEND[];
extern u8 WS_CHECK_TYPE[];
extern u8 WS_CHIISAKUNARU[];
extern u8 WS_CHOUHATSU[];
extern u8 WS_CONDITION_CHECK[];
extern u8 WS_CRITICAL_CHECK[];
extern u8 WS_DAMAGE_LOSS[];
extern u8 WS_DAMAGE_LOSS_ONLY[];
extern u8 WS_GETEND[];
extern u8 WS_HIMITUNOTIKARA[];
extern u8 WS_HITCHECK[];
extern u8 WS_ICHAMON[];
extern u8 WS_ITEMEND[];
extern u8 WS_JIKOANJI[];
extern u8 WS_JUUDEN[];
extern u8 WS_KIERUTAME[];
extern u8 WS_KIERUTAME_AFTAR[];
extern u8 WS_KORAERU_CHECK[];
extern u8 WS_KUROIKIRI[];
extern u8 WS_MARUKUNARU[];
extern u8 WS_MICHIDURE[];
extern u8 WS_MIYABURU[];
extern u8 WS_NARIKIRI[];
extern u8 WS_NEKODAMASHI[];
extern u8 WS_NEWOHARU[];
extern u8 WS_ONNEN[];
extern u8 WS_SCA_END_SET[];
extern u8 WS_SEQEND[];
extern u8 WS_SEQRET[];
extern u8 WS_SIDECONDITION_CHECK[];
extern u8 WS_SPEABIEND[];
extern u8 WS_STATUS_GET16[];
extern u8 WS_STATUS_GET32[];
extern u8 WS_STATUS_GET8[];
extern u8 WS_STATUS_SET16[];
extern u8 WS_STATUS_SET32[];
extern u8 WS_STATUS_SET8[];
extern u8 WS_SWITCH_A_D[];
extern u8 WS_TSUIKA_DIRECT_ACT[];
extern u8 WS_TSUIKA_INDIRECT_ACT[];
extern u8 WS_TYPE_CHECK[];
extern u8 WS_WAZAEND[];
extern u8 WS_WAZAKOUKA_CHECK[];
extern u8 WS_WAZAOBOE_CHECK[];
extern u8 WS_WEATHER_CHANGE[];
extern u8 _HSD_AObjForgetMemory[];
extern u8 _HSD_DispForgetMemory[];
extern u8 _HSD_IDForgetMemory[];
extern u8 _HSD_ObjAllocForgetMemory[];
extern u8 _HSD_RObjForgetMemory[];
extern u8 _HSD_RandForgetMemory[];
extern u8 _dbgMenuFightGetWazaDataIdSub[];
extern u8 fn_80005FFC[];
extern u8 dbgMenuFightStop[];
extern u8 fn_800096B4[];
extern u8 fn_80111DF8[];
extern u8 fn_80111F2C[];
extern u8 fn_8011207C[];
extern u8 fn_80118068[];
extern u8 fn_80118070[];
extern u8 fn_80118100[];
extern u8 _cameraGetStateSize[];
extern u8 _cameraMakeStateData[];
extern u8 _cameraRestoreStateData[];
extern u8 fn_80213A28[];
extern u8 fn_80213A38[];
extern u8 fn_80213A48[];
extern u8 fn_80213A58[];
extern u8 fn_80213A68[];
extern u8 fn_80213A78[];
extern u8 fn_80213E94[];
extern u8 fn_80214450[];
extern u8 fn_802145C8[];
extern u8 fn_802146B4[];
extern u8 fn_802146C4[];
extern u8 fn_80214794[];
extern u8 fn_80214864[];
extern u8 fn_802149B8[];
extern u8 fn_80214AB4[];
extern u8 fn_80214AFC[];
extern u8 fn_80214B58[];
extern u8 fn_80214BB4[];
extern u8 fn_80214C04[];
extern u8 fn_80214CB0[];
extern u8 fn_80214DB0[];
extern u8 fn_80214E50[];
extern u8 fn_80214F10[];
extern u8 fn_802151C0[];
extern u8 fn_80215374[];
extern u8 fn_80215614[];
extern u8 fn_80215720[];
extern u8 fn_80215808[];
extern u8 fn_80215954[];
extern u8 fn_80215AEC[];
extern u8 fn_80215CF0[];
extern u8 fn_802160EC[];
extern u8 fn_80216264[];
extern u8 fn_80216364[];
extern u8 fn_80216410[];
extern u8 fn_802165B4[];
extern u8 fn_80216650[];
extern u8 fn_80216780[];
extern u8 fn_80216A58[];
extern u8 fn_80216D9C[];
extern u8 fn_80216F50[];
extern u8 fn_80217018[];
extern u8 fn_802170B4[];
extern u8 fn_80217220[];
extern u8 fn_80217434[];
extern u8 fn_80217524[];
extern u8 fn_802175A8[];
extern u8 fn_802177E4[];
extern u8 fn_802178F4[];
extern u8 fn_8021799C[];
extern u8 fn_80217AE4[];
extern u8 fn_80217C04[];
extern u8 fn_80217D34[];
extern u8 fn_80217E20[];
extern u8 fn_80218018[];
extern u8 fn_802182D4[];
extern u8 fn_802183BC[];
extern u8 fn_8021847C[];
extern u8 fn_80218824[];
extern u8 fn_80218A6C[];
extern u8 fn_80218BD4[];
extern u8 fn_80218D24[];
extern u8 fn_8021908C[];
extern u8 fn_802192B4[];
extern u8 fn_80219354[];
extern u8 fn_802195A0[];
extern u8 fn_802196A8[];
extern u8 fn_80219838[];
extern u8 fn_80219964[];
extern u8 fn_80219B2C[];
extern u8 fn_80219CF4[];
extern u8 fn_80219D98[];
extern u8 fn_80219E10[];
extern u8 fn_8021A054[];
extern u8 fn_8021A338[];
extern u8 fn_8021A478[];
extern u8 fn_8021A6CC[];
extern u8 fn_8021A764[];
extern u8 fn_8021A80C[];
extern u8 fn_8021A878[];
extern u8 fn_8021A984[];
extern u8 fn_8021AB9C[];
extern u8 fn_8021AC1C[];
extern u8 fn_8021AFAC[];
extern u8 fn_8021B0B0[];
extern u8 fn_8021B1A4[];
extern u8 fn_8021B484[];
extern u8 fn_8021B610[];
extern u8 fn_8021B628[];
extern u8 fn_8021B760[];
extern u8 fn_8021B8B8[];
extern u8 fn_8021C0F4[];
extern u8 fn_8021C190[];
extern u8 fn_8021C308[];
extern u8 fn_8021C490[];
extern u8 fn_8021C588[];
extern u8 fn_8021C6F4[];
extern u8 fn_8021C75C[];
extern u8 fn_8021C900[];
extern u8 fn_8021CA00[];
extern u8 fn_8021CB58[];
extern u8 fn_8021CC5C[];
extern u8 fn_8021CCE0[];
extern u8 fn_8021CE60[];
extern u8 fn_8021CF3C[];
extern u8 fn_8021D010[];
extern u8 fn_8021D090[];
extern u8 fn_8021D224[];
extern u8 fn_8021D40C[];
extern u8 fn_8021D688[];
extern u8 fn_8021D9C0[];
extern u8 fn_8021DB78[];
extern u8 fn_8021DD24[];
extern u8 fn_8021DD34[];
extern u8 fn_8021DD44[];
extern u8 fn_8021DDB8[];
extern u8 fn_8021DDC8[];
extern u8 fn_8021DDD8[];
extern u8 fn_8021DE3C[];
extern u8 fn_8021DE4C[];
extern u8 fn_8021DF70[];
extern u8 fn_8021DF80[];
extern u8 fn_8021E04C[];
extern u8 fn_8021E288[];
extern u8 fn_8021E600[];
extern u8 fn_8021E6CC[];
extern u8 fn_8021E6DC[];
extern u8 fn_8021E6EC[];
extern u8 fn_8021E744[];
extern u8 fn_8021E754[];
extern u8 fn_8021E9F4[];
extern u8 fn_8021EA94[];
extern u8 fn_8021EAE8[];
extern u8 fn_8021ECF8[];
extern u8 fn_8021ED70[];
extern u8 fn_8021EE38[];
extern u8 fn_8021EE48[];
extern u8 fn_8021EE98[];
extern u8 fn_8021EED4[];
extern u8 fn_8021EF14[];
extern u8 fn_8021EF24[];
extern u8 fn_8021F1CC[];
extern u8 fn_8021F24C[];
extern u8 fn_8021F39C[];
extern u8 fn_8021F458[];
extern u8 fn_8021F664[];
extern u8 fn_8021F92C[];
extern u8 fn_8021F998[];
extern u8 fn_8021FAD4[];
extern u8 fn_80220868[];
extern u8 fn_8022106C[];
extern u8 fn_802222F4[];
extern u8 fn_80222370[];
extern u8 fn_802223E0[];
extern u8 fn_80222438[];
extern u8 fn_80222494[];
extern u8 fn_80222500[];
extern u8 fn_80222510[];
extern u8 fn_80222520[];
extern u8 fn_80222554[];
extern u8 fn_80222584[];
extern u8 fn_802225B0[];
extern u8 fn_802225DC[];
extern u8 fn_80222604[];
extern u8 fn_8022262C[];
extern u8 fn_80222654[];
extern u8 fn_802226A4[];
extern u8 fn_802226EC[];
extern u8 fn_80222714[];
extern u8 fn_8022273C[];
extern u8 fn_8022275C[];
extern u8 fn_802227D4[];
extern u8 fn_80222844[];
extern u8 fn_8022290C[];
extern u8 fn_802229EC[];
extern u8 fn_80222ACC[];
extern u8 fn_80222ADC[];
extern u8 fn_80222B7C[];
extern u8 fn_80222BD8[];
extern u8 fn_80222C44[];
extern u8 fn_802232F4[];
extern u8 fn_80223AF4[];
extern u8 fn_80223CE8[];
extern u8 fn_80223D64[];
extern u8 fn_80223F1C[];
extern u8 fn_80224060[];
extern u8 fn_80224158[];
extern u8 fn_80224740[];
extern u8 fn_80224820[];
extern u8 fn_80226134[];
extern u8 fn_802261B0[];
extern u8 fn_8022622C[];
extern u8 fn_80226284[];
extern u8 fn_802262D0[];
extern u8 fn_8022631C[];
extern u8 fn_802266EC[];
extern u8 fn_80226730[];
extern u8 fn_802267E8[];
extern u8 fn_80226914[];
extern u8 fn_80226F0C[];
extern u8 fn_80226FD4[];
extern u8 fn_80227490[];
extern u8 fn_80227C40[];
extern u8 fn_8022808C[];
extern u8 fn_802282D8[];
extern u8 fn_802284B0[];
extern u8 fn_80229C90[];
extern u8 fightGSfloorGetPushDataSize[];
extern u8 fightGSfloorPushData[];
extern u8 fightGSfloorPopData[];
extern u8 lbl_80004000[];
extern u8 lbl_8047DAB0[];
extern u8 lbl_8047DAB8[];
extern u8 lbl_8047DAC0[];
extern u8 lbl_8047DAC4[];
extern u8 lbl_8047DACC[];
extern u8 lbl_8047DAD4[];
extern u8 lbl_8047DAD8[];
extern u8 lbl_8047DADC[];
extern u8 lbl_8047DAE4[];
extern u8 lbl_8047DAEC[];
extern u8 lbl_8047DAF4[];
extern u8 lbl_8047DAFC[];
extern u8 lbl_8047DB04[];
extern u8 peopleBiosGetPushDataSize[];
extern u8 peopleBiosPopData[];
extern u8 peopleBiosPushData[];

/* Auto-carved .rodata unit, now 0x80270008..0x802704A0 (the original carve ran to 0x8027A4F0; gs_log.cpp owns 0x802704A0..0x80270528 and rodata_80270528.c continues up to fobj.c at 0x80274758). Non-relocated data emitted byte-exact as const u8[]; pointer/jump tables as const void*[] for R_PPC_ADDR32 relocations. */

/* 0x80270008..0x80270038 (lbl_80270008) is crt/math_range_800CB2B4.c's own
 * .rodata; this unit starts at 0x80270038. */
const u8 lbl_80270038[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const u8 lbl_80270048[48] = {
    0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xE2, 0xB8, 0x03, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x4C, 0xFD, 0xEB, 0x43, 0xCF, 0xD0, 0x06,
};

const u8 lbl_80270078[264] = {
    0x00, 0xA2, 0xF9, 0x83, 0x00, 0x6E, 0x4E, 0x44, 0x00, 0x15, 0x29, 0xFC, 0x00, 0x27, 0x57, 0xD1,
    0x00, 0xF5, 0x34, 0xDD, 0x00, 0xC0, 0xDB, 0x62, 0x00, 0x95, 0x99, 0x3C, 0x00, 0x43, 0x90, 0x41,
    0x00, 0xFE, 0x51, 0x63, 0x00, 0xAB, 0xDE, 0xBB, 0x00, 0xC5, 0x61, 0xB7, 0x00, 0x24, 0x6E, 0x3A,
    0x00, 0x42, 0x4D, 0xD2, 0x00, 0xE0, 0x06, 0x49, 0x00, 0x2E, 0xEA, 0x09, 0x00, 0xD1, 0x92, 0x1C,
    0x00, 0xFE, 0x1D, 0xEB, 0x00, 0x1C, 0xB1, 0x29, 0x00, 0xA7, 0x3E, 0xE8, 0x00, 0x82, 0x35, 0xF5,
    0x00, 0x2E, 0xBB, 0x44, 0x00, 0x84, 0xE9, 0x9C, 0x00, 0x70, 0x26, 0xB4, 0x00, 0x5F, 0x7E, 0x41,
    0x00, 0x39, 0x91, 0xD6, 0x00, 0x39, 0x83, 0x53, 0x00, 0x39, 0xF4, 0x9C, 0x00, 0x84, 0x5F, 0x8B,
    0x00, 0xBD, 0xF9, 0x28, 0x00, 0x3B, 0x1F, 0xF8, 0x00, 0x97, 0xFF, 0xDE, 0x00, 0x05, 0x98, 0x0F,
    0x00, 0xEF, 0x2F, 0x11, 0x00, 0x8B, 0x5A, 0x0A, 0x00, 0x6D, 0x1F, 0x6D, 0x00, 0x36, 0x7E, 0xCF,
    0x00, 0x27, 0xCB, 0x09, 0x00, 0xB7, 0x4F, 0x46, 0x00, 0x3F, 0x66, 0x9E, 0x00, 0x5F, 0xEA, 0x2D,
    0x00, 0x75, 0x27, 0xBA, 0x00, 0xC7, 0xEB, 0xE5, 0x00, 0xF1, 0x7B, 0x3D, 0x00, 0x07, 0x39, 0xF7,
    0x00, 0x8A, 0x52, 0x92, 0x00, 0xEA, 0x6B, 0xFB, 0x00, 0x5F, 0xB1, 0x1F, 0x00, 0x8D, 0x5D, 0x08,
    0x00, 0x56, 0x03, 0x30, 0x00, 0x46, 0xFC, 0x7B, 0x00, 0x6B, 0xAB, 0xF0, 0x00, 0xCF, 0xBC, 0x20,
    0x00, 0x9A, 0xF4, 0x36, 0x00, 0x1D, 0xA9, 0xE3, 0x00, 0x91, 0x61, 0x5E, 0x00, 0xE6, 0x1B, 0x08,
    0x00, 0x65, 0x99, 0x85, 0x00, 0x5F, 0x14, 0xA0, 0x00, 0x68, 0x40, 0x8D, 0x00, 0xFF, 0xD8, 0x80,
    0x00, 0x4D, 0x73, 0x27, 0x00, 0x31, 0x06, 0x06, 0x00, 0x15, 0x56, 0xCA, 0x00, 0x73, 0xA8, 0xC9,
    0x00, 0x60, 0xE2, 0x7B, 0x00, 0xC0, 0x8C, 0x6B,
};

const u8 lbl_80270180[128] = {
    0x3F, 0xF9, 0x21, 0xFB, 0x40, 0x09, 0x21, 0xFB, 0x40, 0x12, 0xD9, 0x7C, 0x40, 0x19, 0x21, 0xFB,
    0x40, 0x1F, 0x6A, 0x7A, 0x40, 0x22, 0xD9, 0x7C, 0x40, 0x25, 0xFD, 0xBB, 0x40, 0x29, 0x21, 0xFB,
    0x40, 0x2C, 0x46, 0x3A, 0x40, 0x2F, 0x6A, 0x7A, 0x40, 0x31, 0x47, 0x5C, 0x40, 0x32, 0xD9, 0x7C,
    0x40, 0x34, 0x6B, 0x9C, 0x40, 0x35, 0xFD, 0xBB, 0x40, 0x37, 0x8F, 0xDB, 0x40, 0x39, 0x21, 0xFB,
    0x40, 0x3A, 0xB4, 0x1B, 0x40, 0x3C, 0x46, 0x3A, 0x40, 0x3D, 0xD8, 0x5A, 0x40, 0x3F, 0x6A, 0x7A,
    0x40, 0x40, 0x7E, 0x4C, 0x40, 0x41, 0x47, 0x5C, 0x40, 0x42, 0x10, 0x6C, 0x40, 0x42, 0xD9, 0x7C,
    0x40, 0x43, 0xA2, 0x8C, 0x40, 0x44, 0x6B, 0x9C, 0x40, 0x45, 0x34, 0xAC, 0x40, 0x45, 0xFD, 0xBB,
    0x40, 0x46, 0xC6, 0xCB, 0x40, 0x47, 0x8F, 0xDB, 0x40, 0x48, 0x58, 0xEB, 0x40, 0x49, 0x21, 0xFB,
};

const u8 lbl_80270200[16] = {
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06,
};

const u8 lbl_80270210[64] = {
    0x3F, 0xF9, 0x21, 0xFB, 0x40, 0x00, 0x00, 0x00, 0x3E, 0x74, 0x44, 0x2D, 0x00, 0x00, 0x00, 0x00,
    0x3C, 0xF8, 0x46, 0x98, 0x80, 0x00, 0x00, 0x00, 0x3B, 0x78, 0xCC, 0x51, 0x60, 0x00, 0x00, 0x00,
    0x39, 0xF0, 0x1B, 0x83, 0x80, 0x00, 0x00, 0x00, 0x38, 0x7A, 0x25, 0x20, 0x40, 0x00, 0x00, 0x00,
    0x36, 0xE3, 0x82, 0x22, 0x80, 0x00, 0x00, 0x00, 0x35, 0x69, 0xF3, 0x1D, 0x00, 0x00, 0x00, 0x00,
};
