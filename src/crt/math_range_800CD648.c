/**
 * @file math_range_800CD648.c
 * @brief Shared libm candidate range, 0x800CD648 - 0x800CDBB8.
 */
#include "dolphin/types.h"

/*
 * fdlibm s_atan.c / k_tan.c. In retail these were separate objects, so the
 * atan tables sat at the start of their own .rodata; defining them before the
 * shared libm include keeps atan's section-relative offsets at zero.
 */
static const f64 atanhi[] = {
    4.63647609000806093515e-01,
    7.85398163397448278999e-01,
    9.82793723247329054082e-01,
    1.57079632679489655800e+00,
};
static const f64 atanlo[] = {
    2.26987774529616870924e-17,
    3.06161699786838301793e-17,
    1.39033110312309984516e-17,
    6.12323399573676603587e-17,
};
static const f64 aT[] = {
    3.33333333333329318027e-01,
    -1.99999999998764832476e-01,
    1.42857142725034663711e-01,
    -1.11111104054623557880e-01,
    9.09088713343650656196e-02,
    -7.69187620504482999495e-02,
    6.66107313738753120669e-02,
    -5.83357013379057348645e-02,
    4.97687799461593236017e-02,
    -3.65315727442169155270e-02,
    1.62858201153657823623e-02,
};
#include "src/crt/math_range_800CAA58.c"

#define __HI(x) *(int*)&x
#define __LO(x) *(1 + (int*)&x)

static const f64 one = 1.00000000000000000000e+00;
static const f64 pio4 = 7.85398163397448278999e-01;
static const f64 pio4lo = 3.06161699786838301793e-17;
static const f64 T[] = {
    3.33333333333334091986e-01,
    1.33333333333201242699e-01,
    5.39682539762260521377e-02,
    2.18694882948595424599e-02,
    8.86323982359930005737e-03,
    3.59207910759131235356e-03,
    1.45620945432529025516e-03,
    5.88041240820264096874e-04,
    2.46463134818469906812e-04,
    7.81794442939557092300e-05,
    7.14072491382608190305e-05,
    -1.85586374855275456654e-05,
    2.59073051863633712884e-05,
};

f64 __kernel_tan(f64 x, f64 y, s32 iy)
{
    f64 z, r, v, w, s;
    int ix, hx;

    hx = __HI(x);
    ix = hx & 0x7fffffff;
    if (ix < 0x3e300000) {
        if ((int)x == 0) {
            if (((ix | __LO(x)) | (iy + 1)) == 0) {
                return one / fabs(x);
            } else {
                return (iy == 1) ? x : -one / x;
            }
        }
    }
    if (ix >= 0x3FE59428) {
        if (hx < 0) {
            x = -x;
            y = -y;
        }
        z = pio4 - x;
        w = pio4lo - y;
        x = z + w;
        y = 0.0;
    }
    z = x * x;
    w = z * z;
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r += T[0] * s;
    w = x + r;
    if (ix >= 0x3FE59428) {
        v = (f64)iy;
        return (f64)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }
    if (iy == 1) {
        return w;
    } else {
        f64 a, t;
        z = w;
        __LO(z) = 0;
        v = r - (z - x);
        t = a = -1.0 / w;
        __LO(t) = 0;
        s = 1.0 + t * z;
        return t + a * (s + t * v);
    }
}

static const f64 huge = 1.0e300;

f64 atan(f64 x)
{
    f64 w, s1, s2, z;
    int ix, hx, id;

    hx = __HI(x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x44100000) {
        if (ix > 0x7ff00000 || (ix == 0x7ff00000 && (__LO(x) != 0))) {
            return x + x;
        }
        if (hx > 0) {
            return atanhi[3] + atanlo[3];
        } else {
            return -atanhi[3] - atanlo[3];
        }
    }
    if (ix < 0x3fdc0000) {
        if (ix < 0x3e200000) {
            if (huge + x > one) {
                return x;
            }
        }
        id = -1;
    } else {
        x = __fabs(x);
        if (ix < 0x3ff30000) {
            if (ix < 0x3fe60000) {
                id = 0;
                x = (2.0 * x - one) / (2.0 + x);
            } else {
                id = 1;
                x = (x - one) / (x + one);
            }
        } else {
            if (ix < 0x40038000) {
                id = 2;
                x = (x - 1.5) / (one + 1.5 * x);
            } else {
                id = 3;
                x = -1.0 / x;
            }
        }
    }
    z = x * x;
    w = z * z;
    s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
    s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));
    if (id < 0) {
        return x - x * (s1 + s2);
    } else {
        z = atanhi[id] - ((x * (s1 + s2) - atanlo[id]) - x);
        return (hx < 0) ? -z : z;
    }
}
