/**
 * @file snd_song_table.c
 * @brief REL 125 (common_rel): the song table.
 *
 * Module 125 .rodata 0x56C - 0xC54 and .data 0x111B8C - 0x111E04. The
 * module's _prolog publishes the table through lbl_80478E34 and its count
 * (79) through lbl_80478E30. main.dol's sound code (fn_801653CC and its
 * neighbours) loads song N from its resource id, or from the path when the
 * id is 0 (retail has no such entry; entry 10 has neither).
 *
 * The ids use the FSYS member id format (file type 0x08).
 */
#include "dolphin/types.h"

typedef struct SndSong {
    /* 0x0 */ u32 fileId;
    /* 0x4 */ const char* path;
} SndSong;

SndSong lbl_125_data_111B8C[79] = {
    { 0x019F0800, "sound/null_bgm.song" },
    { 0x01A40800, "sound/windycity.song" },
    { 0x01A20800, "sound/watercity.song" },
    { 0x01A00800, "sound/stand.song" },
    { 0x01A10800, "sound/stand2.song" },
    { 0x01E30800, "sound/country.song" },
    { 0x01E20800, "sound/darkside.song" },
    { 0x01E50800, "sound/pokecen.song" },
    { 0x01E40800, "sound/saint.song" },
    { 0x01A30800, "sound/wind_atmos_loop.song" },
    { 0x00000000, NULL },
    { 0x01E60800, "sound/mirrorbo.song" },
    { 0x01E70800, "sound/gtsolo.song" },
    { 0x01E80800, "sound/darkunder.song" },
    { 0x02EE0800, "sound/chakumelo00.song" },
    { 0x02F10800, "sound/chakumelo01.song" },
    { 0x02F20800, "sound/chakumelo02.song" },
    { 0x02F30800, "sound/chakumelo03.song" },
    { 0x02F40800, "sound/chakumelo04.song" },
    { 0x02F50800, "sound/chakumelo05.song" },
    { 0x02F60800, "sound/chakumelo06.song" },
    { 0x02F70800, "sound/chakumelo07.song" },
    { 0x02F80800, "sound/chakumelo08.song" },
    { 0x02F90800, "sound/chakumelo09.song" },
    { 0x030A0800, "sound/battle7.song" },
    { 0x03080800, "sound/battle2.song" },
    { 0x03330800, "sound/agb_me_asa.song" },
    { 0x05EF0800, "sound/battle5.song" },
    { 0x05F00800, "sound/darkside4.song" },
    { 0x05F10800, "sound/me_news.song" },
    { 0x05F20800, "sound/me_undertime.song" },
    { 0x06A40800, "sound/tool_music.song" },
    { 0x06A10800, "sound/tool_battle1.song" },
    { 0x06A20800, "sound/tool_battle2.song" },
    { 0x06A30800, "sound/tool_battle3.song" },
    { 0x06F10800, "sound/me_fue.song" },
    { 0x06F20800, "sound/me_welcome.song" },
    { 0x06F00800, "sound/agb_fanfa1b.song" },
    { 0x08070800, "sound/fanfare00.song" },
    { 0x08080800, "sound/fanfare01.song" },
    { 0x08090800, "sound/fanfare02.song" },
    { 0x080A0800, "sound/mt_battle.song" },
    { 0x080F0800, "sound/dungeon.song" },
    { 0x080B0800, "sound/agb_fanfa5.song" },
    { 0x080C0800, "sound/agb_me_shinka.song" },
    { 0x080D0800, "sound/agb_shinka.song" },
    { 0x080E0800, "sound/battle6.song" },
    { 0x0B5A0800, "sound/me_win.song" },
    { 0x0B590800, "sound/me_snatch.song" },
    { 0x0B580800, "sound/me_relive.song" },
    { 0x0B570800, "sound/battle9.song" },
    { 0x0BE80800, "sound/ev_snatch.song" },
    { 0x0BE70800, "sound/ev_shiccho.song" },
    { 0x0D120800, "sound/worldmap.song" },
    { 0x0D110800, "sound/battle8.song" },
    { 0x0D600800, "sound/shinpi.song" },
    { 0x0D5F0800, "sound/battle9plus.song" },
    { 0x0D710800, "sound/tretre.song" },
    { 0x0DAA0800, "sound/worldmap2.song" },
    { 0x0FED0800, "sound/end_roll_b.song" },
    { 0x0FF00800, "sound/title.song" },
    { 0x0FEE0800, "sound/ev_shadow.song" },
    { 0x0FEF0800, "sound/miraclebo.song" },
    { 0x0FF10800, "sound/me_chukei.song" },
    { 0x10810800, "sound/fanfare03.song" },
    { 0x10900800, "sound/demo_roll.song" },
    { 0x10A20800, "sound/open_roll.song" },
    { 0x10A30800, "sound/jyakira.song" },
    { 0x10B40800, "sound/hyper_shiccho.song" },
    { 0x10B30800, "sound/end_ev_1.song" },
    { 0x11140800, "sound/end_ev_2.song" },
    { 0x11490800, "sound/end_roll_c.song" },
    { 0x114A0800, "sound/level_up.song" },
    { 0x114C0800, "sound/siren_atmos.song" },
    { 0x114D0800, "sound/virtual.song" },
    { 0x114F0800, "sound/me_winwin.song" },
    { 0x114E0800, "sound/me_ex_tre.song" },
    { 0x119A0800, "sound/gs_logo.song" },
    { 0x12640800, "sound/ev_kinpaku.song" },
};
