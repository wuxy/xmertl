/*
 * $FILE: lang_supp.c
 *
 * Some language support stuffs
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

#include <lang_supp.h>

//--------------//
// __dso_handle //
//--------------//

void *__dso_handle; /*only the address of this symbol is taken by gcc*/

struct object
{
  void (*f)(void*);
  void *p;
  void *d;
} object[32] = {[0 ... 31] = {0,0,0}};
unsigned int iObject = 0;

//--------------//
// __cxa_atexit //
//--------------//

int __cxa_atexit(void (*f)(void *), void *p, void *d)
{
  if (iObject >= 32) return -1;
  object[iObject].f = f;
  object[iObject].p = p;
  object[iObject].d = d;
  ++iObject;
  return 0;
}

//----------------//
// __cxa_finalize //
//----------------//

/* This currently destroys all objects */
void __cxa_finalize(void *d)
{
  unsigned int i = iObject;
  for (; i > 0; --i) {
      --iObject;
      object[iObject].f(object[iObject].p);
  }
}

//-----------------//
// dl_iterate_phdr //
//-----------------//
/*
int dl_iterate_phdr(int (*callback) (struct dl_phdr_info *info, 
				     size_t size, void *data), void *data) {
  return -1;
  }*/

//-----------------------//
// __register_frame_info //
//-----------------------//

void __register_frame_info (const void *begin, struct object *ob) 
     __attribute__ ((weak));

void __register_frame_info (const void *begin, struct object *ob) {
}

//-------------------------//
// __deregister_frame_info //
//-------------------------//

void *__deregister_frame_info (const void *begin)
     __attribute__ ((weak));

void *__deregister_frame_info (const void *begin) {
  return 0;
}

//---------------------//
// _Jv_RegisterClasses //
//---------------------//

extern void _Jv_RegisterClasses (void *class)      __attribute__ ((weak));

void _Jv_RegisterClasses (void *class) {
}

//-------------//
// frame_dummy //
//-------------//

extern int __FRAME_END__[];
extern char __EH_FRAME_BEGIN__[];
extern void *__JCR_LIST__[];
static void frame_dummy (void) {
  static struct object object;

  if (__register_frame_info)
    __register_frame_info (__EH_FRAME_BEGIN__, &object);

  if (__JCR_LIST__[0] && _Jv_RegisterClasses)
    _Jv_RegisterClasses (__JCR_LIST__);
}

//----------------//
// init_lang_supp //
//----------------//

typedef void (*func_ptr) (void);
extern func_ptr __CTOR_END__ [];
void init_lang_supp (void) {
  func_ptr *p;

  frame_dummy ();

  for (p = __CTOR_END__ - 1; *p != (func_ptr) -1; p--)
    (*p) ();
}

//------------------//
// finish_lang_supp //
//------------------//
extern func_ptr __DTOR_LIST__[];

void finish_lang_supp (void) {
  func_ptr *p = __DTOR_LIST__ + 1;
  static _Bool completed = 0;
  func_ptr f;

  if (__builtin_expect (completed, 0))
    return;

  __cxa_finalize (__dso_handle);
  
  while ((f = *p)) {
    p++;
    f ();
  }
  if (__deregister_frame_info)
    __deregister_frame_info (__EH_FRAME_BEGIN__);

  completed = 1;
}
