/**
 * @file hw_volconv.c
 * @brief MusyX SAL volume/pan conversion, 0x8015D7D0 - 0x8015DEC0.
 *
 * Follows the reference MusyX runtime's hw_volconv.c (AxioDL/musyx). The
 * TU owns its tables (.data 0x80369A68 - 0x80369C90) and its literal pool
 * (.sdata2 0x8047D430 - 0x8047D468); nothing outside salCalcVolume refers
 * to either.
 *
 * CalcBus and CalcBusDPL2 are ordinary static functions, as in the
 * reference. MWCC compiles them first (their 127.0f, 1.0f and 0.7079f are
 * the first entries of the retail pool, ahead of salCalcVolume's own
 * constants) and inlines them into salCalcVolume, CalcBus four times. The
 * out-of-line copies are never called and the linker dead-strips them.
 *
 * Built with -fp_contract off: retail keeps every multiply and add
 * separate (fmuls/fadds, no fmadds), and salCalcVolume matches with that
 * one unit-wide setting.
 */
#include "dolphin/types.h"
#include "crt/math_ppc.h"

typedef struct SAL_VOLINFO {
    f32 volL;
    f32 volR;
    f32 volS;
    f32 volAuxAL;
    f32 volAuxAR;
    f32 volAuxAS;
    f32 volAuxBL;
    f32 volAuxBR;
    f32 volAuxBS;
} SAL_VOLINFO;

typedef struct SAL_PANINFO {
    u32 pan_i;
    u32 pan_im;
    u32 span_i;
    u32 span_im;
    u32 rpan_i;
    u32 rpan_im;
    f32 pan_f;
    f32 pan_fm;
    f32 span_f;
    f32 span_fm;
    f32 rpan_f;
    f32 rpan_fm;
} SAL_PANINFO;

/* dspDLSVolTab, the DLS volume curve (129 entries), defined with the synth
 * data tables. */
extern f32 lbl_8036984C[129];

static f32 musyx_vol_tab[129] = {
    0.0f, 3.05185e-05f, 0.000152593f, 0.000396741f, 0.000701926f,
    0.00112918f, 0.001648f, 0.00222785f, 0.00292978f, 0.00372326f,
    0.00460829f, 0.00558489f, 0.00665304f, 0.00784326f, 0.00912503f,
    0.0104984f, 0.0119633f, 0.0135502f, 0.0151982f, 0.0169988f,
    0.0188604f, 0.0208441f, 0.0229194f, 0.0251167f, 0.0274056f,
    0.0298166f, 0.0323191f, 0.0349437f, 0.0376598f, 0.0404675f,
    0.0434278f, 0.0464797f, 0.0496231f, 0.0528886f, 0.0562761f,
    0.0597858f, 0.0633869f, 0.0671102f, 0.0709555f, 0.0749229f,
    0.0789819f, 0.0831629f, 0.087466f, 0.0919218f, 0.096469f,
    0.101138f, 0.10593f, 0.110843f, 0.115879f, 0.121036f,
    0.126347f, 0.131748f, 0.137303f, 0.142979f, 0.148778f,
    0.154729f, 0.160772f, 0.166997f, 0.173315f, 0.179785f,
    0.186407f, 0.193121f, 0.200018f, 0.207007f, 0.214179f,
    0.221473f, 0.228919f, 0.236488f, 0.244209f, 0.252083f,
    0.260079f, 0.268258f, 0.276559f, 0.285012f, 0.293649f,
    0.302408f, 0.311319f, 0.320383f, 0.3296f, 0.339f,
    0.348521f, 0.358226f, 0.368084f, 0.378094f, 0.388287f,
    0.398633f, 0.409131f, 0.419813f, 0.430647f, 0.441664f,
    0.452864f, 0.464217f, 0.475753f, 0.487442f, 0.499313f,
    0.511399f, 0.523606f, 0.536027f, 0.548631f, 0.561419f,
    0.574389f, 0.587542f, 0.600879f, 0.614399f, 0.628132f,
    0.642018f, 0.656148f, 0.670431f, 0.684927f, 0.699637f,
    0.71453f, 0.729637f, 0.744926f, 0.76043f, 0.776147f,
    0.792077f, 0.808191f, 0.824549f, 0.84109f, 0.857845f,
    0.874844f, 0.892056f, 0.909452f, 0.927122f, 0.945006f,
    0.963073f, 0.981414f, 1.0f, 1.0f,
};

static f32 pan_tab[4] = {
    0.0f,
    0.7079f,
    1.0f,
    1.0f,
};

static f32 pan_tab_dpl2[4] = {
    0.575f,
    0.7079f,
    1.0f,
    1.0f,
};

static void CalcBus(f32* vol_tab, f32* vl, f32* vr, f32* vs, f32 vol, SAL_PANINFO* pi)
{
    u32 i;
    f32 f;
    f32 v;

    i = vol * 127.0f;
    v = (vol * 127) - (f32) i;
    f = (1.0f - v) * vol_tab[i] + v * vol_tab[i + 1];
    *vs = f * ((1.0f - pi->span_f) * pan_tab[pi->span_i] + pi->span_f * pan_tab[pi->span_i + 1]) *
          0.7079f;
    f = f * ((1.0f - pi->span_fm) * pan_tab[pi->span_im] + pi->span_fm * pan_tab[pi->span_im + 1]);
    *vr = f * ((1.0f - pi->pan_f) * pan_tab[pi->pan_i] + pi->pan_f * pan_tab[pi->pan_i + 1]);
    *vl = f * ((1.0f - pi->pan_fm) * pan_tab[pi->pan_im] + pi->pan_fm * pan_tab[pi->pan_im + 1]);
}

static void CalcBusDPL2(f32* vol_tab, f32* fvl, f32* fvr, f32* rvl, f32* rvr, f32 vol,
                        SAL_PANINFO* pi)
{
    u32 i;
    f32 f;
    f32 v;
    f32 vs;

    i = vol * 127;
    f = vol * 127 - (f32) i;
    v = (1.0f - f) * vol_tab[i] + f * vol_tab[i + 1];
    vs = v * ((1.0f - pi->span_f) * pan_tab[pi->span_i] + pi->span_f * pan_tab[pi->span_i + 1]);
    v *= (1.0f - pi->span_fm) * pan_tab[pi->span_im] + pi->span_fm * pan_tab[pi->span_im + 1];
    *fvr = v * ((1.0f - pi->pan_f) * pan_tab[pi->pan_i] + pi->pan_f * pan_tab[pi->pan_i + 1]);
    *fvl = v * ((1.0f - pi->pan_fm) * pan_tab[pi->pan_im] + pi->pan_fm * pan_tab[pi->pan_im + 1]);
    /* The rear pans read pan_tab_dpl2 for the lower entry and pan_tab for
     * the upper one; retail does the same (0x8015DD4C/0x8015DD50). */
    *rvr = vs * ((1.0f - pi->rpan_f) * pan_tab_dpl2[pi->rpan_i] +
                 pi->rpan_f * pan_tab[pi->rpan_i + 1]);
    *rvl = vs * ((1.0f - pi->rpan_fm) * pan_tab_dpl2[pi->rpan_im] +
                 pi->rpan_fm * pan_tab[pi->rpan_im + 1]);
}

void salCalcVolume(u8 voltab_index, SAL_VOLINFO* vi, f32 vol, u32 pan, u32 span, f32 auxa,
                   f32 auxb, u32 itd, u32 dpl2)
{
    f32* vol_tab;
    f32 p;
    f32 sp;
    SAL_PANINFO pi;

    if (voltab_index == 0) {
        vol_tab = musyx_vol_tab;
    } else {
        vol_tab = lbl_8036984C;
    }

    if (pan == 0x800000) {
        pan = 0;
        span = 0x7F0000;
    }

    pan = (pan <= 0x10000 ? 0 : pan - 0x10000);
    span = (span <= 0x10000 ? 0 : span - 0x10000);

    p = pan * 2.4220301e-07f;
    sp = span * 2.4220301e-07f;

    if (dpl2 != FALSE) {
        pi.rpan_f = fmodf(p, 1.0f);
        pi.rpan_i = p;
        pi.rpan_fm = fmodf(2.0f - p, 1.0f);
        pi.rpan_im = 2.0f - p;
    }

    if (itd != FALSE) {
        p = (p - 1.0f) * 0.76604f + 1.0f;
    }

    pi.pan_f = fmodf(p, 1.0f);
    pi.pan_i = p;
    pi.span_f = fmodf(sp, 1.0f);
    pi.span_i = sp;
    p = 2.0f - p;
    sp = 2.0f - sp;
    pi.pan_fm = fmodf(p, 1.0f);
    pi.pan_im = p;
    pi.span_fm = fmodf(sp, 1.0f);
    pi.span_im = sp;

    if (!dpl2) {
        CalcBus(vol_tab, &vi->volL, &vi->volR, &vi->volS, vol, &pi);
        CalcBus(vol_tab, &vi->volAuxAL, &vi->volAuxAR, &vi->volAuxAS, auxa, &pi);
        CalcBus(vol_tab, &vi->volAuxBL, &vi->volAuxBR, &vi->volAuxBS, auxb, &pi);
    } else {
        CalcBusDPL2(vol_tab, &vi->volL, &vi->volR, &vi->volAuxBL, &vi->volAuxBR, vol, &pi);
        CalcBus(vol_tab, &vi->volAuxAL, &vi->volAuxAR, &vi->volAuxAS, auxa, &pi);
        vi->volS = 0.0f;
        vi->volAuxBS = 0.0f;
    }
}
