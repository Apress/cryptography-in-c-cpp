/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach, published by Apress Berkeley CA, 2005             */
/*                                                                            */
/* Module kmul.c           Revision: 19.12.2000                               */
/*                                                                            */
/*  Copyright (C) 1998-2005 by Michael Welschenbach                           */
/*  Copyright (C) 2001-2005 by Springer-Verlag Berlin, Heidelberg             */
/*  Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA                      */
/*  Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland                      */
/*  Copyright (C) 2002-2005 by PHEI, P.R.China                                */
/*  Copyright (C) 2002-2005 by InfoBook, Korea                                */
/*  Copyright (C) 2002-2005 by Triumph Publishing, Russia                     */
/*                                                                            */
/*  All Rights Reserved                                                       */
/*                                                                            */
/*  The software may be used for noncommercial purposes and may be altered,   */
/*  as long as the following conditions are accepted without any              */
/*  qualification:                                                            */
/*                                                                            */
/*  (1) All changes to the sources must be identified in such a way that the  */
/*      changed software cannot be misinterpreted as the original software.   */
/*                                                                            */
/*  (2) The statements of copyright may not be removed or altered.            */
/*                                                                            */
/*  (3) The following DISCLAIMER is accepted:                                 */
/*                                                                            */
/*  DISCLAIMER:                                                               */
/*                                                                            */
/*  There is no warranty for the software contained in this distribution, to  */
/*  the extent permitted by applicable law. The copyright holders provide the */
/*  software `as is' without warranty of any kind, either expressed or        */
/*  implied, including, but not limited to, the implied warranty of fitness   */
/*  for a particular purpose. The entire risk as to the quality and           */
/*  performance of the program is with you.                                   */
/*                                                                            */
/*  In no event unless required by applicable law or agreed to in writing     */
/*  will the copyright holders, or any of the individual authors named in     */
/*  the source files, be liable to you for damages, including any general,    */
/*  special, incidental or consequential damages arising out of any use of    */
/*  the software or out of inability to use the software (including but not   */
/*  limited to any financial losses, loss of data or data being rendered      */
/*  inaccurate or losses sustained by you or by third parties as a result of  */
/*  a failure of the software to operate with any other programs), even if    */
/*  such holder or other party has been advised of the possibility of such    */
/*  damages.                                                                  */
/*                                                                            */
/******************************************************************************/

#include <memory.h>
#include "flint.h"
#include "kmul.h"

#define NO_ASSERTS

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_assert.h"
#include "_alloc.h"
#ifdef COVERAGE
#include "utclog.h"
#endif
#endif

#ifdef NO_ASSERTS
#define Assert(a) (void)0
#endif


static void kmul (clint *, clint *, int, int, CLINT);
static void ksqr (clint *, int, CLINT);

static void shiftadd (CLINT a_l, CLINT b_l, int l, CLINT s_l);
static void addkar (clint *a_l, clint *b_l, int len_arg, CLINT s_l);

#define MUL_THRESHOLD 40
#define SQR_THRESHOLD 40

CLINTD tmp_l;

/******************************************************************************/
/*                                                                            */
/*  Function:  Interface to Karatsuba multiplication                          */
/*  Syntax:    void kmul_l (CLINT a_l, CLINT b_l, CLINT c_l);                 */
/*  Input:     aa_l, bb_l (Factors)                                           */
/*  Output:    p_l (Product)                                                  */
/*  Returns:   E_CLINT_OK if everything is O.K.                               */
/*             E_CLINT_OFL in case of Overflow                                */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
kmul_l (CLINT a_l, CLINT b_l, CLINT p_l)
{
  CLINT aa_l, bb_l;
  CLINTD pp_l;
  int OFL = E_CLINT_OK;

  cpy_l (aa_l, a_l);
  cpy_l (bb_l, b_l);

  kmul (LSDPTR_L(aa_l), LSDPTR_L(bb_l), DIGITS_L (aa_l), DIGITS_L (bb_l), pp_l);

  if (DIGITS_L (pp_l) > (USHORT) CLINTMAXDIGIT)                /* Overflow ? */
    {
      ANDMAX_L (pp_l);                            /* Reduction modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (p_l, pp_l);

  ZEROCLINT_L (aa_l);
  ZEROCLINT_L (bb_l);
  ZEROCLINTD_L (pp_l);
  ZEROCLINTD_L (tmp_l);
  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Interface to Karatsuba squaring                                */
/*  Syntax:    void ksqr_l (CLINT a_l, CLINT c_l);                            */
/*  Input:     aa_l (Factor)                                                  */
/*  Output:    p_l (Square)                                                   */
/*  Returns:   E_CLINT_OK if everything is O.K.                               */
/*             E_CLINT_OFL in case of overflow                                */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ksqr_l (CLINT a_l, CLINT p_l)
{
  CLINT aa_l;
  CLINTD pp_l;
  int OFL = E_CLINT_OK;

  cpy_l (aa_l, a_l);

  ksqr (LSDPTR_L(aa_l), DIGITS_L (aa_l), pp_l);

  if (DIGITS_L (pp_l) > (USHORT) CLINTMAXDIGIT)                /* Overflow ? */
    {
      ANDMAX_L (pp_l);                            /* Reduction modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (p_l, pp_l);

  ZEROCLINT_L (aa_l);
  ZEROCLINTD_L (pp_l);
  ZEROCLINTD_L (tmp_l);
  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Karatsuba multiplication of two factors a_l and b_l            */
/*             with 2k digits to base B                                       */
/*  Syntax:    void kmul (clint *aptr_l, clint *bptr_l,                       */
/*                        int len_a, int len_b, CLINT p_l);                   */
/*  Input:     aptr_l (Pointer to least significant digit of a_l)             */
/*             bptr_l (Pointer to least significant digit of b_l)             */
/*             len_a (Number of digits of a_l)                                */
/*             len_b (Number of digits of b_l)                                */
/*  Output:    p_l (Product)                                                  */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
static void
kmul (clint *aptr_l, clint *bptr_l, int len_a, int len_b, CLINT p_l)
{
  CLINT c01_l, c10_l;
  clint c0_l[CLINTMAXSHORT + 2], c1_l[CLINTMAXSHORT + 2], c2_l[CLINTMAXSHORT + 2];
  clint *a1ptr_l, *b1ptr_l;
  int l2;

  if ((len_a == len_b) && (len_a >= MUL_THRESHOLD) && (0 == (len_a & 1)))
    {
      l2 = len_a/2;
      a1ptr_l = aptr_l + l2;
      b1ptr_l = bptr_l + l2;

      kmul (aptr_l, bptr_l, l2, l2, c0_l);
      kmul (a1ptr_l, b1ptr_l, l2, l2, c1_l);

      addkar (a1ptr_l, aptr_l, l2, c01_l);
      addkar (b1ptr_l, bptr_l, l2, c10_l);

      kmul (LSDPTR_L (c01_l), LSDPTR_L (c10_l), DIGITS_L (c01_l), DIGITS_L (c10_l), c2_l);

      sub (c2_l, c1_l, tmp_l);
      sub (tmp_l, c0_l, c2_l);

      shiftadd (c1_l, c2_l, l2, tmp_l);
      shiftadd (tmp_l, c0_l, l2, p_l);
    }

  else /* Fallback to nonrecursive multiplication */
    {
      memcpy (LSDPTR_L (c1_l), aptr_l, len_a * sizeof (clint));
      memcpy (LSDPTR_L (c2_l), bptr_l, len_b * sizeof (clint));
      SETDIGITS_L (c1_l, len_a);
      SETDIGITS_L (c2_l, len_b);
      mult (c1_l, c2_l, p_l);
    }

  ZEROCLINT_L (c01_l);
  ZEROCLINT_L (c10_l);
  ZEROCLINT_L (c0_l);
  ZEROCLINT_L (c1_l);
  ZEROCLINT_L (c2_l);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Karatsuba squaring of a factor a_l                             */
/*             with 2k digits to base B                                       */
/*  Syntax:    void kmul (clint *aptr_l, int len_a, CLINT p_l);               */
/*  Input:     aptr_l (Pointer to least significant digit of a_l)             */
/*             len_a (Number of digits of a_l)                                */
/*  Output:    p_l (Square)                                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
static void
ksqr (clint *aptr_l, int len_a, CLINT p_l)
{
  CLINT c01_l;
  clint c0_l[CLINTMAXSHORT + 2], c1_l[CLINTMAXSHORT + 2], c2_l[CLINTMAXSHORT + 2];
  clint *a1ptr_l;
  int l2;

  if ((len_a >= SQR_THRESHOLD) && (0 == (len_a & 1)))
    {
      l2 = len_a/2;
      a1ptr_l = aptr_l + l2;

      ksqr (aptr_l, l2, c0_l);
      ksqr (a1ptr_l, l2, c1_l);

      addkar (a1ptr_l, aptr_l, l2, c01_l);

      ksqr (LSDPTR_L (c01_l), DIGITS_L (c01_l), c2_l);

      sub (c2_l, c1_l, tmp_l);
      sub (tmp_l, c0_l, c2_l);

      shiftadd (c1_l, c2_l, l2, tmp_l);
      shiftadd (tmp_l, c0_l, l2, p_l);
    }

  else /* Fallback to nonrecursive squaring */
    {
      memcpy (LSDPTR_L (c1_l), aptr_l, len_a * sizeof (clint));
      SETDIGITS_L (c1_l, len_a);
      sqr (c1_l, p_l);
    }

  ZEROCLINT_L (c01_l);
  ZEROCLINT_L (c0_l);
  ZEROCLINT_L (c1_l);
  ZEROCLINT_L (c2_l);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Addition of two arguments                                      */
/*             Parameters are pointers to LSD of arguments                    */
/*  Syntax:    void addkar (clint *a_l, clint *b_l, int dgts, CLINT s_l);     */
/*  Input:     a_l, b_l (Arguments with equal number of digits)               */
/*  Output:    s_l (Sum)                                                      */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
static void
addkar (clint *a_l, clint *b_l, int dgts, CLINT s_l)
{
  clint *msdptra_l;
  clint *aptr_l, *bptr_l, *sptr_l = LSDPTR_L (s_l);
  ULONG carry = 0L;

  aptr_l = a_l;
  bptr_l = b_l;
  msdptra_l = a_l + dgts - 1;
  SETDIGITS_L (s_l, dgts);

  while (aptr_l <= msdptra_l)
    {
      *sptr_l++ = (USHORT) (carry = (ULONG) * aptr_l++ + (ULONG) * bptr_l++
                            + (ULONG) (USHORT) (carry >> BITPERDGT));
    }

  if (carry & BASE)
    {
      *sptr_l = 1;
      INCDIGITS_L (s_l);
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Addition of two arguments combined with shift of first         */
/*             argument, w/o check for leading zeros                          */
/*  Syntax:    void shiftadd (CLINT a_l, CLINT b_l, int dgts, CLINT s_l);     */
/*  Input:     a_l, b_l (arguments)                                           */
/*             dgts (Exponent to base 2)                                      */
/*             Assumption: DIGITS_L (a_l) + dgts >= DIGITS_L (b_l))           */
/*  Output:    s_l (Sum = (2^dgts)*a_l + b_l)                                 */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
static void
shiftadd (CLINT a_l, CLINT b_l, int dgts, CLINT s_l)
{
  clint *msdptra_l, *msdptrb_l;
  clint *aptr_l, *bptr_l, *sptr_l = LSDPTR_L (s_l);
  ULONG carry = 0L;

  aptr_l = LSDPTR_L (a_l);
  bptr_l = LSDPTR_L (b_l);
  msdptra_l = MSDPTR_L (a_l);
  msdptrb_l = MSDPTR_L (b_l);
  SETDIGITS_L (s_l, DIGITS_L (a_l) + dgts);

  while (bptr_l <= MIN (msdptrb_l, b_l + dgts))
    {
      *sptr_l++ = *bptr_l++;
    }

  while (sptr_l <= s_l + dgts)
    {
      *sptr_l++ = 0;
    }

  while (bptr_l <= msdptrb_l)
    {
      *sptr_l++ = (USHORT) (carry = (ULONG) * aptr_l++ + (ULONG) * bptr_l++
                            + (ULONG) (USHORT) (carry >> BITPERDGT));
    }

  while (aptr_l <= msdptra_l)
    {
      *sptr_l++ = (USHORT) (carry = (ULONG) * aptr_l++
                            + (ULONG) (USHORT) (carry >> BITPERDGT));
    }

  if (carry & BASE)
    {
      *sptr_l = 1;
      INCDIGITS_L (s_l);
    }

  RMLDZRS_L (s_l);
}





















