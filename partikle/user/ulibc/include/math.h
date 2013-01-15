/*
 * $FILE: math.h
 *
 * Math related functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _ULIBC_MATH_H_
#define _ULIBC_MATH_H_

#include <sys/types.h>
#include <stddef.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

extern double fmod(double x, double y);
extern float fmodf(float x, float y);
extern long double fmodl(long double x, long double y);
extern double ceil(double x);
extern float ceilf(float x);
extern long double ceill(long double x);
extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);
extern double rint(double x);
extern float rintf(float x);
extern long double rintl(long double x);
extern double exp(double x);
extern float expf(float x);
extern long double expl(long double x);
extern double atan(double x);
extern float atanf(float x);
extern long double atanl( long double x);
extern double sin(double x);
extern float sinf(float x);
extern long double sinl(long double x);
extern double tan(double x);
extern float tanf(float x);
extern long double tanl(long double x);
extern double asin(double x);
extern float asinf(float x);
extern long double asinl(long double x);
extern double acos(double x);
extern float acosf(float x);
extern long double acosl(long double x);
extern double atan2(double y, double x);
extern float atan2f(float y, float x);
extern long double atan2l(long double y, long double x);
extern double log(double x);
extern float logf(float x);
extern long double logl(long double x);
extern double sqrt(double x);
extern float sqrtf(float x);
extern long double sqrtl(long double x);
extern double pow(double x, double y);
extern float powf(float x, float y);
extern long double powl(long double x, long double y);
extern double cos(double x);
extern float cosf(float x);
extern long double cosl(long double x);

static inline int isinf(double d) {
  union {
    unsigned long long l;
    double d;
  } u;
  u.d=d;
  return (u.l==0x7FF0000000000000ll?1:u.l==0xFFF0000000000000ll?-1:0);
}

static inline int isnan(double d) {
  union {
    unsigned long long l;
    double d;
  } u;
  u.d=d;
  return (u.l==0x7FF8000000000000ll || u.l==0x7FF0000000000000ll || u.l==0xfff8000000000000ll);
}

END_DECLS

#endif
