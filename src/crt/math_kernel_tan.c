/**
 * @file math_kernel_tan.c
 * @brief fdlibm k_tan.c, 0x800CD648 - 0x800CD85C.
 */
#include "dolphin/types.h"
#include "crt/math_ppc.h"

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
