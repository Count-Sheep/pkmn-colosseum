/**
 * @file math_range_800CB4D8.c
 * @brief MSL fdlibm fmod, log, pow and rem_pio2, 0x800CB4D8 - 0x800CC660.
 *
 * Sun fdlibm e_fmod.c, e_log.c, e_pow.c and e_rem_pio2.c with the MSL
 * changes: errno on domain errors, NAN from __float_nan, ldexp for
 * scalbn.
 */
#include "crt/math.h"

#define __HI(x) *(s32*)&x
#define __LO(x) *(1 + (s32*)&x)

#define EDOM 33
#define NAN (*(f32*)lbl_80478AC0)

extern s32 lbl_8047AA10; /* errno */
#define errno lbl_8047AA10

extern f64 sqrt(f64 x);

inline f64 fabs(f64 x)
{
    return __fabs(x);
}

extern f64 ldexp(f64 x, s32 n);
extern s32 __kernel_rem_pio2(f64* x, f64* y, s32 e0, s32 nx, s32 prec,
                             const s32* ipio2);


/* e_fmod.c */
#if !defined(MATH_E_SPLIT) || defined(MATH_E_FMOD)

static const f64 one = 1.0, Zero[] = { 0.0, -0.0 };

f64 __ieee754_fmod(f64 x, f64 y)
{
    int n, hx, hy, hz, ix, iy, sx, i;
    unsigned lx, ly, lz;

    hx = __HI(x);
    lx = __LO(x);
    hy = __HI(y);
    ly = __LO(y);
    sx = hx & 0x80000000;
    hx ^= sx;
    hy &= 0x7FFFFFFF;

    /* purge off exception values */
    if ((hy | ly) == 0 || (hx >= 0x7FF00000) ||
        ((hy | ((ly | -ly) >> 31)) > 0x7FF00000)) {
        return (x * y) / (x * y);
    }
    if (hx <= hy) {
        if ((hx < hy) || (lx < ly)) {
            return x;
        }
        if (lx == ly) {
            return Zero[(u32)sx >> 31];
        }
    }

    /* determine ix = ilogb(x) */
    if (hx < 0x00100000) {
        if (hx == 0) {
            for (ix = -1043, i = lx; i > 0; i <<= 1) {
                ix -= 1;
            }
        } else {
            for (ix = -1022, i = (hx << 11); i > 0; i <<= 1) {
                ix -= 1;
            }
        }
    } else {
        ix = (hx >> 20) - 1023;
    }

    /* determine iy = ilogb(y) */
    if (hy < 0x00100000) {
        if (hy == 0) {
            for (iy = -1043, i = ly; i > 0; i <<= 1) {
                iy -= 1;
            }
        } else {
            for (iy = -1022, i = (hy << 11); i > 0; i <<= 1) {
                iy -= 1;
            }
        }
    } else {
        iy = (hy >> 20) - 1023;
    }

    /* set up {hx,lx}, {hy,ly} and align y to x */
    if (ix >= -1022) {
        hx = 0x00100000 | (0x000FFFFF & hx);
    } else {
        n = -1022 - ix;
        if (n <= 31) {
            hx = (hx << n) | (lx >> (32 - n));
            lx <<= n;
        } else {
            hx = lx << (n - 32);
            lx = 0;
        }
    }
    if (iy >= -1022) {
        hy = 0x00100000 | (0x000FFFFF & hy);
    } else {
        n = -1022 - iy;
        if (n <= 31) {
            hy = (hy << n) | (ly >> (32 - n));
            ly <<= n;
        } else {
            hy = ly << (n - 32);
            ly = 0;
        }
    }

    /* fix point fmod */
    n = ix - iy;
    while (n--) {
        hz = hx - hy;
        lz = lx - ly;
        if (lx < ly) {
            hz -= 1;
        }
        if (hz < 0) {
            hx = hx + hx + (lx >> 31);
            lx = lx + lx;
        } else {
            if ((hz | lz) == 0) {
                return Zero[(u32)sx >> 31];
            }
            hx = hz + hz + (lz >> 31);
            lx = lz + lz;
        }
    }
    hz = hx - hy;
    lz = lx - ly;
    if (lx < ly) {
        hz -= 1;
    }
    if (hz >= 0) {
        hx = hz;
        lx = lz;
    }

    /* convert back to floating value and restore the sign */
    if ((hx | lx) == 0) {
        return Zero[(u32)sx >> 31];
    }
    while (hx < 0x00100000) {
        hx = hx + hx + (lx >> 31);
        lx = lx + lx;
        iy -= 1;
    }
    if (iy >= -1022) {
        hx = ((hx - 0x00100000) | ((iy + 1023) << 20));
        __HI(x) = hx | sx;
        __LO(x) = lx;
    } else {
        n = -1022 - iy;
        if (n <= 20) {
            lx = (lx >> n) | ((u32)hx << (32 - n));
            hx >>= n;
        } else if (n <= 31) {
            lx = (hx << (32 - n)) | (lx >> n);
            hx = sx;
        } else {
            lx = hx >> (n - 32);
            hx = sx;
        }
        __HI(x) = hx | sx;
        __LO(x) = lx;
        x *= one;
    }
    return x;
}

#endif

/* e_log.c */
#if !defined(MATH_E_SPLIT) || defined(MATH_E_LOG)

static const f64
ln2_hi = 6.93147180369123816490e-01,
ln2_lo = 1.90821492927058770002e-10,
two54 = 1.80143985094819840000e+16,
Lg1 = 6.666666666666735130e-01,
Lg2 = 3.999999999940941908e-01,
Lg3 = 2.857142874366239149e-01,
Lg4 = 2.222219843214978396e-01,
Lg5 = 1.818357216161805012e-01,
Lg6 = 1.531383769920937332e-01,
Lg7 = 1.479819860511658591e-01;

static f64 zero = 0.0;

f64 __ieee754_log(f64 x)
{
    f64 hfsq, f, s, z, R, w, t1, t2, dk;
    s32 k, hx, i, j;
    u32 lx;

    hx = __HI(x);
    lx = __LO(x);

    k = 0;
    if (hx < 0x00100000) {
        if (((hx & 0x7FFFFFFF) | lx) == 0) {
            return -two54 / zero;
        }
        if (hx < 0) {
            errno = EDOM;
            return (x - x) / zero;
        }
        k -= 54;
        x *= two54;
        hx = __HI(x);
    }
    if (hx >= 0x7FF00000) {
        return x + x;
    }
    k += (hx >> 20) - 1023;
    hx &= 0x000FFFFF;
    i = (hx + 0x95F64) & 0x100000;
    __HI(x) = hx | (i ^ 0x3FF00000);
    k += (i >> 20);
    f = x - 1.0;
    if ((0x000FFFFF & (2 + hx)) < 3) {
        if (f == zero) {
            if (k == 0) {
                return zero;
            } else {
                dk = (f64)k;
                return dk * ln2_hi + dk * ln2_lo;
            }
        }
        R = f * f * (0.5 - 0.33333333333333333 * f);
        if (k == 0) {
            return f - R;
        } else {
            dk = (f64)k;
            return dk * ln2_hi - ((R - dk * ln2_lo) - f);
        }
    }
    s = f / (2.0 + f);
    dk = (f64)k;
    z = s * s;
    i = hx - 0x6147A;
    w = z * z;
    j = 0x6B851 - hx;
    t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
    t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
    i |= j;
    R = t2 + t1;
    if (i > 0) {
        hfsq = 0.5 * f * f;
        if (k == 0) {
            return f - (hfsq - s * (hfsq + R));
        } else {
            return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
        }
    } else {
        if (k == 0) {
            return f - s * (f - R);
        } else {
            return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
        }
    }
}

#endif

/* e_pow.c */
#if !defined(MATH_E_SPLIT) || defined(MATH_E_POW)

static const f64
bp[] = { 1.0, 1.5 },
dp_h[] = { 0.0, 5.84962487220764160156e-01 },
dp_l[] = { 0.0, 1.35003920212974897128e-08 },
pow_zero = 0.0,
two = 2.0,
two53 = 9007199254740992.0,
huge = 1.0e300,
tiny = 1.0e-300,
L1 = 5.99999999999994648725e-01,
L2 = 4.28571428578550184252e-01,
L3 = 3.33333329818377432918e-01,
L4 = 2.72728123808534006489e-01,
L5 = 2.30660745775561754067e-01,
L6 = 2.06975017800338417784e-01,
P1 = 1.66666666666666019037e-01,
P2 = -2.77777777770155933842e-03,
P3 = 6.61375632143793436117e-05,
P4 = -1.65339022054652515390e-06,
P5 = 4.13813679705723846039e-08,
lg2 = 6.93147180559945286227e-01,
lg2_h = 6.93147182464599609375e-01,
lg2_l = -1.90465429995776804525e-09,
ovt = 8.0085662595372944372e-0017,
cp = 9.61796693925975554329e-01,
cp_h = 9.61796700954437255859e-01,
cp_l = -7.02846165095275826516e-09,
ivln2 = 1.44269504088896338700e+00,
ivln2_h = 1.44269502162933349609e+00,
ivln2_l = 1.92596299112661746887e-08;

#if defined(MATH_E_SPLIT)
/*
 * RULE-EXCEPTION(user-approved): in its own object e_pow.c's `one` is
 * written as a literal and the i0 endianness probe reads bp[0] (also 1.0),
 * so MWCC emits no separate unreferenced `one` and the .sdata2 pool is
 * 0x8047C678-0x8047C788 as in retail — see docs/RULE_EXCEPTIONS.md
 */
#define one 1.0
#endif
f64 __ieee754_pow(f64 x, f64 y)
{
    f64 z, ax, z_h, z_l, p_h, p_l;
    f64 y1, t1, t2, r, s, t, u, v, w;
    int i0, i1, i, j, k, yisint, n;
    int hx, hy, ix, iy;
    unsigned lx, ly;

    i0 = ((*(s32*)&bp[0]) >> 29) ^ 1;
    i1 = 1 - i0;
    hx = __HI(x);
    lx = __LO(x);
    hy = __HI(y);
    ly = __LO(y);
    ix = hx & 0x7FFFFFFF;
    iy = hy & 0x7FFFFFFF;

    /* y==zero: x**0 = 1 */
    if ((iy | ly) == 0) {
        return one;
    }

    /* +-NaN return x+y */
    if (ix > 0x7FF00000 || ((ix == 0x7FF00000) && (lx != 0)) ||
        iy > 0x7FF00000 || ((iy == 0x7FF00000) && (ly != 0))) {
        return x + y;
    }

    /* determine if y is an odd int when x < 0 */
    yisint = 0;
    if (hx < 0) {
        if (iy >= 0x43400000) {
            yisint = 2;
        } else if (iy >= 0x3FF00000) {
            k = (iy >> 20) - 0x3FF;
            if (k > 20) {
                j = ly >> (52 - k);
                if ((j << (52 - k)) == ly) {
                    yisint = 2 - (j & 1);
                }
            } else if (ly == 0) {
                j = iy >> (20 - k);
                if ((j << (20 - k)) == iy) {
                    yisint = 2 - (j & 1);
                }
            }
        }
    }

    /* special value of y */
    if (ly == 0) {
        if (iy == 0x7FF00000) {
            if (((ix - 0x3FF00000) | lx) == 0) {
                return y - y;
            } else if (ix >= 0x3FF00000) {
                return (hy >= 0) ? y : pow_zero;
            } else {
                return (hy < 0) ? -y : pow_zero;
            }
        }
        if (iy == 0x3FF00000) {
            if (hy < 0) {
                return one / x;
            } else {
                return x;
            }
        }
        if (hy == 0x40000000) {
            return x * x;
        }
        if (hy == 0x3FE00000) {
            if (hx >= 0) {
                return sqrt(x);
            }
        }
    }

    ax = fabs(x);
    /* special value of x */
    if (lx == 0) {
        if (ix == 0x7FF00000 || ix == 0 || ix == 0x3FF00000) {
            z = ax;
            if (hy < 0) {
                z = one / z;
            }
            if (hx < 0) {
                if (((ix - 0x3FF00000) | yisint) == 0) {
                    z = (z - z) / (z - z);
                } else if (yisint == 1) {
                    z = -z;
                }
            }
            return z;
        }
    }

    /* (x<0)**(non-int) is NaN */
    if ((((hx >> 31) + 1) | yisint) == 0) {
        errno = EDOM;
        return NAN;
    }

    /* |y| is huge */
    if (iy > 0x41E00000) {
        if (iy > 0x43F00000) {
            if (ix <= 0x3FEFFFFF) {
                return (hy < 0) ? huge * huge : tiny * tiny;
            }
            if (ix >= 0x3FF00000) {
                return (hy > 0) ? huge * huge : tiny * tiny;
            }
        }
        /* over/underflow if x is not close to one */
        if (ix < 0x3FEFFFFF) {
            return (hy < 0) ? huge * huge : tiny * tiny;
        }
        if (ix > 0x3FF00000) {
            return (hy > 0) ? huge * huge : tiny * tiny;
        }
        t = x - 1;
        w = (t * t) * (0.5 - t * (0.3333333333333333333333 - t * 0.25));
        u = ivln2_h * t;
        v = t * ivln2_l - w * ivln2;
        t1 = u + v;
        __LO(t1) = 0;
        t2 = v - (t1 - u);
    } else {
        f64 s2, s_h, s_l, t_h, t_l;
        n = 0;
        /* take care subnormal number */
        if (ix < 0x00100000) {
            ax *= two53;
            n -= 53;
            ix = __HI(ax);
        }
        n += ((ix) >> 20) - 0x3FF;
        j = ix & 0x000FFFFF;
        /* determine interval */
        ix = j | 0x3FF00000;
        if (j <= 0x3988E) {
            k = 0;
        } else if (j < 0xBB67A) {
            k = 1;
        } else {
            k = 0;
            n += 1;
            ix -= 0x00100000;
        }
        __HI(ax) = ix;

        /* compute s = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
        u = ax - bp[k];
        v = one / (ax + bp[k]);
        s = u * v;
        s_h = s;
        __LO(s_h) = 0;
        /* t_h=ax+bp[k] High */
        t_h = pow_zero;
        __HI(t_h) = ((ix >> 1) | 0x20000000) + 0x00080000 + (k << 18);
        t_l = ax - (t_h - bp[k]);
        s_l = v * ((u - s_h * t_h) - s_h * t_l);
        /* compute log(ax) */
        s2 = s * s;
        r = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2 * (L5 + s2 * L6)))));
        r += s_l * (s_h + s);
        s2 = s_h * s_h;
        t_h = 3.0 + s2 + r;
        __LO(t_h) = 0;
        t_l = r - ((t_h - 3.0) - s2);
        /* u+v = s*(1+...) */
        u = s_h * t_h;
        v = s_l * t_h + t_l * s;
        /* 2/(3log2)*(s+...) */
        p_h = u + v;
        __LO(p_h) = 0;
        p_l = v - (p_h - u);
        z_h = cp_h * p_h;
        z_l = cp_l * p_h + p_l * cp + dp_l[k];
        /* log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l */
        t = (f64)n;
        t1 = (((z_h + z_l) + dp_h[k]) + t);
        __LO(t1) = 0;
        t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);
    }

    s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
    if ((((hx >> 31) + 1) | (yisint - 1)) == 0) {
        s = -one;
    }

    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
    y1 = y;
    __LO(y1) = 0;
    p_l = (y - y1) * t1 + y * t2;
    p_h = y1 * t1;
    z = p_l + p_h;
    j = __HI(z);
    i = __LO(z);
    if (j >= 0x40900000) {
        if (((j - 0x40900000) | i) != 0) {
            return s * huge * huge;
        } else {
            if (p_l + ovt > z - p_h) {
                return s * huge * huge;
            }
        }
    } else if ((j & 0x7FFFFFFF) >= 0x4090CC00) {
        if (((j - 0xC090CC00) | i) != 0) {
            return s * tiny * tiny;
        } else {
            if (p_l <= z - p_h) {
                return s * tiny * tiny;
            }
        }
    }
    /*
     * compute 2**(p_h+p_l)
     */
    i = j & 0x7FFFFFFF;
    k = (i >> 20) - 0x3FF;
    n = 0;
    if (i > 0x3FE00000) {
        n = j + (0x00100000 >> (k + 1));
        k = ((n & 0x7FFFFFFF) >> 20) - 0x3FF;
        t = pow_zero;
        __HI(t) = (n & ~(0x000FFFFF >> k));
        n = ((n & 0x000FFFFF) | 0x00100000) >> (20 - k);
        if (j < 0) {
            n = -n;
        }
        p_h -= t;
    }
    t = p_l + p_h;
    __LO(t) = 0;
    u = t * lg2_h;
    v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
    z = u + v;
    w = v - (z - u);
    t = z * z;
    t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
    r = (z * t1) / (t1 - two) - (w + z * w);
    z = one - (r - z);
    j = __HI(z);
    j += (n << 20);
    if ((j >> 20) <= 0) {
        z = ldexp(z, n);
    } else {
        __HI(z) += (n << 20);
    }
    return s * z;
}

#endif

/* e_rem_pio2.c */
#if !defined(MATH_E_SPLIT) || defined(MATH_E_REM_PIO2)

static const s32 two_over_pi[] = {
    0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
    0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
    0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
    0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
    0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
    0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
    0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
    0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
    0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
    0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
    0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,
};

static const s32 npio2_hw[] = {
    0x3FF921FB, 0x400921FB, 0x4012D97C, 0x401921FB, 0x401F6A7A,
    0x4022D97C, 0x4025FDBB, 0x402921FB, 0x402C463A, 0x402F6A7A,
    0x4031475C, 0x4032D97C, 0x40346B9C, 0x4035FDBB, 0x40378FDB,
    0x403921FB, 0x403AB41B, 0x403C463A, 0x403DD85A, 0x403F6A7A,
    0x40407E4C, 0x4041475C, 0x4042106C, 0x4042D97C, 0x4043A28C,
    0x40446B9C, 0x404534AC, 0x4045FDBB, 0x4046C6CB, 0x40478FDB,
    0x404858EB, 0x404921FB,
};

static const f64
rem_zero = 0.00000000000000000000e+00,
half = 5.00000000000000000000e-01,
two24 = 1.67772160000000000000e+07,
invpio2 = 6.36619772367581382433e-01,
pio2_1 = 1.57079632673412561417e+00,
pio2_1t = 6.07710050650619224932e-11,
pio2_2 = 6.07710050630396597660e-11,
pio2_2t = 2.02226624879595063154e-21,
pio2_3 = 2.02226624871116645580e-21,
pio2_3t = 8.47842766036889956997e-32;

s32 __ieee754_rem_pio2(f64 x, f64* y)
{
    f64 z, w, t, r, fn;
    f64 tx[3];
    s32 e0, i, j, nx, n, ix, hx;

    hx = __HI(x);
    ix = hx & 0x7FFFFFFF;
    if (ix <= 0x3FE921FB) {
        y[0] = x;
        y[1] = 0;
        return 0;
    }
    if (ix < 0x4002D97C) {
        if (hx > 0) {
            z = x - pio2_1;
            if (ix != 0x3FF921FB) {
                y[0] = z - pio2_1t;
                y[1] = (z - y[0]) - pio2_1t;
            } else {
                z -= pio2_2;
                y[0] = z - pio2_2t;
                y[1] = (z - y[0]) - pio2_2t;
            }
            return 1;
        } else {
            z = x + pio2_1;
            if (ix != 0x3FF921FB) {
                y[0] = z + pio2_1t;
                y[1] = (z - y[0]) + pio2_1t;
            } else {
                z += pio2_2;
                y[0] = z + pio2_2t;
                y[1] = (z - y[0]) + pio2_2t;
            }
            return -1;
        }
    }
    if (ix <= 0x413921FB) {
        t = fabs(x);
        n = (s32)(t * invpio2 + half);
        fn = (f64)n;
        r = t - fn * pio2_1;
        w = fn * pio2_1t;
        if (n < 32 && ix != npio2_hw[n - 1]) {
            y[0] = r - w;
        } else {
            j = ix >> 20;
            y[0] = r - w;
            i = j - (((__HI(y[0])) >> 20) & 0x7FF);
            if (i > 16) {
                t = r;
                w = fn * pio2_2;
                r = t - w;
                w = fn * pio2_2t - ((t - r) - w);
                y[0] = r - w;
                i = j - (((__HI(y[0])) >> 20) & 0x7FF);
                if (i > 49) {
                    t = r;
                    w = fn * pio2_3;
                    r = t - w;
                    w = fn * pio2_3t - ((t - r) - w);
                    y[0] = r - w;
                }
            }
        }
        y[1] = (r - y[0]) - w;
        if (hx < 0) {
            y[0] = -y[0];
            y[1] = -y[1];
            return -n;
        } else {
            return n;
        }
    }
    /*
     * all other (large) arguments
     */
    if (ix >= 0x7FF00000) {
        y[0] = y[1] = x - x;
        return 0;
    }
    /* set z = scalbn(|x|,ilogb(x)-23) */
    __LO(z) = __LO(x);
    e0 = (ix >> 20) - 1046;
    __HI(z) = ix - (e0 << 20);
    for (i = 0; i < 2; i++) {
        tx[i] = (f64)((s32)(z));
        z = (z - tx[i]) * two24;
    }
    tx[2] = z;
    nx = 3;
    while (tx[nx - 1] == rem_zero) {
        nx--;
    }
    n = __kernel_rem_pio2(tx, y, e0, nx, 2, two_over_pi);
    if (hx < 0) {
        y[0] = -y[0];
        y[1] = -y[1];
        return -n;
    }
    return n;
}
#endif
