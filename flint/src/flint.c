/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module flint.c          Revision: 19.05.2005                               */
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
/*                                                                            */
/*  Requirements:                                                             */
/*                                                                            */
/*  sizeof (ULONG)  == 4                                                      */
/*  sizeof (USHORT) == 2                                                      */
/*                                                                            */
/******************************************************************************/

#ifndef FLINT_ANSI
#define FLINT_ANSI
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "flint.h"

#define NO_ASSERTS 1

#define FLINTCVMAJ  3
#define FLINTCVMIN  0

#if ((FLINTCVMIN != FLINT_VERMIN) || (FLINTCVMAJ != FLINT_VERMAJ))
#error Error: Incomaptible versions of FLINT.C and FLINT.H
#endif

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


/* Mute wrap up error messages of PC-lint */
/*lint -esym(14,add,sub,mul,umul,sqr)   */
/*lint -esym(15,add,sub,mul,umul,sqr)   */
/*lint -esym(515,add,sub,mul,umul,sqr)  */
/*lint -esym(516,add,sub,mul,umul,sqr)  */
/*lint -esym(532,add,sub,mul,umul,sqr)  */
/*lint -esym(533,add,sub,mul,umul,sqr)  */
/*lint -esym(1066,add,sub,mul,umul,sqr) */
/*lint -esym(534,add_l,sub_l,mul_l,sqr_l,div_l,mmul_l,msub_l,dec_l,madd_l) */
/*lint -esym(534,msqr_l,mexp_l,mexp5_l,mexpk_l,mod_l,mod2_l,mexp2_l) */


/***********  Prototypes of local functions ***********************************/

/* Private register functions */
static void
destroy_reg_l (void);
static int
allocate_reg_l (void);
/* Integer square roots from ULONG values */
static ULONG
ul_iroot (unsigned long n);
/*  Test and set bit w/o checking for overrun */
static int 
setbit (CLINT a_l, unsigned int pos);

#ifdef FLINT_SECURE
#define PURGEVARS_L(X) purgevars_l X
/* Function to purge variables */
static void purgevars_l (int noofvars, ...);
#ifdef FLINT_DEBUG
#define ISPURGED_L(X) Assert(ispurged_l X)
/* Function to check, whether variables have been purged */
static int ispurged_l (int noofvars, ...);
#else
#define ISPURGED_L(X) (void)0
#endif /* FLINT_DEBUG */
#else
#define PURGEVARS_L(X) (void)0
#define ISPURGED_L(X) (void)0
#endif /* FLINT_SECURE */
/******************************************************************************/

/* CLINT-Constant Values */
clint __FLINT_API_DATA
nul_l[] = {0, 0, 0, 0, 0};
clint __FLINT_API_DATA
one_l[] = {1, 1, 0, 0, 0};
clint __FLINT_API_DATA
two_l[] = {1, 2, 0, 0, 0};

#ifdef FLINT_INITRAND
#include "random.h"
STATEPRNG xs;
#endif


/******************************************************************************/
/*                                                                            */
/*  Function:   Initialization of FLINT/C-Library                             */
/*              If the FLINT/C functions are provided as DLL                  */
/*              the function initializing the DLL, e. g. DllMain(),           */
/*              should call this function.                                    */
/*                                                                            */
/*  Syntax:     FLINTInit_l() (void);                                         */
/*  Input:      -                                                             */
/*  Output:     -                                                             */
/*  Returns:    E_CLINT_OK if everything is O.K.                              */
/*              -1 else                                                       */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
FLINTInit_l (void)
{
  int error;
  initrand64_lt();

#ifdef FLINT_INITRAND
  /* Initialization of all PRNGs in global state buffer xs */
  /* N. B.: For cryptographic purposes use individual state buffers */
  /* that are initialized and purged separetely */
  InitRand_l (&xs, "", 0, 200, 0); 
#endif

  error = create_reg_l();

  if (!error)
    return E_CLINT_OK;
  else
    return -1;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Exit the FLINT/C-Library                                      */
/*  Syntax:     FLINTExit_l (void);                                           */
/*  Input:      -                                                             */
/*  Output:     -                                                             */
/*  Returns:    E_CLINT_OK if everything is O.K.                              */
/*              -1 else                                                       */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
FLINTExit_l (void)
{
  free_reg_l();

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Copy CLINT to CLINT                                           */
/*  Syntax:     void cpy_l (CLINT dest_l, CLINT src_l);                       */
/*  Input:      CLINT src_l                                                   */
/*  Output:     CLINT dest_l                                                  */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
cpy_l (CLINT dest_l, CLINT src_l)
{
  clint *lastsrc_l = MSDPTR_L (src_l);
  *dest_l = *src_l;

  while ((*lastsrc_l == 0) && (*dest_l > 0))
    {
      --lastsrc_l;
      --*dest_l;
    }

  while (src_l < lastsrc_l)
    {
      *++dest_l = *++src_l;
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Swap two CLINT operands                                        */
/*  Syntax:    void fswap_l (CLINT a_l, CLINT b_l);                           */
/*  Input:     CLINT a_l, b_l                                                 */
/*  Output:    Swapped CLINT operands b_l, a_l                                */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
fswap_l (CLINT a_l, CLINT b_l)
{
  CLINT tmp_l;

  cpy_l (tmp_l, a_l);
  cpy_l (a_l, b_l);
  cpy_l (b_l, tmp_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Test whether two CLINT operands are equal                      */
/*  Syntax:    int equ_l (CLINT a_l, CLINT b_l);                              */
/*  Input:     CLINT a_l, b_l                                                 */
/*  Output:    -                                                              */
/*  Returns:   1 : a_l and b_l have equal values                              */
/*             0 : otherwise                                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
equ_l (CLINT a_l, CLINT b_l)
{
  clint *msdptra_l, *msdptrb_l;
  int la = (int)DIGITS_L (a_l);
  int lb = (int)DIGITS_L (b_l);

  if (la == 0 && lb == 0)
    {
      return 1;
    }

  while (a_l[la] == 0 && la > 0)
    {
      --la;
    }

  while (b_l[lb] == 0 && lb > 0)
    {
      --lb;
    }

  if (la == 0 && lb == 0)
    {
      return 1;
    }

  if (la != lb)
    {
      return 0;
    }

  msdptra_l = a_l + la;
  msdptrb_l = b_l + lb;

  while ((*msdptra_l == *msdptrb_l) && (msdptra_l > a_l))
    {
      msdptra_l--;
      msdptrb_l--;
    }

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (la), &la,
                   sizeof (lb), &lb));

  ISPURGED_L ((2, sizeof (la), &la,
                  sizeof (lb), &lb));

  return (msdptra_l > a_l ? 0 : 1);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Test whether two CLINT operands are equal modulo m             */
/*  Syntax:    int mequ_l (CLINT a_l, CLINT b_l, CLINT m_l);                  */
/*  Input:     CLINT a_l, b_l (values to compare), CLINT m_l (Modulus)        */
/*  Output:    -                                                              */
/*  Returns:   1          : a_l = b_l mod m_l                                 */
/*             0          : a_l != b_l mod m_l                                */
/*             E_CLINT_DBZ: division by 0                                     */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mequ_l (CLINT a_l, CLINT b_l, CLINT m_l)
{
  CLINT r_l;
  int res;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero? */
    }

  msub_l (a_l, b_l, r_l, m_l);

  res = (0 == DIGITS_L (r_l))?1:0;

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (r_l), r_l));
  ISPURGED_L ((1, sizeof (r_l), r_l));

  return res;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Comparison of two CLINT operands                               */
/*  Syntax:    int cmp_l (CLINT a_l, CLINT b_l);                              */
/*  Input:     CLINT a_l, b_l (values to compare)                             */
/*  Output:    -                                                              */
/*  Returns:   -1: a_l < b_l,                                                 */
/*              0: a_l == b_l,                                                */
/*              1: a_l > b_l                                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
cmp_l (CLINT a_l, CLINT b_l)
{
  clint *msdptra_l, *msdptrb_l;
  int la = (int)DIGITS_L (a_l);
  int lb = (int)DIGITS_L (b_l);

  if (la == 0 && lb == 0)
    {
      return 0;
    }

  while (a_l[la] == 0 && la > 0)
    {
      --la;
    }

  while (b_l[lb] == 0 && lb > 0)
    {
      --lb;
    }

  if (la == 0 && lb == 0)
    {
      return 0;
    }

  if (la > lb)
    {
      PURGEVARS_L ((2, sizeof (la), &la,
                       sizeof (lb), &lb));
      ISPURGED_L  ((2, sizeof (la), &la,
                       sizeof (lb), &lb));
      return 1;
    }

  if (la < lb)
    {
      PURGEVARS_L ((2, sizeof (la), &la,
                       sizeof (lb), &lb));
      ISPURGED_L  ((2, sizeof (la), &la,
                       sizeof (lb), &lb));
      return -1;
    }

  msdptra_l = a_l + la;
  msdptrb_l = b_l + lb;

  while ((*msdptra_l == *msdptrb_l) && (msdptra_l > a_l))
    {
      msdptra_l--;
      msdptrb_l--;
    }

  PURGEVARS_L ((2, sizeof (la), &la,
                   sizeof (lb), &lb));
  ISPURGED_L  ((2, sizeof (la), &la,
                   sizeof (lb), &lb));

  if (msdptra_l == a_l)
    {
      return 0;
    }

  if (*msdptra_l > *msdptrb_l)
    {
      return 1;
    }
  else
    {
      return -1;
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of maximum CLINT value 2^CLINTMAXBIT - 1            */
/*  Syntax:    clint * setmax_l (CLINT a_l);                                  */
/*  Input:     a_l CLINT variable                                             */
/*  Output:    a_l set to value of 2^CLINTMAXBIT - 1 = Nmax                   */
/*  Returns:   Address of CLINT variable a_l                                  */
/*                                                                            */
/******************************************************************************/
clint * __FLINT_API
setmax_l (CLINT a_l)
{
  clint *aptr_l = a_l;
  clint *msdptra_l = a_l + CLINTMAXDIGIT;

  while (++aptr_l <= msdptra_l)
    {
      *aptr_l = BASEMINONE;
    }

  SETDIGITS_L (a_l, CLINTMAXDIGIT);
  return (clint *)a_l;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Addition of two CLINT operands                                 */
/*  Syntax:    int add_l (CLINT a_l, CLINT b_l, CLINT s_l);                   */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    s_l (Sum)                                                      */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
add_l (CLINT a_l, CLINT b_l, CLINT s_l)
{
  clint ss_l[CLINTMAXSHORT + 1];
  int OFL = 0;

  add (a_l, b_l, ss_l);

  if (DIGITS_L (ss_l) > (USHORT)CLINTMAXDIGIT)       /* Overflow ? */
    {
      ANDMAX_L (ss_l);                  /* Reduction modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (s_l, ss_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (ss_l), ss_l));
  ISPURGED_L  ((1, sizeof (ss_l), ss_l));

  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Subtraction of one CLINT operand from another                  */
/*  Syntax:    int sub_l (CLINT aa_l, CLINT bb_l, CLINT d_l);                 */
/*  Input:     aa_l, bb_l (Operands)                                          */
/*  Output:    d_l (Value of a_l - b_l)                                       */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_UFL: Underflow                                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
sub_l (CLINT aa_l, CLINT bb_l, CLINT d_l)
{
  CLINT b_l;
  clint a_l[CLINTMAXSHORT + 1], t_l[CLINTMAXSHORT + 1], tmp_l[CLINTMAXSHORT + 1];
  int UFL = 0;

  cpy_l (b_l, bb_l);

  if (LT_L (aa_l, b_l))            /* Underflow ? */
    {
      setmax_l (a_l);              /* We calculate with Nmax             */
      cpy_l (t_l, aa_l);           /* aa_l will be needed once again, ...*/
      UFL = E_CLINT_UFL;           /*  ... will be corrected at the end  */
    }
  else
    {
      cpy_l (a_l, aa_l);
    }

  sub (a_l, b_l, tmp_l);

  if (UFL)
    {                              /* Underflow ? */
      add_l (tmp_l, t_l, d_l);     /* Correction needed */
      inc_l (d_l);                 /* One is missing */
    }
  else
    {
      cpy_l (d_l, tmp_l);
    }

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (t_l), t_l,
                   sizeof (tmp_l), tmp_l));

  ISPURGED_L  ((4, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (t_l), t_l,
                   sizeof (tmp_l), tmp_l));

  return UFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Increment                                                      */
/*  Syntax:    int inc_l (CLINT a_l);                                         */
/*  Input:     a_l (CLINT value)                                              */
/*  Output:    a_l, incremented by 1                                          */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
inc_l (CLINT a_l)
{
  clint *msdptra_l, *aptr_l = LSDPTR_L (a_l);
  ULONG carry = BASE;
  int OFL = 0;

  msdptra_l = MSDPTR_L (a_l);
  while ((aptr_l <= msdptra_l) && (carry & BASE))
    {
      *aptr_l = (USHORT)(carry = 1UL + (ULONG)(*aptr_l));
      aptr_l++;
    }

  if ((aptr_l > msdptra_l) && (carry & BASE))
    {
      *aptr_l = 1;
      INCDIGITS_L (a_l);
      if (DIGITS_L (a_l) > (USHORT)CLINTMAXDIGIT)    /* Overflow ? */
        {
          SETZERO_L (a_l);              /* Reduction modulo Nmax+1 */
          OFL = E_CLINT_OFL;
        }
    }

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (carry), &carry));
  ISPURGED_L  ((1, sizeof (carry), &carry));

  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Decrement                                                      */
/*  Syntax:    int dec_l (CLINT a_l);                                         */
/*  Input:     a_l (CLINT value)                                              */
/*  Output:    a_l, decremented by 1                                          */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_UFL: Underflow                                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
dec_l (CLINT a_l)
{
  clint *msdptra_l, *aptr_l = LSDPTR_L (a_l);
  ULONG carry = DBASEMINONE;

  if (DIGITS_L (a_l) == 0)                     /* Underflow ? */
    {
      setmax_l (a_l);              /* Reduction modulo Nmax+1 */
      return E_CLINT_UFL;
    }

  msdptra_l = MSDPTR_L (a_l);
  while ((aptr_l <= msdptra_l) && (carry & (BASEMINONEL << BITPERDGT)))
    {
      *aptr_l = (USHORT)(carry = (ULONG)*aptr_l - 1L);
      aptr_l++;
    }

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (carry), &carry));
  ISPURGED_L  ((1, sizeof (carry), &carry));

  RMLDZRS_L (a_l);
  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Multiplication                                                 */
/*  Syntax:    int mul_l (CLINT f1_l, CLINT f2_l, CLINT pp_l);                */
/*  Input:     f1_l, f2_l (Factors)                                           */
/*  Output:    p_l (Product)                                                  */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mul_l (CLINT f1_l, CLINT f2_l, CLINT pp_l)
{
  CLINT a_l, b_l;
  CLINTD p_l;
  int OFL = 0;

  cpy_l (a_l, f1_l);
  cpy_l (b_l, f2_l);

  mult (a_l, b_l, p_l);

  if (DIGITS_L (p_l) > (USHORT)CLINTMAXDIGIT)   /* Overflow ? */
    {
      ANDMAX_L (p_l);              /* Reduction modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (pp_l, p_l);

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (p_l), p_l));

  ISPURGED_L  ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (p_l), p_l));

  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Squaring                                                       */
/*  Syntax:    int sqr_l (CLINT f_l, CLINT pp_l);                             */
/*  Input:     f_l (Factor)                                                   */
/*  Output:    pp_l (Square)                                                  */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
sqr_l (CLINT f_l, CLINT pp_l)
{
  CLINT a_l;
  CLINTD p_l;
  int OFL = 0;

  cpy_l (a_l, f_l);

  sqr (a_l, p_l);

  if (DIGITS_L (p_l) > (USHORT)CLINTMAXDIGIT)   /* Overflow ? */
    {
      ANDMAX_L (p_l);              /* Reduction modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (pp_l, p_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (a_l), a_l,
                   sizeof (p_l), p_l));

  ISPURGED_L  ((2, sizeof (a_l), a_l,
                   sizeof (p_l), p_l));

  return OFL;
}


#if !defined FLINT_ASM
/******************************************************************************/
/*                                                                            */
/*  Function:  Integer Division                                               */
/*  Syntax:    int div_l (CLINT d1_l, CLINT d2_l, CLINT quot_l, CLINT rem_l); */
/*  Input:     d1_l (Dividend), d2_l (Divisor)                                */
/*  Output:    quot_l (Quotient), rem_l (Remainder)                           */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*  Remark:    A double long dividend (type CLINTD) is supported as long as   */
/*             calling function provides sufficient memory for the quotient.  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
div_l (CLINT d1_l, CLINT d2_l, CLINT quot_l, CLINT rem_l)
{
  register clint *rptr_l, *bptr_l;
  CLINT b_l;
  clint r_l[2 + (CLINTMAXDIGIT << 1)]; /* Provide space for double long */
                                       /* dividend + 1 digit */
  clint *qptr_l, *msdptrb_l, *msdptrr_l, *lsdptrr_l;
  USHORT bv, rv, qhat, ri, ri_1, ri_2, bn_1, bn_2;
  ULONG right, left, rhat, borrow, carry, sbitsminusd;
  unsigned int d = 0;
  int i;

  cpy_l (r_l, d1_l);
  cpy_l (b_l, d2_l);

  if (EQZ_L (b_l))
    {
      PURGEVARS_L ((1, sizeof (r_l), r_l));
      ISPURGED_L  ((1, sizeof (r_l), r_l));

      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQZ_L (r_l))
    {
      SETZERO_L (quot_l);
      SETZERO_L (rem_l);

      PURGEVARS_L ((1, sizeof (b_l), b_l));
      ISPURGED_L  ((1, sizeof (b_l), b_l));

      return E_CLINT_OK;
    }

  i = cmp_l (r_l, b_l);

  if (i == -1)
    {
      cpy_l (rem_l, r_l);
      SETZERO_L (quot_l);

      PURGEVARS_L ((2, sizeof (b_l), b_l,
                       sizeof (r_l), r_l));
      ISPURGED_L  ((2, sizeof (b_l), b_l,
                       sizeof (r_l), r_l));
      return E_CLINT_OK;
    }
  else if (i == 0)
    {
      SETONE_L (quot_l);
      SETZERO_L (rem_l);

      PURGEVARS_L ((2, sizeof (b_l), b_l,
                       sizeof (r_l), r_l));
      ISPURGED_L  ((2, sizeof (b_l), b_l,
                       sizeof (r_l), r_l));
      return E_CLINT_OK;
    }

  if (DIGITS_L (b_l) == 1)
    {
      goto shortdiv;
    }

  /* Step 1 */
  msdptrb_l = MSDPTR_L (b_l);

  bn_1 = *msdptrb_l;
  while (bn_1 < BASEDIV2)
    {
      d++;
      bn_1 <<= 1;
    }

  sbitsminusd = (int)BITPERDGT - d;

  if (d > 0)
    {
      bn_1 += *(msdptrb_l - 1) >> sbitsminusd;

      if (DIGITS_L (b_l) > 2)
        {
          bn_2 = (USHORT)((*(msdptrb_l - 1) << d) + (*(msdptrb_l - 2) >> sbitsminusd));
        }
      else
        {
          bn_2 = (USHORT)(*(msdptrb_l - 1) << d);
        }
    }
  else
    {
      bn_2 = (USHORT)(*(msdptrb_l - 1));
    }

  /* Steps 2 and 3 */
  msdptrr_l = MSDPTR_L (r_l) + 1;
  lsdptrr_l = MSDPTR_L (r_l) - DIGITS_L (b_l) + 1;
  *msdptrr_l = 0;

  qptr_l = quot_l + DIGITS_L (r_l) - DIGITS_L (b_l) + 1;

  /* Step 4 */
  while (lsdptrr_l >= LSDPTR_L (r_l))
    {
      ri = (USHORT)((*msdptrr_l << d) + (*(msdptrr_l - 1) >> sbitsminusd));

      ri_1 = (USHORT)((*(msdptrr_l - 1) << d) + (*(msdptrr_l - 2) >> sbitsminusd));

      if (msdptrr_l - 3 > r_l)
        {
          ri_2 = (USHORT)((*(msdptrr_l - 2) << d) + (*(msdptrr_l - 3) >> sbitsminusd));
        }
      else
        {
          ri_2 = (USHORT)(*(msdptrr_l - 2) << d);
        }

      if (ri != bn_1)               /* almost always */
        {
          qhat = (USHORT)((rhat = ((ULONG)ri << BITPERDGT) + (ULONG)ri_1) / bn_1);
          right = ((rhat = (rhat - (ULONG)bn_1 * qhat)) << BITPERDGT) + ri_2;

          /* test qhat */

          if ((left = (ULONG)bn_2 * qhat) > right)
            {
              qhat--;
              if ((rhat + bn_1) < BASE)
                  /* else bn_2 * qhat < rhat * b_l */
                {
                  if ((left - bn_2) > (right + ((ULONG)bn_1 << BITPERDGT)))
                    {
                      qhat--;
                    }
                }
            }
        }
      else                        /* ri == bn_1, almost never */
        {
          qhat = BASEMINONE;
          right = ((ULONG)(rhat = (ULONG)bn_1 + (ULONG)ri_1) << BITPERDGT) + ri_2;
          if (rhat < BASE)       /* else bn_2 * qhat < rhat * b_l */
            {
              /* test qhat */

              if ((left = (ULONG)bn_2 * qhat) > right)
                {
                  qhat--;
                  if ((rhat + bn_1) < BASE)
                      /* else bn_2 * qhat < rhat * b_l */
                    {
                      if ((left - bn_2) > (right + ((ULONG)bn_1 << BITPERDGT)))
                        {
                          qhat--;
                        }
                    }
                }
            }
        }

      /* Step 5 */
      borrow = BASE;
      carry = 0;
      for (bptr_l = LSDPTR_L (b_l), rptr_l = lsdptrr_l; bptr_l <= msdptrb_l; bptr_l++, rptr_l++)
        {
          if (borrow >= BASE)
            {
              *rptr_l = (USHORT)(borrow = ((ULONG)(*rptr_l) + BASE -
                         (ULONG)(USHORT)(carry = (ULONG)(*bptr_l) *
                         qhat + (ULONG)(USHORT)(carry >> BITPERDGT))));
            }
          else
            {
              *rptr_l = (USHORT)(borrow = ((ULONG)(*rptr_l) + BASEMINONEL -
                                (ULONG)(USHORT)(carry = (ULONG)(*bptr_l) *
                                qhat + (ULONG)(USHORT)(carry >> BITPERDGT))));
            }
        }

      if (borrow >= BASE)
        {
          *rptr_l = (USHORT)(borrow = ((ULONG)(*rptr_l) + BASE -
                             (ULONG)(USHORT)(carry >> BITPERDGT)));
        }
      else
        {
          *rptr_l = (USHORT)(borrow = ((ULONG)(*rptr_l) + BASEMINONEL -
                                    (ULONG)(USHORT)(carry >> BITPERDGT)));
        }

      /* Step 6 */
      *qptr_l = qhat;

      if (borrow < BASE)
        {
          carry = 0;
          for (bptr_l = LSDPTR_L (b_l), rptr_l = lsdptrr_l; bptr_l <= msdptrb_l; bptr_l++, rptr_l++)
            {
              *rptr_l = (USHORT)(carry = ((ULONG)(*rptr_l) + (ULONG)(*bptr_l) +
                                          (ULONG)(USHORT)(carry >> BITPERDGT)));
            }
          *rptr_l += (USHORT)(carry >> BITPERDGT);
          (*qptr_l)--;
        }

      /* Step 7 */
      msdptrr_l--;
      lsdptrr_l--;
      qptr_l--;
    }

  /* Step 8 */
  SETDIGITS_L (quot_l, DIGITS_L (r_l) - DIGITS_L (b_l) + 1);
  RMLDZRS_L (quot_l);

  SETDIGITS_L (r_l, DIGITS_L (b_l));
  cpy_l (rem_l, r_l);

  /* Purging of variables */
  PURGEVARS_L ((17, sizeof (bv), &bv,
                    sizeof (rv), &rv,
                    sizeof (qhat), &qhat,
                    sizeof (ri), &ri,
                    sizeof (ri_1), &ri_1,
                    sizeof (ri_2), &ri_2,
                    sizeof (bn_1), &bn_1,
                    sizeof (bn_2), &bn_2,
                    sizeof (right), &right,
                    sizeof (left), &left,
                    sizeof (rhat), &rhat,
                    sizeof (borrow), &borrow,
                    sizeof (carry), &carry,
                    sizeof (sbitsminusd), &sbitsminusd,
                    sizeof (d), &d,
                    sizeof (b_l), b_l,
                    sizeof (r_l), r_l));

  ISPURGED_L  ((17, sizeof (bv), &bv,
                    sizeof (rv), &rv,
                    sizeof (qhat), &qhat,
                    sizeof (ri), &ri,
                    sizeof (ri_1), &ri_1,
                    sizeof (ri_2), &ri_2,
                    sizeof (bn_1), &bn_1,
                    sizeof (bn_2), &bn_2,
                    sizeof (right), &right,
                    sizeof (left), &left,
                    sizeof (rhat), &rhat,
                    sizeof (borrow), &borrow,
                    sizeof (carry), &carry,
                    sizeof (sbitsminusd), &sbitsminusd,
                    sizeof (d), &d,
                    sizeof (b_l), b_l,
                    sizeof (r_l), r_l));

  return E_CLINT_OK;

  /* Division by divisor with one-digit */
  shortdiv:

  rv = 0;
  bv = *LSDPTR_L (b_l);
  for (rptr_l = MSDPTR_L (r_l), qptr_l = quot_l + DIGITS_L (r_l); rptr_l >= LSDPTR_L (r_l); rptr_l--, qptr_l--)
    {
      *qptr_l = (USHORT)((rhat = ((((ULONG)rv) << BITPERDGT) +
                                          (ULONG)*rptr_l)) / bv);
      rv = (USHORT)(rhat - (ULONG)bv * (ULONG)*qptr_l);
    }

  SETDIGITS_L (quot_l, DIGITS_L (r_l));

  RMLDZRS_L (quot_l);
  u2clint_l (rem_l, rv);

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (rv), &rv,
                   sizeof (bv), &bv,
                   sizeof (b_l), b_l,
                   sizeof (r_l), r_l));

  ISPURGED_L  ((4, sizeof (rv), &rv,
                   sizeof (bv), &bv,
                   sizeof (b_l), b_l,
                   sizeof (r_l), r_l));

  return E_CLINT_OK;
}
#endif /* FLINT_ASM */


/******************************************************************************/
/*                                                                            */
/*  Function:  Binary rightshift by 1 bit                                     */
/*  Syntax:    int shr_l (CLINT a_l);                                         */
/*  Input:     a_l (Operand)                                                  */
/*  Output:    a_l (Shifted value)                                            */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_UFL: Underflow                                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
shr_l (CLINT a_l)
{
  clint *ap_l;
  USHORT help, carry = 0;

  if (DIGITS_L (a_l) == 0)
    {
      return E_CLINT_UFL;          /* Underflow */
    }

  for (ap_l = MSDPTR_L (a_l); ap_l > a_l; ap_l--)
    {
      help = (USHORT)((USHORT)(*ap_l >> 1) | (USHORT)(carry << (BITPERDGT - 1)));
      carry = (USHORT)(*ap_l & 1U);
      *ap_l = help;
    }

  RMLDZRS_L (a_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (help), &help,
                   sizeof (carry), &carry));

  ISPURGED_L  ((2, sizeof (help), &help,
                   sizeof (carry), &carry));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Binary leftshift by 1 bit                                      */
/*  Syntax:    int shl_l (CLINT a_l);                                         */
/*  Input:     a_l (Operand)                                                  */
/*  Output:    a_l (Shifted value)                                            */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
shl_l (CLINT a_l)
{
  clint *ap_l, *msdptra_l;
  ULONG carry = 0L;
  int error = E_CLINT_OK;

  RMLDZRS_L (a_l);
  if (ld_l (a_l) >= (USHORT)CLINTMAXBIT)
    {
      SETDIGITS_L (a_l, CLINTMAXDIGIT);
      error = E_CLINT_OFL;         /* Overflow */
    }

  msdptra_l = MSDPTR_L (a_l);
  for (ap_l = LSDPTR_L (a_l); ap_l <= msdptra_l; ap_l++)
    {
      *ap_l = (USHORT)(carry = ((ULONG)(*ap_l) << 1) | (carry >> BITPERDGT));
    }

  if (carry >> BITPERDGT)
    {
      if (DIGITS_L (a_l) < CLINTMAXDIGIT)
        {
          *ap_l = 1;
          INCDIGITS_L (a_l);
          error = E_CLINT_OK;
        }
      else
        {
          error = E_CLINT_OFL;
        }
    }

  RMLDZRS_L (a_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (carry), &carry));
  ISPURGED_L  ((1, sizeof (carry), &carry));

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Binary left-/rightshift by n bits                              */
/*  Syntax:    int shift_l (CLINT n_l, long int noofbits);                    */
/*  Input:     n_l (Operand)                                                  */
/*             noofbits (Number of places to be shifted)                      */
/*             negative sign: Shift right                                     */
/*             positive sign: Shift left                                      */
/*  Output:    a_l (Shifted value)                                            */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*             E_CLINT_UFL: Underflow                                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
shift_l (CLINT n_l, long int noofbits)
{
  USHORT shorts = (USHORT)((ULONG)(noofbits < 0 ? -noofbits : noofbits) / BITPERDGT);
  USHORT bits = (USHORT)((ULONG)(noofbits < 0 ? -noofbits : noofbits) % BITPERDGT);
  long int resl;
  USHORT i;
  int error = E_CLINT_OK;

  clint *nptr_l;
  clint *msdptrn_l;

  RMLDZRS_L (n_l);
  resl = (int)ld_l (n_l) + noofbits;

  if (DIGITS_L (n_l) == 0)
    {
      shorts = bits = 0;
      return ((resl < 0) ? E_CLINT_UFL : E_CLINT_OK);
    }

  if (noofbits == 0)
    {
      return E_CLINT_OK;
    }

  if ((resl < 0) || (resl > (long)CLINTMAXBIT))
    {
      error = ((resl < 0) ? E_CLINT_UFL : E_CLINT_OFL);   /* Under-/Overflow */
    }

  SETDIGITS_L (n_l, MIN (DIGITS_L (n_l), CLINTMAXDIGIT));

  if (noofbits < 0)
    {

      /* Shift Right */

      shorts = (USHORT)MIN (DIGITS_L (n_l), shorts);
      msdptrn_l = MSDPTR_L (n_l) - shorts;
      for (nptr_l = LSDPTR_L (n_l); nptr_l <= msdptrn_l; nptr_l++)
        {
          *nptr_l = *(nptr_l + shorts);
        }
      SETDIGITS_L (n_l, DIGITS_L (n_l) - shorts);

      for (i = 0; i < bits; i++)
        {
          shr_l (n_l);
        }
    }
  else
    {

      /* Shift Left   */

      if (shorts < CLINTMAXDIGIT)
        {
          SETDIGITS_L (n_l, MIN ((USHORT)(DIGITS_L (n_l) + shorts), CLINTMAXDIGIT));
          nptr_l = n_l + DIGITS_L (n_l);
          msdptrn_l = n_l + shorts;
          while (nptr_l > msdptrn_l)
            {
              *nptr_l = *(nptr_l - shorts);
              --nptr_l;
            }

          while (nptr_l > n_l)
            {
              *nptr_l-- = 0;
            }

          RMLDZRS_L (n_l);
          for (i = 0; i < bits; i++)
            {
              shl_l (n_l);
            }
        }
      else
        {
          SETZERO_L (n_l);
        }
    }

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (resl), &resl,
                   sizeof (shorts), &shorts,
                   sizeof (bits), &bits));

  ISPURGED_L  ((3, sizeof (resl), &resl,
                   sizeof (shorts), &shorts,
                   sizeof (bits), &bits));

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Reduction modulo m                                             */
/*  Syntax:    int mod_l (CLINT dv_l, CLINT ds_l, CLINT r_l);                 */
/*  Input:     dv_l (Dividend), ds_l (Divisor)                                */
/*  Output:    r_l (Remainder of dv_l mod ds_l)                               */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*  Remark:    A double long dividend (type CLINTD) is supported as long as   */
/*             calling function provides sufficient memory for the quotient.  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mod_l (CLINT dv_l, CLINT ds_l, CLINT r_l)
{
  CLINTD junk_l;
  int err;

  err = div_l (dv_l, ds_l, junk_l, r_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (junk_l), junk_l));
  ISPURGED_L  ((1, sizeof (junk_l), junk_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Reduction mod 2^k                                              */
/*  Syntax:    int mod2_l (CLINT d_l, ULONG k, CLINT r_l);                    */
/*  Input:     d_l (Dividend), k (Exponent of 2^k)                            */
/*  Output:    r_l (Remainder of d_l mod 2^k)                                 */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*  Remark:    A double long dividend (type CLINTD) is supported as long as   */
/*             calling function provides sufficient memory for the quotient.  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mod2_l (CLINT d_l, ULONG k, CLINT r_l)
{
  int i;

  cpy_l (r_l, d_l);

  if (k > CLINTMAXBIT)
    {
      return E_CLINT_OK;
    }

  i = 1 + (k >> LDBITPERDGT);

  if (i > (int)DIGITS_L (r_l))
    {
      return E_CLINT_OK;
    }

  r_l[i] &= (1U << (k & (BITPERDGT - 1UL))) - 1U;
  SETDIGITS_L (r_l, i);            /* r_l[i] = 2^(k mod BITPERDGT) - 1 */

  RMLDZRS_L (r_l);
  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular addition                                               */
/*  Syntax:    int madd_l (CLINT aa_l, CLINT bb_l, CLINT c_l, CLINT m_l);     */
/*  Input:     aa_l, bb_l, m_l (Operands)                                     */
/*  Output:    c_l (Remainder of aa_l + bb_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
madd_l (CLINT aa_l, CLINT bb_l, CLINT c_l, CLINT m_l)
{
  CLINT a_l, b_l;
  clint tmp_l[CLINTMAXSHORT + 1];

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  cpy_l (a_l, aa_l);
  cpy_l (b_l, bb_l);

  if (GE_L (a_l, m_l) || GE_L (b_l, m_l))
    {
      add (a_l, b_l, tmp_l);
      mod_l (tmp_l, m_l, c_l);
    }
  else
    {
      add (a_l, b_l, tmp_l);
      if (GE_L (tmp_l, m_l))
        {
          sub_l (tmp_l, m_l, tmp_l);    /* Underflow prevented */
        }
      cpy_l (c_l, tmp_l);
    }

  Assert(DIGITS_L (c_l) <= CLINTMAXDIGIT);

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  ISPURGED_L  ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular subtraction                                            */
/*  Syntax:    int msub_l (CLINT aa_l, CLINT bb_l, CLINT c_l, CLINT m_l);     */
/*  Input:     aa_l, bb_l, m_l (Operands)                                     */
/*  Output:    c_l (Remainder of aa_l - bb_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
msub_l (CLINT aa_l, CLINT bb_l, CLINT c_l, CLINT m_l)
{
  CLINT a_l, b_l, tmp_l;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  cpy_l (a_l, aa_l);
  cpy_l (b_l, bb_l);

  if (GE_L (a_l, b_l))
    {
      sub (a_l, b_l, tmp_l);
      mod_l (tmp_l, m_l, c_l);
    }
  else
    {
      sub (b_l, a_l, tmp_l);       /* Sign tmp_l = -1 */
      mod_l (tmp_l, m_l, tmp_l);
      if (GTZ_L (tmp_l))
        {
          sub (m_l, tmp_l, c_l);
        }
      else
        {
          SETZERO_L (c_l);
        }
    }

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  ISPURGED_L  ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular Multiplication                                         */
/*  Syntax:    int mmul_l (CLINT aa_l, CLINT bb_l, CLINT c_l, CLINT m_l);     */
/*  Input:     aa_l, bb_l, m_l (Operands)                                     */
/*  Output:    c_l (Remainder of aa_l * bb_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mmul_l (CLINT aa_l, CLINT bb_l, CLINT c_l, CLINT m_l)
{
  CLINT a_l, b_l;
  CLINTD tmp_l;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  cpy_l (a_l, aa_l);
  cpy_l (b_l, bb_l);

  mult (a_l, b_l, tmp_l);
  mod_l (tmp_l, m_l, c_l);

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  ISPURGED_L  ((3, sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular squaring                                               */
/*  Syntax:    int msqr_l (CLINT aa_l, CLINT c_l, CLINT m_l);                 */
/*  Input:     aa_l, m_l (Operands)                                           */
/*  Output:    c_l (Remainder of aa_l * aa_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
msqr_l (CLINT aa_l, CLINT c_l, CLINT m_l)
{
  CLINT a_l;
  CLINTD tmp_l;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  cpy_l (a_l, aa_l);

  sqr (a_l, tmp_l);
  mod_l (tmp_l, m_l, c_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (a_l), a_l,
                   sizeof (tmp_l), tmp_l));

  ISPURGED_L  ((2, sizeof (a_l), a_l,
                   sizeof (tmp_l), tmp_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Montgomery multiplication                                      */
/*  Syntax:    void mulmon_l (CLINT a_l, CLINT b_l, CLINT n_l, USHORT nprime, */
/*                                                 USHORT logB_r, CLINT p_l); */
/*  Input:     a_l, b_l (Factors)                                             */
/*             n_l (Modulus, odd, n_l > a_l, b_l)                             */
/*             nprime (-n_l^(-1) mod B)                                       */
/*             logB_r (Integral part of logarithm of r to base B)             */
/*             (For an explanation of the operands cf. Chap. 6)               */
/*  Output:    p_l (Remainder of a_l * b_l * r^(-1) mod n_l)                  */
/*             with r := B^logB_r, B^(logB_r-1) <= n_l < B^logB_r)            */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
mulmon_l(CLINT a_l, CLINT b_l, CLINT n_l, USHORT nprime, USHORT logB_r, CLINT p_l)
{
  clint t_l[2 + (CLINTMAXDIGIT << 1)];
  clint *tptr_l, *nptr_l, *tiptr_l, *lasttnptr, *lastnptr;
  ULONG carry;
  USHORT mi;
  int i;

  mult (a_l, b_l, t_l);
  Assert (DIGITS_L (t_l) <= (1 + (CLINTMAXDIGIT << 1)));

  lasttnptr = t_l + DIGITS_L (n_l);
  lastnptr = MSDPTR_L (n_l);

  for (i = (int)DIGITS_L (t_l) + 1; i <= (int)(DIGITS_L (n_l) << 1); i++)
    {
      Assert (i < sizeof (t_l));
      t_l[i] = 0;
    }

  SETDIGITS_L (t_l, MAX(DIGITS_L (t_l), DIGITS_L (n_l) << 1));

  Assert (DIGITS_L (t_l) <= (CLINTMAXDIGIT << 1));

  for (tptr_l = LSDPTR_L (t_l); tptr_l <= lasttnptr; tptr_l++)
    {
      carry = 0;
      mi = (USHORT)((ULONG)nprime * (ULONG)*tptr_l);
      for (nptr_l = LSDPTR_L (n_l), tiptr_l = tptr_l; nptr_l <= lastnptr; nptr_l++, tiptr_l++)
        {
          Assert (tiptr_l <= t_l + (CLINTMAXDIGIT << 1));
          *tiptr_l = (USHORT)(carry = (ULONG)mi * (ULONG)*nptr_l +
                     (ULONG)*tiptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
        }

      for (; ((carry >> BITPERDGT) > 0) && tiptr_l <= MSDPTR_L (t_l); tiptr_l++)
        {
          Assert (tiptr_l <= t_l + (CLINTMAXDIGIT << 1));
          *tiptr_l = (USHORT)(carry = (ULONG)*tiptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
        }

      if (((carry >> BITPERDGT) > 0))
        {
          Assert (tiptr_l <= t_l + 1 + (CLINTMAXDIGIT << 1));
          *tiptr_l = (USHORT)(carry >> BITPERDGT);
          INCDIGITS_L (t_l);
        }
    }

  tptr_l = t_l + logB_r;
  SETDIGITS_L (tptr_l, DIGITS_L (t_l) - logB_r);
  Assert (DIGITS_L (tptr_l) <= (CLINTMAXDIGIT + 1));

  if (GE_L (tptr_l, n_l))
    {
      sub_l (tptr_l, n_l, p_l);
    }
  else
    {
      cpy_l (p_l, tptr_l);
    }

  Assert (DIGITS_L (p_l) <= CLINTMAXDIGIT);

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (mi), &mi,
                   sizeof (carry), &carry,
                   sizeof (t_l), t_l));

  ISPURGED_L  ((3, sizeof (mi), &mi,
                   sizeof (carry), &carry,
                   sizeof (t_l), t_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Montgomery squaring                                            */
/*  Syntax:    void sqrmon_l (CLINT a_l, CLINT n_l, USHORT nprime,            */
/*                                                 USHORT logB_r, CLINT p_l); */
/*  Input:     a_l (factor),  n_l (Modulus, odd)                              */
/*             nprime (n' mod B),                                             */
/*             logB_r (Integral Part of Logarithm of r to base B)             */
/*             (For an explanation of the operands cf. Chap. 6)               */
/*  Output:    p_l (Remainder a_l * a_l * r^(-1) mod n_l)                     */
/*             with r := B^logB_r, B^(logB_r-1) <= n_l < B^logB_r)            */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sqrmon_l(CLINT a_l, CLINT n_l, USHORT nprime, USHORT logB_r, CLINT p_l)
{
  clint t_l[2 + (CLINTMAXDIGIT << 1)];
  clint *tptr_l, *nptr_l, *tiptr_l, *lasttnptr, *lastnptr;
  ULONG carry;
  USHORT mi;
  int i;

  sqr (a_l, t_l);

  lasttnptr = t_l + DIGITS_L (n_l);
  lastnptr = MSDPTR_L (n_l);

  for (i = (int)DIGITS_L (t_l) + 1; i <= (int)(DIGITS_L (n_l) << 1); i++)
    {
      t_l[i] = 0;
    }

  SETDIGITS_L (t_l, MAX(DIGITS_L (t_l), DIGITS_L (n_l) << 1));

  for (tptr_l = LSDPTR_L (t_l); tptr_l <= lasttnptr; tptr_l++)
    {
      carry = 0;
      mi = (USHORT)((ULONG)nprime * (ULONG)*tptr_l);
      for (nptr_l = LSDPTR_L (n_l), tiptr_l = tptr_l; nptr_l <= lastnptr; nptr_l++, tiptr_l++)
        {
          Assert (tiptr_l <= t_l + (CLINTMAXDIGIT << 1));
          *tiptr_l = (USHORT)(carry = (ULONG)mi * (ULONG)*nptr_l +
                     (ULONG)*tiptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
        }

      for (; ((carry >> BITPERDGT) > 0) && tiptr_l <= MSDPTR_L (t_l); tiptr_l++)
        {
          Assert (tiptr_l <= t_l + (CLINTMAXDIGIT << 1));
          *tiptr_l = (USHORT)(carry = (ULONG)*tiptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
        }

      if (((carry >> BITPERDGT) > 0) && tiptr_l > MSDPTR_L (t_l))
        {
          Assert (tiptr_l <= t_l + 1 + (CLINTMAXDIGIT << 1));
          *tiptr_l = (USHORT)(carry >> BITPERDGT);
          INCDIGITS_L (t_l);
        }
    }

  tptr_l = t_l + logB_r;
  SETDIGITS_L (tptr_l, DIGITS_L (t_l) - logB_r);

  if (GE_L (tptr_l, n_l))
    {
      sub_l (tptr_l, n_l, p_l);
    }
  else
    {
      cpy_l (p_l, tptr_l);
    }

  Assert (DIGITS_L (p_l) <= CLINTMAXDIGIT);

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (mi), &mi,
                   sizeof (carry), &carry,
                   sizeof (t_l), t_l));

  ISPURGED_L ((3,  sizeof (mi), &mi,
                   sizeof (carry), &carry,
                   sizeof (t_l), t_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Inverse -n^(-1) mod B for odd n                                */
/*  Syntax:    USHORT invmon_l (CLINT n_l);                                   */
/*  Input:     n_l (Modulus)                                                  */
/*  Output:    -                                                              */
/*  Returns:   -n^(-1) mod B                                                  */
/*                                                                            */
/******************************************************************************/
USHORT __FLINT_API
invmon_l (CLINT n_l)
{
  unsigned int i;
  ULONG x = 2, y = 1;

  if (ISEVEN_L (n_l))
    {
      return (USHORT)E_CLINT_MOD;
    }

  for (i = 2; i <= BITPERDGT; i++, x <<= 1)
    {
      if (x < (((ULONG)((ULONG)(*LSDPTR_L (n_l)) * (ULONG)y)) & ((x << 1) - 1)))
        {
          y += x;
        }
    }

  return (USHORT)(x - y);
}


/******************************************************************************/

static int twotab[] =
{0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0,
 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0,
 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0,
 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};


static USHORT oddtab[] =
{0, 1, 1, 3, 1, 5, 3, 7, 1, 9, 5, 11, 3, 13, 7, 15, 1, 17, 9, 19, 5, 21, 11, 23, 3, 25, 13, 27, 7, 29, 15, 31, 1,
 33, 17, 35, 9, 37, 19, 39, 5, 41, 21, 43, 11, 45, 23, 47, 3, 49, 25, 51, 13, 53, 27, 55, 7, 57, 29, 59, 15,
 61, 31, 63, 1, 65, 33, 67, 17, 69, 35, 71, 9, 73, 37, 75, 19, 77, 39, 79, 5, 81, 41, 83, 21, 85, 43, 87, 11,
 89, 45, 91, 23, 93, 47, 95, 3, 97, 49, 99, 25, 101, 51, 103, 13, 105, 53, 107, 27, 109, 55, 111, 7, 113,
 57, 115, 29, 117, 59, 119, 15, 121, 61, 123, 31, 125, 63, 127, 1, 129, 65, 131, 33, 133, 67, 135, 17,
 137, 69, 139, 35, 141, 71, 143, 9, 145, 73, 147, 37, 149, 75, 151, 19, 153, 77, 155, 39, 157, 79, 159,
 5, 161, 81, 163, 41, 165, 83, 167, 21, 169, 85, 171, 43, 173, 87, 175, 11, 177, 89, 179, 45, 181, 91,
 183, 23, 185, 93, 187, 47, 189, 95, 191, 3, 193, 97, 195, 49, 197, 99, 199, 25, 201, 101, 203, 51, 205,
 103, 207, 13, 209, 105, 211, 53, 213, 107, 215, 27, 217, 109, 219, 55, 221, 111, 223, 7, 225, 113,
 227, 57, 229, 115, 231, 29, 233, 117, 235, 59, 237, 119, 239, 15, 241, 121, 243, 61, 245, 123, 247, 31,
 249, 125, 251, 63, 253, 127, 255};


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular exponentiation                                         */
/*             Automatic application of Montgomery exponentiation mexpkm_l    */
/*             if modulus is even, else mexpk_l is used                       */
/*  Syntax:    int mexp_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l);   */
/*  Input:     bas_l (Base), exp_l (Exponent), m_l (Modulus)                  */
/*  Output:    p_l (Remainder of bas_l^exp_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mexp_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l)
{
  if (ISODD_L (m_l))              /* Montgomery exponentiation possible */
    {
      return mexpkm_l (bas_l, exp_l, p_l, m_l);
    }
  else
    {
      return mexpk_l (bas_l, exp_l, p_l, m_l);
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular Exponentiation                                         */
/*             with representation of exponent to base 2^5                    */
/*  Syntax:    int mexp5_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l);  */
/*  Input:     bas_l (Base), exp_l (Exponent), m_l (Modulus)                  */
/*  Output:    p_l (Remainder of bas_l^exp_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mexp5_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l)
{
  CLINT a_l;
  clint e_l[CLINTMAXSHORT + 1];
  CLINTD acc_l;
  CLINT a2_l, a3_l, a5_l, a7_l, a9_l, a11_l, a13_l, a15_l, a17_l, a19_l,
      a21_l, a23_l, a25_l, a27_l, a29_l, a31_l;
  clint *aptr_l[32];
  int i, noofdigits, s, t;
  unsigned int bit, digit, f5, word;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (p_l);             /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  cpy_l (a_l, bas_l);
  cpy_l (e_l, exp_l);

  if (DIGITS_L (e_l) == 0)
    {
      SETONE_L (p_l);

      PURGEVARS_L ((1, sizeof (a_l), a_l));
      ISPURGED_L ((1, sizeof (a_l), a_l));

      return E_CLINT_OK;
    }

  if (DIGITS_L (a_l) == 0)
    {
      SETZERO_L (p_l);

      PURGEVARS_L ((1, sizeof (e_l), e_l));
      ISPURGED_L ((1, sizeof (e_l), e_l));

      return E_CLINT_OK;
    }

  mod_l (a_l, m_l, a_l);

  aptr_l[1] = a_l;
  aptr_l[3] = a3_l;
  aptr_l[5] = a5_l;
  aptr_l[7] = a7_l;
  aptr_l[9] = a9_l;
  aptr_l[11] = a11_l;
  aptr_l[13] = a13_l;
  aptr_l[15] = a15_l;
  aptr_l[17] = a17_l;
  aptr_l[19] = a19_l;
  aptr_l[21] = a21_l;
  aptr_l[23] = a23_l;
  aptr_l[25] = a25_l;
  aptr_l[27] = a27_l;
  aptr_l[29] = a29_l;
  aptr_l[31] = a31_l;

  msqr_l (a_l, a2_l, m_l);
  for (i = 3; i <= 31; i += 2)
    {
      mmul_l (a2_l, aptr_l[i - 2], aptr_l[i], m_l);
    }

  *(MSDPTR_L (e_l) + 1) = 0;    /* Zero follows most significant digit of e_l */

  noofdigits = (ld_l (e_l) - 1)/5;                               /*lint !e713 */
  f5 = (unsigned int)(noofdigits * 5);  /* >>loss of precision<< not critical */

      word = (unsigned int)(f5 >> LDBITPERDGT);     /* f5 div 16 */
      bit = (unsigned int)(f5 & (BITPERDGT - 1U));  /* f5 mod 16 */

      digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2] << BITPERDGT)) >> bit) & 0x1f;

  if (digit != 0)                  /* 5-digit > 0 */
    {
      cpy_l (acc_l, aptr_l[oddtab[digit]]);

      t = twotab[digit];
      for (; t > 0; t--)
        {
          msqr_l (acc_l, acc_l, m_l);
        }
    }
  else
    {
      SETONE_L (acc_l);
    }

  for (noofdigits--, f5 -= 5; noofdigits >= 0; noofdigits--, f5 -= 5)
    {
      word = (unsigned int)f5 >> LDBITPERDGT;    /* f5 div 16 */
      bit = f5 & (BITPERDGT - 1UL);              /* f5 mod 16 */

      digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2] << BITPERDGT)) >> bit) & 0x1f;

      if (digit != 0)             /* 5-digit > 0 */
        {
          t = twotab[digit];

          for (s = 5 - t; s > 0; s--)
            {
              msqr_l (acc_l, acc_l, m_l);
            }

          mmul_l (acc_l, aptr_l[oddtab[digit]], acc_l, m_l);

          for (; t > 0; t--)
            {
              msqr_l (acc_l, acc_l, m_l);
            }
        }
      else                        /* 5-digit > 0 */
        {
          for (s = 5; s > 0; s--)
            {
              msqr_l (acc_l, acc_l, m_l);
            }
        }
    }

  cpy_l (p_l, acc_l);

  /* Purging of variables */
  PURGEVARS_L ((8,  sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (f5), &f5,
                    sizeof (word), &word));
  PURGEVARS_L ((19, sizeof (acc_l), acc_l,
                    sizeof (a_l), a_l,
                    sizeof (e_l), e_l,
                    sizeof (a2_l), a2_l,
                    sizeof (a3_l), a3_l,
                    sizeof (a5_l), a5_l,
                    sizeof (a7_l), a7_l,
                    sizeof (a9_l), a9_l,
                    sizeof (a11_l), a11_l,
                    sizeof (a13_l), a13_l,
                    sizeof (a15_l), a15_l,
                    sizeof (a17_l), a17_l,
                    sizeof (a19_l), a19_l,
                    sizeof (a21_l), a21_l,
                    sizeof (a23_l), a23_l,
                    sizeof (a25_l), a25_l,
                    sizeof (a27_l), a27_l,
                    sizeof (a29_l), a29_l,
                    sizeof (a31_l), a31_l));

  ISPURGED_L  ((8,  sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (f5), &f5,
		    sizeof (word), &word));
ISPURGED_L  ((19,   sizeof (acc_l), acc_l,
                    sizeof (a_l), a_l,
                    sizeof (e_l), e_l,
                    sizeof (a2_l), a2_l,
                    sizeof (a3_l), a3_l,
                    sizeof (a5_l), a5_l,
                    sizeof (a7_l), a7_l,
                    sizeof (a9_l), a9_l,
                    sizeof (a11_l), a11_l,
                    sizeof (a13_l), a13_l,
                    sizeof (a15_l), a15_l,
                    sizeof (a17_l), a17_l,
                    sizeof (a19_l), a19_l,
                    sizeof (a21_l), a21_l,
                    sizeof (a23_l), a23_l,
                    sizeof (a25_l), a25_l,
                    sizeof (a27_l), a27_l,
                    sizeof (a29_l), a29_l,
                    sizeof (a31_l), a31_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular Exponentiation                                         */
/*             with representation of exponent to base 2^k                    */
/*  Syntax:    int mexpk_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l);  */
/*  Input:     bas_l (Base), exp_l (Exponent), m_l (Modulus)                  */
/*  Output:    p_l (Remainder of bas_l^exp_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*             E_CLINT_MAL: Error with malloc()                               */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mexpk_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l)
{
  CLINT a_l, a2_l;
  clint e_l[CLINTMAXSHORT + 1];
  CLINTD acc_l;
  clint **aptr_l, *ptr_l = NULL;
  int noofdigits, s, t, i;
  unsigned int k, lge, bit, digit, fk, word, pow2k, k_mask;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (p_l);             /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  cpy_l (a_l, bas_l);
  cpy_l (e_l, exp_l);

  if (DIGITS_L (e_l) == 0)
    {
      SETONE_L (p_l);

      PURGEVARS_L ((1, sizeof (a_l), a_l));
      ISPURGED_L  ((1, sizeof (a_l), a_l));

      return E_CLINT_OK;
    }

  if (DIGITS_L (a_l) == 0)
    {
      SETZERO_L (p_l);

      PURGEVARS_L ((1, sizeof (e_l), e_l));
      ISPURGED_L  ((1, sizeof (e_l), e_l));

      return E_CLINT_OK;
    }

  lge = ld_l (e_l);

  k = 8;

  while (k > 1 && ((k - 1) * (k << ((k - 1) << 1)) / ((1 << k) - k - 1)) >= lge - 1)
    {
      --k;
    }

  pow2k = 1U << k;                 /*lint !e644*/

#if defined FLINT_DEBUG && defined FLINT_VERBOSE
  printf ("ld(e) = %d, k = %ld, pow2k = %u\n", lge, k, pow2k);
#endif

  k_mask = pow2k - 1U;

  if ((aptr_l = (clint **)malloc (sizeof (clint *) * pow2k)) == NULL)
    {
      PURGEVARS_L ((2, sizeof (a_l), a_l,
                       sizeof (e_l), e_l));
      ISPURGED_L  ((2, sizeof (a_l), a_l,
                       sizeof (e_l), e_l));
      return E_CLINT_MAL;
    }

  mod_l (a_l, m_l, a_l);
  aptr_l[1] = a_l;

  if (k > 1)
    {
      if ((ptr_l = (clint *)malloc (sizeof (CLINT) * ((pow2k >> 1) - 1))) == NULL)
        {
          free (aptr_l);
          PURGEVARS_L ((2, sizeof (a_l), a_l,
                           sizeof (e_l), e_l));
          ISPURGED_L  ((2, sizeof (a_l), a_l,
                           sizeof (e_l), e_l));
          return E_CLINT_MAL;
        }
      aptr_l[2] = a2_l;
      msqr_l (a_l, aptr_l[2], m_l);

      for (aptr_l[3] = ptr_l, i = 5; i < (int)pow2k; i += 2)
        {
          aptr_l[i] = aptr_l[i - 2] + CLINTMAXSHORT;   /*lint !e661 !e662 */
        }

      for (i = 3; i < (int)pow2k; i += 2)
        {
          mmul_l (aptr_l[2], aptr_l[i - 2], aptr_l[i], m_l);
        }
    }

  *(MSDPTR_L (e_l) + 1) = 0;    /* 0 follows most significant digit of e_l */

  noofdigits = (lge - 1)/k;                                   /*lint !e713 */
  fk = noofdigits * k;              /*  >>loss of precision<< not critical */

  word = (unsigned int)(fk >> LDBITPERDGT);         /* fk div 16 */
  bit = (unsigned int)(fk & (BITPERDGT - 1UL));     /* fk mod 16 */

  switch (k)
    {
      case 1:
      case 2:
      case 4:
      case 8:
        digit = ((ULONG)(e_l[word + 1]) >> bit) & k_mask;
        break;
      default:
        digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2]
                                 << BITPERDGT)) >> bit) & k_mask;
    }

  if (digit != 0)                  /* k-digit > 0 */
    {
      cpy_l (acc_l, aptr_l[oddtab[digit]]);

      t = twotab[digit];
      for (; t > 0; t--)
        {
          msqr_l (acc_l, acc_l, m_l);
        }
    }
  else
    {
      SETONE_L (acc_l);
    }

  for (noofdigits--, fk -= k; noofdigits >= 0; noofdigits--, fk -= k)
    {
      word = (unsigned int)(fk >> LDBITPERDGT);       /* fk div 16 */
      bit = (unsigned int)(fk & (BITPERDGT - 1UL));   /* fk mod 16 */

      switch (k)
        {
          case 1:
          case 2:
          case 4:
          case 8:
            digit = ((ULONG)(e_l[word + 1]) >> bit) & k_mask;
            break;
          default:
            digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2]
                                     << BITPERDGT)) >> bit) & k_mask;
        }

      if (digit != 0)              /* k-digit > 0 */
        {
          t = twotab[digit];

          for (s = (int)(k - t); s > 0; s--)
            {
              msqr_l (acc_l, acc_l, m_l);
            }

          mmul_l (acc_l, aptr_l[oddtab[digit]], acc_l, m_l);

          for (; t > 0; t--)
            {
              msqr_l (acc_l, acc_l, m_l);
            }
        }
      else                         /* k-digit == 0 */
        {
          for (s = (int)k; s > 0; s--)
            {
              msqr_l (acc_l, acc_l, m_l);
            }
        }
    }

  cpy_l (p_l, acc_l);

  free (aptr_l);
  if (ptr_l != NULL)
    {

#ifdef FLINT_SECURE
      memset (ptr_l, 0, sizeof (CLINT) * ((pow2k >> 1) - 1));   /*lint !e668*/
#endif

      free (ptr_l);                /*lint !e644 */
    }

  /* Purging of variables */
  PURGEVARS_L ((12, sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (k), &k,
                    sizeof (lge), &lge,
                    sizeof (fk), &fk,
                    sizeof (word), &word,
                    sizeof (pow2k), &pow2k,
                    sizeof (k_mask), &k_mask));
  PURGEVARS_L ((4,  sizeof (a_l), a_l,
                    sizeof (a2_l), a2_l,
                    sizeof (e_l), e_l,
                    sizeof (acc_l), acc_l));

  ISPURGED_L  ((16, sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (k), &k,
                    sizeof (lge), &lge,
                    sizeof (fk), &fk,
                    sizeof (word), &word,
                    sizeof (pow2k), &pow2k,
                    sizeof (k_mask), &k_mask,
                    sizeof (a_l), a_l,
                    sizeof (a2_l), a2_l,
                    sizeof (e_l), e_l,
                    sizeof (acc_l), acc_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular Exponentiation                                         */
/*             with representation of exponent to base 2^5                    */
/*             for odd moduli, with Montgomery reduction                      */
/*  Syntax:    int mexp5m_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l); */
/*  Input:     bas_l (Base), exp_l (Exponent), m_l (Modulus)                  */
/*  Output:    p_l (Remainder of bas_l^exp_l mod m_l)                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*             E_CLINT_MOD: Modulus even                                      */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mexp5m_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l)
{
  CLINT a_l, md_l;
  clint e_l[CLINTMAXSHORT + 1];
  clint r_l[CLINTMAXSHORT + 1];
  CLINTD acc_l;
  CLINT a2_l, a3_l, a5_l, a7_l, a9_l, a11_l, a13_l, a15_l, a17_l, a19_l,
      a21_l, a23_l, a25_l, a27_l, a29_l, a31_l;
  clint *aptr_l[32];
  int i, noofdigits, s, t;
  unsigned int bit, digit, f5, word;
  USHORT logB_r, mprime;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (ISEVEN_L (m_l))
    {
      return E_CLINT_MOD;          /* Modulus is even */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (p_l);             /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  cpy_l (a_l, bas_l);
  cpy_l (e_l, exp_l);
  cpy_l (md_l, m_l);

  if (DIGITS_L (e_l) == 0)
    {
      SETONE_L (p_l);

      PURGEVARS_L ((2, sizeof (a_l), a_l,
                       sizeof (md_l), md_l));
      ISPURGED_L  ((2, sizeof (a_l), a_l,
                       sizeof (md_l), md_l));
      return E_CLINT_OK;
    }

  if (DIGITS_L (a_l) == 0)
    {
      SETZERO_L (p_l);

      PURGEVARS_L ((2, sizeof (e_l), e_l,
                       sizeof (md_l), md_l));
      ISPURGED_L  ((2, sizeof (e_l), e_l,
                       sizeof (md_l), md_l));
      return E_CLINT_OK;
    }

  aptr_l[1] = a_l;
  aptr_l[3] = a3_l;
  aptr_l[5] = a5_l;
  aptr_l[7] = a7_l;
  aptr_l[9] = a9_l;
  aptr_l[11] = a11_l;
  aptr_l[13] = a13_l;
  aptr_l[15] = a15_l;
  aptr_l[17] = a17_l;
  aptr_l[19] = a19_l;
  aptr_l[21] = a21_l;
  aptr_l[23] = a23_l;
  aptr_l[25] = a25_l;
  aptr_l[27] = a27_l;
  aptr_l[29] = a29_l;
  aptr_l[31] = a31_l;

  SETZERO_L (r_l);
  logB_r = DIGITS_L (md_l);
  setbit (r_l, logB_r << LDBITPERDGT);
  if (DIGITS_L (r_l) > CLINTMAXDIGIT)
    {
      mod_l (r_l, md_l, r_l);
    }

  mprime = invmon_l (md_l);

  mmul_l (a_l, r_l, a_l, md_l);

  sqrmon_l (a_l, md_l, mprime, logB_r, a2_l);

  for (i = 3; i <= 31; i += 2)
    {
      mulmon_l (a2_l, aptr_l[i - 2], md_l, mprime, logB_r, aptr_l[i]);
    }

  *(MSDPTR_L (e_l) + 1) = 0;      /* 0 follows most significant digit of e_l */

  noofdigits = (ld_l (e_l) - 1)/5;                              /*lint !e713 */
  f5 = (unsigned int)(noofdigits * 5); /* >>loss of precision<< not critical */

  word = (unsigned int)(f5 >> LDBITPERDGT);         /* f5 div 16 */
  bit = (unsigned int)(f5 & (BITPERDGT - 1UL));     /* f5 mod 16 */

  digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2] << BITPERDGT)) >> bit) & 0x1f;

  if (digit != 0)                  /* 5-digit > 0 */
    {
      cpy_l (acc_l, aptr_l[oddtab[digit]]);

      t = twotab[digit];
      for (; t > 0; t--)
        {
          sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
        }
    }
  else
    {
      mod_l (r_l, md_l, acc_l);
    }

  for (noofdigits--, f5 -= 5; noofdigits >= 0; noofdigits--, f5 -= 5)
    {
      word = (unsigned int)f5 >> LDBITPERDGT;       /* f5 div 16 */
      bit = (unsigned int)f5 & (BITPERDGT - 1UL);   /* f5 mod 16 */

      digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2] << BITPERDGT)) >> bit) & 0x1f;

      if (digit != 0)              /* 5-digit == 0 */
        {
          t = twotab[digit];
          for (s = 5 - t; s > 0; s--)
            {
              sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
            }

          mulmon_l (acc_l, aptr_l[oddtab[digit]], md_l, mprime, logB_r, acc_l);

          for (; t > 0; t--)
            {
              sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
            }
        }
      else                         /* 5-digit == 0 */
        {
          for (s = 5; s > 0; s--)
            {
              sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
            }
        }
    }

  mulmon_l (acc_l, one_l, md_l, mprime, logB_r, p_l);

  /* Purging of variables */
  PURGEVARS_L ((8,  sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (f5), &f5,
                    sizeof (word), &word));
  PURGEVARS_L ((21, sizeof (acc_l), acc_l,
                    sizeof (a_l), a_l,
                    sizeof (md_l), md_l,
                    sizeof (e_l), e_l,
                    sizeof (r_l), r_l,
                    sizeof (a2_l), a2_l,
                    sizeof (a3_l), a3_l,
                    sizeof (a5_l), a5_l,
                    sizeof (a7_l), a7_l,
                    sizeof (a9_l), a9_l,
                    sizeof (a11_l), a11_l,
                    sizeof (a13_l), a13_l,
                    sizeof (a15_l), a15_l,
                    sizeof (a17_l), a17_l,
                    sizeof (a19_l), a19_l,
                    sizeof (a21_l), a21_l,
                    sizeof (a23_l), a23_l,
                    sizeof (a25_l), a25_l,
                    sizeof (a27_l), a27_l,
                    sizeof (a29_l), a29_l,
                    sizeof (a31_l), a31_l));

  ISPURGED_L  ((8,  sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (f5), &f5,
		    sizeof (word), &word));
  ISPURGED_L  ((21, sizeof (acc_l), acc_l,
                    sizeof (a_l), a_l,
                    sizeof (md_l), md_l,
                    sizeof (e_l), e_l,
                    sizeof (r_l), r_l,
                    sizeof (a2_l), a2_l,
                    sizeof (a3_l), a3_l,
                    sizeof (a5_l), a5_l,
                    sizeof (a7_l), a7_l,
                    sizeof (a9_l), a9_l,
                    sizeof (a11_l), a11_l,
                    sizeof (a13_l), a13_l,
                    sizeof (a15_l), a15_l,
                    sizeof (a17_l), a17_l,
                    sizeof (a19_l), a19_l,
                    sizeof (a21_l), a21_l,
                    sizeof (a23_l), a23_l,
                    sizeof (a25_l), a25_l,
                    sizeof (a27_l), a27_l,
                    sizeof (a29_l), a29_l,
                    sizeof (a31_l), a31_l));
  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular Exponentiation for odd moduli (Montgomery reduction)   */
/*  Syntax:    int mexpkm_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l); */
/*  Input:     bas_l (Base), exp_l (Exponent), m_l (Modulus )                 */
/*  Output:    p_l (Remainder of bas_l ^ exp_l mod m_l)                       */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*             E_CLINT_MAL: Error with malloc()                               */
/*             E_CLINT_MOD: Modulus even                                      */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mexpkm_l (CLINT bas_l, CLINT exp_l, CLINT p_l, CLINT m_l)
{
  CLINT a_l, a2_l, md_l;
  clint e_l[CLINTMAXSHORT + 1];
  clint r_l[CLINTMAXSHORT + 1];
  CLINTD acc_l;
  clint **aptr_l, *ptr_l = NULL;
  int noofdigits, s, t, i;
  unsigned int k, lge, bit, digit, fk, word, pow2k, k_mask;
  USHORT logB_r, mprime;

#ifdef FLINT_DEBUG
  int sign_rmin1, sign_mprime;
  CLINTD d_l, mprime_l, rmin1_l;
#endif

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (ISEVEN_L (m_l))
    {
      return E_CLINT_MOD;          /* Modulus even */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (p_l);             /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  cpy_l (a_l, bas_l);
  cpy_l (e_l, exp_l);
  cpy_l (md_l, m_l);

  if (DIGITS_L (e_l) == 0)
    {
      SETONE_L (p_l);
      PURGEVARS_L ((2, sizeof (a_l), a_l,
                       sizeof (md_l), md_l));
      ISPURGED_L  ((2, sizeof (a_l), a_l,
                       sizeof (md_l), md_l));
      return E_CLINT_OK;
    }

  if (DIGITS_L (a_l) == 0)
    {
      SETZERO_L (p_l);

      PURGEVARS_L ((2, sizeof (e_l), e_l,
                       sizeof (md_l), md_l));
      ISPURGED_L  ((2, sizeof (e_l), e_l,
                       sizeof (md_l), md_l));
      return E_CLINT_OK;
    }

  lge = ld_l (e_l);

  k = 8;

  while (k > 1 && ((k - 1) * (k << ((k - 1) << 1)) / ((1 << k) - k - 1)) >= lge - 1)
    {
      --k;
    }

  pow2k = 1U << k;                 /*lint !e644 */

#if defined FLINT_DEBUG && defined FLINT_VERBOSE
  printf ("ld(e) = %d, k = %ld, pow2k = %u\n", lge, k, pow2k);
#endif

  k_mask = pow2k - 1;

  if ((aptr_l = (clint **)malloc (sizeof (clint *) * pow2k)) == NULL)
    {
      PURGEVARS_L ((3, sizeof (a_l), a_l,
                       sizeof (e_l), e_l,
                       sizeof (md_l), md_l));
      ISPURGED_L  ((3, sizeof (a_l), a_l,
                       sizeof (e_l), e_l,
                       sizeof (md_l), md_l));
      return E_CLINT_MAL;
    }

  aptr_l[1] = a_l;
  SETZERO_L (r_l);
  logB_r = DIGITS_L (md_l);
  setbit (r_l, logB_r << LDBITPERDGT);
  if (DIGITS_L (r_l) > CLINTMAXDIGIT)
    {
      mod_l (r_l, md_l, r_l);
    }

  mprime = invmon_l (md_l);

#ifdef FLINT_DEBUG
  if (logB_r < CLINTMAXDIGIT)
    {
      xgcd_l (r_l, md_l, d_l, rmin1_l, &sign_rmin1, mprime_l, &sign_mprime);
      if (sign_mprime > 0)
      {
        msub_l (r_l, mprime_l, mprime_l, r_l);
      }

      Assert(EQONE_L (d_l));
      Assert(*LSDPTR_L (mprime_l) == mprime);
    }
#endif /* FLINT_DEBUG */

  mmul_l (a_l, r_l, a_l, md_l);

  if (k > 1)
    {
      if ((ptr_l = (clint *)malloc (sizeof (CLINT) * ((pow2k >> 1) - 1))) == NULL)
        {
          free (aptr_l);
          PURGEVARS_L ((3, sizeof (a_l), a_l,
                           sizeof (e_l), e_l,
                           sizeof (md_l), md_l));
          ISPURGED_L  ((3, sizeof (a_l), a_l,
                           sizeof (e_l), e_l,
                           sizeof (md_l), md_l));
          return E_CLINT_MAL;
        }

      aptr_l[2] = a2_l;
      sqrmon_l (a_l, md_l, mprime, logB_r, aptr_l[2]);

      for (aptr_l[3] = ptr_l, i = 5; i < (int)pow2k; i += 2)
        {
          aptr_l[i] = aptr_l[i - 2] + CLINTMAXSHORT;   /*lint !e661 !e662 */
        }

      for (i = 3; i < (int)pow2k; i += 2)
        {
          mulmon_l (aptr_l[2], aptr_l[i - 2], md_l, mprime, logB_r, aptr_l[i]);
        }
    }

  *(MSDPTR_L (e_l) + 1) = 0;     /* 0 follows most significant digit of e_l */

  noofdigits = (lge - 1)/k;                                    /*lint !e713 */
  fk = noofdigits * k;                /* >>loss of precision<< not critical */

  word = (unsigned int)(fk >> LDBITPERDGT);        /* fk div 16 */
  bit = (unsigned int)(fk & (BITPERDGT - 1UL));    /* fk mod 16 */

  switch (k)
    {
      case 1:
      case 2:
      case 4:
      case 8:
        digit = ((ULONG)(e_l[word + 1]) >> bit) & k_mask;
        break;
      default:
        digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2]
                                 << BITPERDGT)) >> bit) & k_mask;
    }

  if (digit != 0)                  /* k-digit > 0 */
    {
      cpy_l (acc_l, aptr_l[oddtab[digit]]);

      t = twotab[digit];
      for (; t > 0; t--)
        {
          sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
        }
    }
  else
    {
      mod_l (r_l, md_l, acc_l);
    }

  for (noofdigits--, fk -= k; noofdigits >= 0; noofdigits--, fk -= k)
    {
      word = (unsigned int)fk >> LDBITPERDGT;       /* fk div 16 */
      bit = (unsigned int)fk & (BITPERDGT - 1UL);   /* fk mod 16 */

      switch (k)
        {
          case 1:
          case 2:
          case 4:
          case 8:
            digit = ((ULONG)(e_l[word + 1]) >> bit) & k_mask;
            break;
          default:
            digit = ((ULONG)(e_l[word + 1] | ((ULONG)e_l[word + 2]
                                     << BITPERDGT)) >> bit) & k_mask;
        }

      if (digit != 0)              /* k-digit > 0 */
        {
          t = twotab[digit];

          for (s = (int)(k - t); s > 0; s--)
            {
              sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
            }

          mulmon_l (acc_l, aptr_l[oddtab[digit]], md_l, mprime, logB_r, acc_l);

          for (; t > 0; t--)
            {
              sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
            }
        }
      else                         /* k-digit == 0 */
        {
          for (s = (int)k; s > 0; s--)
            {
              sqrmon_l (acc_l, md_l, mprime, logB_r, acc_l);
            }
        }
    }

  mulmon_l (acc_l, one_l, md_l, mprime, logB_r, p_l);

#ifdef FLINT_SECURE
  memset (aptr_l, 0, sizeof (clint *) * pow2k);
#endif

  free (aptr_l);
  if (ptr_l != NULL)
    {

#ifdef FLINT_SECURE
      memset (ptr_l, 0, sizeof (CLINT) * ((pow2k >> 1) - 1));   /*lint !e668*/
#endif

      free (ptr_l);                /*lint !e644 */
    }

  /* Purging of variables */
  PURGEVARS_L ((14, sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (k), &k,
                    sizeof (lge), &lge,
                    sizeof (fk), &fk,
                    sizeof (word), &word,
                    sizeof (pow2k), &pow2k,
                    sizeof (k_mask), &k_mask,
                    sizeof (logB_r), &logB_r,
                    sizeof (mprime), &mprime));
  PURGEVARS_L ((6,  sizeof (a_l), a_l,
                    sizeof (a2_l), a2_l,
                    sizeof (e_l), e_l,
                    sizeof (r_l), r_l,
                    sizeof (acc_l), acc_l,
                    sizeof (md_l), md_l));

  ISPURGED_L  ((20, sizeof (i), &i,
                    sizeof (noofdigits), &noofdigits,
                    sizeof (s), &s,
                    sizeof (t), &t,
                    sizeof (bit), &bit,
                    sizeof (digit), &digit,
                    sizeof (k), &k,
                    sizeof (lge), &lge,
                    sizeof (fk), &fk,
                    sizeof (word), &word,
                    sizeof (pow2k), &pow2k,
                    sizeof (k_mask), &k_mask,
                    sizeof (logB_r), &logB_r,
                    sizeof (mprime), &mprime,
                    sizeof (a_l), a_l,
                    sizeof (a2_l), a2_l,
                    sizeof (e_l), e_l,
                    sizeof (r_l), r_l,
                    sizeof (acc_l), acc_l,
                    sizeof (md_l), md_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular exponentiation with exponent 2^k                       */
/*  Syntax:    int mexp2_l (CLINT a_l, USHORT k, CLINT p_l, CLINT m_l);       */
/*  Input:     a_l (Basis), k (Exponent of exponent 2^k)                      */
/*             m_l (Modulus)                                                  */
/*  Output:    p_l (Remainder of a_l ^(2^k) mod m_l)                          */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
mexp2_l (CLINT a_l, USHORT k, CLINT p_l, CLINT m_l)
{
  CLINT tmp_l;
  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (p_l);             /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  if (k > 0)
    {
      cpy_l (tmp_l, a_l);
      while (k-- > 0)
        {
          msqr_l (tmp_l, tmp_l, m_l);
        }
      cpy_l (p_l, tmp_l);
    }
  else
    {
      mod_l (a_l, m_l, p_l);
    }

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
  ISPURGED_L  ((1, sizeof (tmp_l), tmp_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/* Arithmetic interface with CLINT and USHORT types (mixed functions)         */
/*                                                                            */
/* Functions:                                                                 */
/*      wmexp_l, wmexpm_l                                                     */
/*                                                                            */
/* The first argument is of type USHORT                                       */
/*                                                                            */
/* Functions:                                                                 */
/*      uadd_l, umadd_l, usub_l, umsub_l, umul_l, ummul_l, udiv_l, umod_l     */
/*      umexp_l, umexpm_l                                                     */
/*                                                                            */
/* The second argument is of type USHORT                                      */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*  Function:  Addition of CLINT and USHORT type                              */
/*  Syntax:    int uadd_l (CLINT a_l, USHORT b, CLINT s_l);                   */
/*  Input:     a_l, b (Operands)                                              */
/*  Output:    s_l (Sum)                                                      */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
uadd_l (CLINT a_l, USHORT b, CLINT s_l)
{
  int err;
  CLINT tmp_l;

  u2clint_l (tmp_l, b);
  err = add_l (a_l, tmp_l, s_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
  ISPURGED_L  ((1, sizeof (tmp_l), tmp_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular addition of CLINT and USHORT type                      */
/*  Syntax:    int umadd_l (CLINT a_l, USHORT b, CLINT s_l, CLINT m_l);       */
/*  Input:     a_l, b, m_l (Operands)                                         */
/*  Output:    s_l (Remainder of a_l + b mod m_l)                             */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
umadd_l (CLINT a_l, USHORT b, CLINT s_l, CLINT m_l)
{
  int err;
  CLINT tmp_l;

  u2clint_l (tmp_l, b);
  err = madd_l (a_l, tmp_l, s_l, m_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
  ISPURGED_L  ((1, sizeof (tmp_l), tmp_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Subtraction of CLINT and USHORT type                           */
/*  Syntax:    int usub_l (CLINT a_l, USHORT b, CLINT d_l);                   */
/*  Input:     a_l, b (Operands)                                              */
/*  Output:    d_l (Difference)                                               */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_UFL: Underflow                                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
usub_l (CLINT a_l, USHORT b, CLINT d_l)
{
  int err;
  CLINT tmp_l;

  u2clint_l (tmp_l, b);
  err = sub_l (a_l, tmp_l, d_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
  ISPURGED_L  ((1, sizeof (tmp_l), tmp_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular subtraction of CLINT and USHORT type                   */
/*  Syntax:    int umsub_l (CLINT a_l, USHORT b, CLINT d_l, CLINT m_l);       */
/*  Input:     a_l, b, m_l (Operands)                                         */
/*  Output:    d_l (Remainder of a_l - b mod m_l)                             */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
umsub_l (CLINT a_l, USHORT b, CLINT d_l, CLINT m_l)
{
  int err;
  CLINT tmp_l;

  u2clint_l (tmp_l, b);
  err = msub_l (a_l, tmp_l, d_l, m_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
  ISPURGED_L  ((1, sizeof (tmp_l), tmp_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Multiplication of CLINT and USHORT value                       */
/*  Syntax:    int umul_l (CLINT aa_l, USHORT b, CLINT pp_l);                 */
/*  Input:     aa_l, b (Factors)                                              */
/*  Output:    pp_l (Product)                                                 */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
umul_l (CLINT aa_l, USHORT b, CLINT pp_l)
{
  CLINT a_l;
  clint p_l[CLINTMAXSHORT + 1];
  int OFL = 0;

  cpy_l (a_l, aa_l);

  umul (a_l, b, p_l);

  if (DIGITS_L (p_l) > (USHORT)CLINTMAXDIGIT)        /* Overflow ? */
    {
      ANDMAX_L (p_l);                   /* Reduction modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (pp_l, p_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (a_l), a_l,
                   sizeof (p_l), p_l));

  ISPURGED_L  ((2, sizeof (a_l), a_l,
                   sizeof (p_l), p_l));
  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular multiplication of CLINT and USHORT value               */
/*  Syntax:    int ummul_l (CLINT a_l, USHORT b, CLINT c_l, CLINT m_l);       */
/*  Input:     a_l, b, m_l (Operands)                                         */
/*  Output:    c_l (Remainder a_l * b mod m_l)                                */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ummul_l (CLINT a_l, USHORT b, CLINT c_l, CLINT m_l)
{
  CLINTD tmp_l;
  int err;

  umul (a_l, b, tmp_l);
  err = mod_l (tmp_l, m_l, c_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (tmp_l), tmp_l));
  ISPURGED_L  ((1, sizeof (tmp_l), tmp_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular exponentiation with USHORT base                        */
/*  Syntax:    int wmexp_l (USHORT bas, CLINT  e_l, CLINT rem_l, CLINT m_l);  */
/*  Input:     bas (Base), e_l (Exponent), m_l (Modulus)                      */
/*  Output:    rem_l (Remainder bas^e_l mod m_l)                              */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
wmexp_l (USHORT bas, CLINT  e_l, CLINT rem_l, CLINT m_l)
{
  CLINT p_l, z_l;
  USHORT k, b, w;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (rem_l);           /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  if (EQZ_L (e_l))
    {
      SETONE_L (rem_l);
      return E_CLINT_OK;
    }

  if (0 == bas)
    {
      SETZERO_L (rem_l);
      return E_CLINT_OK;
    }

  cpy_l (z_l, e_l);
  SETONE_L (p_l);

  b = 1 << ((ld_l (z_l) - 1) & (BITPERDGT - 1UL));
  w = *MSDPTR_L (z_l);

  for (; b > 0; b >>= 1)
    {
      msqr_l (p_l, p_l, m_l);
      if ((w & b) > 0)
        {
          ummul_l (p_l, bas, p_l, m_l);
        }
    }

  for (k = DIGITS_L (z_l) - 1; k > 0; k--)
    {
      w = z_l[k];
      for (b = BASEDIV2; b > 0; b >>= 1)
        {
          msqr_l (p_l, p_l, m_l);
          if ((w & b) > 0)
            {
              ummul_l (p_l, bas, p_l, m_l);
            }
        }
    }

  cpy_l (rem_l, p_l);

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (b), &b,
                   sizeof (w), &w,
                   sizeof (p_l), p_l,
                   sizeof (z_l), z_l));

  ISPURGED_L  ((4, sizeof (b), &b,
                   sizeof (w), &w,
                   sizeof (p_l), p_l,
                   sizeof (z_l), z_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular exponentiation with USHORT base and odd modulus        */
/*             (Montgomery exponentiation)                                    */
/*  Syntax:    int wmexpm_l (USHORT bas, CLINT  e_l, CLINT rem_l, CLINT m_l); */
/*  Input:     bas (Base), e_l (Exponent), m_l (Modulus)                      */
/*  Output:    rem_l (Remainder of bas^e_l mod m_l)                           */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_MOD: Modulus even                                      */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
wmexpm_l (USHORT bas, CLINT  e_l, CLINT rem_l, CLINT m_l)
{
  CLINT p_l, z_l, md_l;
  clint r_l[CLINTMAXSHORT + 1];
  USHORT k, b, w, logB_r, mprime;

#ifdef FLINT_DEBUG
  int sign_rmin1, sign_mprime;
  CLINTD d_l, mprime_l, rmin1_l;
#endif

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (ISEVEN_L (m_l))
    {
      return E_CLINT_MOD;          /* Modulus not odd */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (rem_l);           /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  if (EQZ_L (e_l))
    {
      SETONE_L (rem_l);
      return E_CLINT_OK;
    }

  if (0 == bas)
    {
      SETZERO_L (rem_l);
      return E_CLINT_OK;
    }

  cpy_l (md_l, m_l);
  cpy_l (z_l, e_l);

  SETZERO_L (r_l);
  logB_r = DIGITS_L (md_l);
  setbit (r_l, logB_r << LDBITPERDGT);

  mprime = invmon_l (md_l);

#ifdef FLINT_DEBUG
  if (logB_r < CLINTMAXDIGIT)
    {
      xgcd_l (r_l, md_l, d_l, rmin1_l, &sign_rmin1, mprime_l, &sign_mprime);
      if (sign_mprime > 0)
        {
          msub_l (r_l, mprime_l, mprime_l, r_l);
        }

      Assert(EQONE_L (d_l));
      Assert(*LSDPTR_L (mprime_l) == mprime);
    }
#endif /* FLINT_DEBUG */

  mod_l (r_l, md_l, p_l);

  b = 1 << ((ld_l (z_l) - 1) & (BITPERDGT - 1UL));
  w = *MSDPTR_L (z_l);

  for (; b > 0; b >>= 1)
    {
      sqrmon_l (p_l, md_l, mprime, logB_r, p_l);

      if ((w & b) > 0)
        {
          ummul_l (p_l, bas, p_l, md_l);
        }
    }

  for (k = DIGITS_L (z_l) - 1; k > 0; k--)
    {
      w = z_l[k];
      for (b = BASEDIV2; b > 0; b >>= 1)
        {
          sqrmon_l (p_l, md_l, mprime, logB_r, p_l);
          if ((w & b) > 0)
            {
              ummul_l (p_l, bas, p_l, md_l);
            }
        }
    }

  mulmon_l (p_l, one_l, md_l, mprime, logB_r, rem_l);

  /* Purging of variables */
  PURGEVARS_L ((8, sizeof (b), &b,
                   sizeof (w), &w,
                   sizeof (logB_r), &logB_r,
                   sizeof (mprime), &mprime,
                   sizeof (p_l), p_l,
                   sizeof (z_l), z_l,
                   sizeof (r_l), r_l,
                   sizeof (md_l), md_l));

  ISPURGED_L  ((8, sizeof (b), &b,
                   sizeof (w), &w,
                   sizeof (logB_r), &logB_r,
                   sizeof (mprime), &mprime,
                   sizeof (p_l), p_l,
                   sizeof (z_l), z_l,
                   sizeof (r_l), r_l,
                   sizeof (md_l), md_l));

  return E_CLINT_OK;
}



/******************************************************************************/
/*                                                                            */
/*  Function:  Modular exponentiation with USHORT exponent                    */
/*  Syntax:    int umexp_l (CLINT bas_l, USHORT e, CLINT rem_l, CLINT m_l);   */
/*  Input:     bas_l (Base), e (Exponent), m_l (Modulus)                      */
/*  Output:    rem_l (Remainder of bas_l^e mod m_l)                           */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
umexp_l (CLINT bas_l, USHORT e, CLINT rem_l, CLINT m_l)
{
  CLINT tmp_l, tmpbas_l;
  USHORT k = BASEDIV2;
  int err = E_CLINT_OK;

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (rem_l);           /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  if (e == 0)
    {
      SETONE_L (rem_l);
      return E_CLINT_OK;
    }

  if (EQZ_L (bas_l))
    {
      SETZERO_L (rem_l);
      return E_CLINT_OK;
    }

  mod_l (bas_l, m_l, tmp_l);
  cpy_l (tmpbas_l, tmp_l);
  while ((e & k) == 0)
    {
      k >>= 1;
    }

  k >>= 1;

  while (k != 0)
    {
      msqr_l (tmp_l, tmp_l, m_l);
      if (e & k)
        {
          mmul_l (tmp_l, tmpbas_l, tmp_l, m_l);
        }
      k >>= 1;
    }

  cpy_l (rem_l, tmp_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (tmp_l), tmp_l,
                   sizeof (tmpbas_l), tmpbas_l));

  ISPURGED_L  ((2, sizeof (tmp_l), tmp_l,
                   sizeof (tmpbas_l), tmpbas_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Modular exponentiation with USHORT exponent (Montgomery)       */
/*  Syntax:    int umexpm_l (CLINT bas_l, USHORT e, CLINT rem_l, CLINT m_l);  */
/*  Input:     bas_l (Base), e (Exponent), m_l (Modulus)                      */
/*  Output:    rem_l (Remainder of bas_l^e mod m_l)                           */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
umexpm_l (CLINT bas_l, USHORT e, CLINT rem_l, CLINT m_l)
{
  CLINT a_l, p_l, md_l;
  clint r_l[CLINTMAXSHORT + 1];
  USHORT k, logB_r, mprime;
  int err = E_CLINT_OK;

#ifdef FLINT_DEBUG
  int sign_rmin1, sign_mprime;
  CLINTD d_l, mprime_l, rmin1_l;
#endif

  if (EQZ_L (m_l))
    {
      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (ISEVEN_L (m_l))
    {
      return E_CLINT_MOD;          /* Modulus not odd */
    }

  if (EQONE_L (m_l))
    {
      SETZERO_L (rem_l);           /* Modulus = 1 ==> Remainder = 0 */
      return E_CLINT_OK;
    }

  if (e == 0)
    {
      SETONE_L (rem_l);
      return E_CLINT_OK;
    }

  if (EQZ_L (bas_l))
    {
      cpy_l (rem_l, bas_l);
      return E_CLINT_OK;
    }

  if (DIGITS_L (bas_l) > (USHORT)CLINTMAXDIGIT)
    {
      err = E_CLINT_OFL;
    }

  cpy_l (md_l, m_l);

  SETZERO_L (r_l);
  logB_r = DIGITS_L (md_l);
  setbit (r_l, logB_r << LDBITPERDGT);
  if (DIGITS_L (r_l) > CLINTMAXDIGIT)
    {
      mod_l (r_l, md_l, r_l);
    }

  mprime = invmon_l (md_l);

#ifdef FLINT_DEBUG
  if (logB_r < CLINTMAXDIGIT)
    {
      xgcd_l (r_l, md_l, d_l, rmin1_l, &sign_rmin1, mprime_l, &sign_mprime);
      if (sign_mprime > 0)
       {
          msub_l (r_l, mprime_l, mprime_l, r_l);
        }

      Assert(EQONE_L (d_l));
      Assert(*LSDPTR_L (mprime_l) == mprime);
    }
#endif /* FLINT_DEBUG */

  mmul_l (bas_l, r_l, p_l, md_l);
  cpy_l (a_l, p_l);

  k = BASEDIV2;

  while ((e & k) == 0)
    {
      k >>= 1;
    }

  k >>= 1;

  while (k != 0)
    {
      sqrmon_l (p_l, md_l, mprime, logB_r, p_l);

      if (e & k)
        {
          mulmon_l (p_l, a_l, md_l, mprime, logB_r, p_l);
        }

      k >>= 1;
    }

  mulmon_l (p_l, one_l, md_l, mprime, logB_r, rem_l);

  /* Purging of variables */
  PURGEVARS_L ((5, sizeof (logB_r), &logB_r,
                   sizeof (mprime), &mprime,
                   sizeof (a_l), a_l,
                   sizeof (p_l), p_l,
                   sizeof (md_l), md_l));

  ISPURGED_L  ((5, sizeof (logB_r), &logB_r,
                   sizeof (mprime), &mprime,
                   sizeof (a_l), a_l,
                   sizeof (p_l), p_l,
                   sizeof (md_l), md_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Division with USHORT divisor                                   */
/*  Syntax:    int udiv_l (CLINT dv_l, USHORT uds, CLINT q_l, CLINT r_l);     */
/*  Input:     dv_l (Dividend), uds (Divisor)                                 */
/*  Output:    q_l (Quotient), r_l (Remainder)                                */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_DBZ: Division by Zero                                  */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
udiv_l (CLINT dv_l, USHORT uds, CLINT q_l, CLINT r_l)
{
  register clint *aptr_l;
  CLINTD a_l;                      /* Allow for dividend of double length */
  clint *qptr_l, *msdptra_l;
  ULONG rhat;
  USHORT rv;

  cpy_l (a_l, dv_l);

  if (0 == uds)
    {
      PURGEVARS_L ((1, sizeof (a_l), a_l));
      ISPURGED_L  ((1, sizeof (a_l), a_l));

      return E_CLINT_DBZ;          /* Division by Zero */
    }

  if (EQZ_L (a_l))
    {
      SETZERO_L (q_l);
      SETZERO_L (r_l);
      return E_CLINT_OK;
    }

  if (1 == DIGITS_L (a_l))
    {
      if (*LSDPTR_L (a_l) < uds)
        {
          cpy_l (r_l, a_l);
          SETZERO_L (q_l);
        }
      else if (*LSDPTR_L (a_l) == uds)
        {
          SETONE_L (q_l);
          SETZERO_L (r_l);
        }
      else
        {
          u2clint_l (q_l, (USHORT)(*LSDPTR_L (a_l) / uds));
          u2clint_l (r_l, (USHORT)(*LSDPTR_L (a_l) % uds));
        }

      PURGEVARS_L ((1, sizeof (a_l), a_l));
      ISPURGED_L  ((1, sizeof (a_l), a_l));

      return E_CLINT_OK;
    }

  msdptra_l = MSDPTR_L (a_l);

  rv = 0;
  for (aptr_l = msdptra_l, qptr_l = q_l + DIGITS_L (a_l); aptr_l >= LSDPTR_L (a_l); aptr_l--, qptr_l--)
    {
      *qptr_l = (USHORT)((rhat = ((((ULONG)rv) << BITPERDGT) +
                                          (ULONG)*aptr_l)) / uds);
      rv = (USHORT)(rhat - (ULONG)uds * (ULONG)*qptr_l);
    }
  SETDIGITS_L (q_l, DIGITS_L (a_l));

  RMLDZRS_L (q_l);

  if (rv == 0)
    {
      SETZERO_L (r_l);
    }
  else
    {
      u2clint_l (r_l, rv);
    }

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (rhat), &rhat,
                   sizeof (rv), &rv,
                   sizeof (a_l), a_l));

  ISPURGED_L  ((3, sizeof (rhat), &rhat,
                   sizeof (rv), &rv,
                   sizeof (a_l), a_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Reduction modulo USHORT divisor                                */
/*  Syntax:    USHORT umod_l (CLINT dv_l, USHORT uds);                        */
/*  Input:     dv_l (Dividend), uds (Divisor)                                 */
/*  Output:    -                                                              */
/*  Returns:   Remainder of dv_l mod uds (remainder < 0xffff)                 */
/*             0xffff in case of division by Zero                             */
/*                                                                            */
/******************************************************************************/
USHORT __FLINT_API
umod_l (CLINT dv_l, USHORT uds)
{
  CLINT q_l, r_l;
  USHORT rem;

  if (0 == uds)
    {
      return 0xffff;
    }

  udiv_l (dv_l, uds, q_l, r_l);
  switch (DIGITS_L (r_l))
    {
      case 1:
         rem = *LSDPTR_L (r_l);
        break;
      default:
        rem = 0;
    }

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (q_l), q_l,
                   sizeof (r_l), r_l));

  ISPURGED_L  ((2, sizeof (q_l), q_l,
                   sizeof (r_l), r_l));

  return rem;
}

/*** Mixed functions finished here ********************************************/


/******************************************************************************/
/*                                                                            */
/*  Function:  Calculate number of bits of a CLINT operand                    */
/*             (Integral part of base-2-logarithm + 1)                        */
/*  Syntax:    unsigned int ld_l (n_l);                                       */
/*  Input:     n_l (Argument)                                                 */
/*  Output:    -                                                              */
/*  Returns:   Number of relevant binary digits of n_l                        */
/*                                                                            */
/******************************************************************************/
unsigned int __FLINT_API
ld_l (CLINT n_l)
{
  unsigned int l;
  USHORT test;

  l = (unsigned int)DIGITS_L (n_l);
  while (n_l[l] == 0 && l > 0)
    {
      --l;
    }

  if (l == 0)
    {
      return 0;
    }

  test = n_l[l];
  l <<= LDBITPERDGT;

  while ((test & BASEDIV2) == 0)
    {
      test <<= 1;
      --l;
    }

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (test), &test));
  ISPURGED_L  ((1, sizeof (test), &test));

  return l;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Testing and setting of a single bit                            */
/*  Syntax:    int setbit_l (CLINT a_l, unsigned int pos);                    */
/*  Input:     a_l (Argument),                                                */
/*             pos (Position of the bit to be set in a_l, leftmost position   */
/*             is 0)                                                          */
/*  Output:    a_l, bit in position pos set to 1                              */
/*  Returns:   E_CLINT_OFL if pos >= CLINTMAXBIT                              */
/*             1 if bit in position pos had value 1 before it was set, 0 else */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
setbit_l (CLINT a_l, unsigned int pos)
{
  if (pos >= CLINTMAXBIT)
    {
      return E_CLINT_OFL;
    }

  return setbit (a_l, pos);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Testing a bit                                                  */
/*  Syntax:    int testbit_l (CLINT a_l, unsigned int pos);                   */
/*  Input:     a_l (Argument),                                                */
/*             pos (Position of bit to be tested in a_l, leftmost             */
/*             position is 0)                                                 */
/*  Output:    -                                                              */
/*  Returns:   Value 0 or 1 of bit in position pos                            */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
testbit_l (CLINT a_l, unsigned int pos)
{
  int res = 0;
  USHORT shortpos = (USHORT)(pos >> LDBITPERDGT);
  USHORT bitpos = (USHORT)(pos & (BITPERDGT - 1));
  if (shortpos < DIGITS_L (a_l))
    {
      if (a_l[shortpos + 1] & (USHORT)(1U << bitpos))
        {
          res = 1;
        }
    }

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (shortpos), &shortpos,
                   sizeof (bitpos), &bitpos));

  ISPURGED_L  ((2, sizeof (shortpos), &shortpos,
                   sizeof (bitpos), &bitpos));

  return res;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Testing and clearing of a single bit                           */
/*  Syntax:    int clearbit_l (CLINT a_l, unsigned int pos);                  */
/*  Input:     a_l (Argument),                                                */
/*             pos (Position of bit to be cleared in a_l, leftmost            */
/*             position is 0)                                                 */
/*  Output:    a_l, with bit in position pos cleared                          */
/*  Returns:   Value of bit in position pos before the clearing               */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
clearbit_l (CLINT a_l, unsigned int pos)
{
  int res = 0;
  USHORT shortpos = (USHORT)(pos >> LDBITPERDGT);
  USHORT bitpos = (USHORT)(pos & (BITPERDGT - 1));
  USHORT m = (USHORT)(1U << bitpos);

  if (shortpos < DIGITS_L (a_l))
    {
      if (a_l[shortpos + 1] & m)
        {
          res = 1;
        }
      a_l[shortpos + 1] &= (USHORT)(~m);
      RMLDZRS_L (a_l);
    }

  /* Purging of variables */
  PURGEVARS_L ((3, sizeof (shortpos), &shortpos,
                   sizeof (bitpos), &bitpos,
                   sizeof (m), &m));

  ISPURGED_L  ((3, sizeof (shortpos), &shortpos,
                   sizeof (bitpos), &bitpos,
                   sizeof (m), &m));

  return res;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  XOR (Exclusive Or) of two CLINT operands                       */
/*  Syntax:    void xor_l (CLINT a_l, CLINT b_l, CLINT c_l);                  */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    c_l (XOR sum of a_l and b_l)                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
xor_l (CLINT a_l, CLINT b_l, CLINT c_l)
{
  CLINT d_l;
  clint *r_l, *s_l, *t_l;
  clint *msdptrr_l;
  clint *msdptrs_l;

  if (DIGITS_L (a_l) < DIGITS_L (b_l))
    {
      r_l = LSDPTR_L (b_l);
      s_l = LSDPTR_L (a_l);
      msdptrr_l = MSDPTR_L (b_l);
      msdptrs_l = MSDPTR_L (a_l);
    }
  else
    {
      r_l = LSDPTR_L (a_l);
      s_l = LSDPTR_L (b_l);
      msdptrr_l = MSDPTR_L (a_l);
      msdptrs_l = MSDPTR_L (b_l);
    }

  t_l = LSDPTR_L (d_l);
  SETDIGITS_L (d_l, DIGITS_L (r_l - 1));

  while (s_l <= msdptrs_l)
    {
      *t_l++ = *r_l++ ^ *s_l++;
    }

  while (r_l <= msdptrr_l)
    {
      *t_l++ = *r_l++;
    }

  cpy_l (c_l, d_l);


  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (d_l), d_l));
  ISPURGED_L  ((1, sizeof (d_l), d_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  OR sum of two CLINT operands                                   */
/*  Syntax:    void or_l (CLINT a_l, CLINT b_l, CLINT c_l);                   */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    c_l (OR sum of a_l and b_l)                                    */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
or_l (CLINT a_l, CLINT b_l, CLINT c_l)
{
  CLINT d_l;
  clint *r_l, *s_l, *t_l;
  clint *msdptrr_l;
  clint *msdptrs_l;

  if (DIGITS_L (a_l) < DIGITS_L (b_l))
    {
      r_l = LSDPTR_L (b_l);
      s_l = LSDPTR_L (a_l);
      msdptrr_l = MSDPTR_L (b_l);
      msdptrs_l = MSDPTR_L (a_l);
    }
  else
    {
      r_l = LSDPTR_L (a_l);
      s_l = LSDPTR_L (b_l);
      msdptrr_l = MSDPTR_L (a_l);
      msdptrs_l = MSDPTR_L (b_l);
    }

  t_l = LSDPTR_L (d_l);
  SETDIGITS_L (d_l, DIGITS_L (r_l - 1));

  while (s_l <= msdptrs_l)
    {
      *t_l++ = *r_l++ | *s_l++;
    }

  while (r_l <= msdptrr_l)
    {
      *t_l++ = *r_l++;
    }

  cpy_l (c_l, d_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (d_l), d_l));
  ISPURGED_L  ((1, sizeof (d_l), d_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  AND sum of two CLINT operands                                  */
/*  Syntax:    void and_l (CLINT a_l, CLINT b_l, CLINT c_l);                  */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    c_l (AND-Sum of a_l and b_l)                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
and_l (CLINT a_l, CLINT b_l, CLINT c_l)
{
  CLINT d_l;
  clint *r_l, *s_l, *t_l;
  clint *lastptr_l;

  if (DIGITS_L (a_l) < DIGITS_L (b_l))
    {
      r_l = LSDPTR_L (b_l);
      s_l = LSDPTR_L (a_l);
      lastptr_l = MSDPTR_L (a_l);
    }
  else
    {
      r_l = LSDPTR_L (a_l);
      s_l = LSDPTR_L (b_l);
      lastptr_l = MSDPTR_L (b_l);
    }

  t_l = LSDPTR_L (d_l);
  SETDIGITS_L (d_l, DIGITS_L (s_l - 1));

  while (s_l <= lastptr_l)
    {
      *t_l++ = *r_l++ & *s_l++;
    }

  cpy_l (c_l, d_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (d_l), d_l));
  ISPURGED_L  ((1, sizeof (d_l), d_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Greatest Common Divisor of two CLINT operands                  */
/*  Syntax:    void gcd_l (CLINT aa_l, CLINT bb_l, CLINT cc_l);               */
/*  Input:     aa_l, bb_l (Operands)                                          */
/*  Output:    cc_l (GCD of a_l and b_l)                                      */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
gcd_l (CLINT aa_l, CLINT bb_l, CLINT cc_l)
{
  CLINT a_l, b_l, r_l, t_l;
  unsigned int k = 0;
  int sign_of_t;

  /* Step 1 */
  if (LT_L (aa_l, bb_l))
    {
      cpy_l (a_l, bb_l);
      cpy_l (b_l, aa_l);
    }
  else
    {
      cpy_l (a_l, aa_l);
      cpy_l (b_l, bb_l);
    }

  if (EQZ_L (b_l))
    {
      cpy_l (cc_l, a_l);

      PURGEVARS_L ((1, sizeof (a_l), a_l));
      ISPURGED_L  ((1, sizeof (a_l), a_l));

      return;
    }

  /* Step 2 */
  div_l (a_l, b_l, t_l, r_l);
  cpy_l (a_l, b_l);
  cpy_l (b_l, r_l);

  if (EQZ_L (b_l))
    {
      cpy_l (cc_l, a_l);

      k = sign_of_t = 0;
      PURGEVARS_L ((3, sizeof (a_l), a_l,
                       sizeof (t_l), t_l,
                       sizeof (r_l), r_l));

      ISPURGED_L  ((3, sizeof (a_l), a_l,
                       sizeof (t_l), t_l,
                       sizeof (r_l), r_l));
      return;
    }

  while (ISEVEN_L (a_l) && ISEVEN_L (b_l))
    {
      ++k;
      shr_l (a_l);
      shr_l (b_l);
    }

  /* Step 3 */
  while (ISEVEN_L (a_l))
    {
      shr_l (a_l);
    }
  while (ISEVEN_L (b_l))
    {
      shr_l (b_l);
    }

  /* Step 4 */
  do
    {
      if (GE_L (a_l, b_l))
        {
          sub_l (a_l, b_l, t_l);
          sign_of_t = 1;
        }
      else
        {
          sub_l (b_l, a_l, t_l);
          sign_of_t = -1;
        }

      if (EQZ_L (t_l))
        {                                    /* finished */
          cpy_l (cc_l, a_l);                 /* cc_l <- a */
          shift_l (cc_l, (long int)k);       /* cc_l <- cc_l*2**k */

          PURGEVARS_L ((3, sizeof (a_l), a_l,
                           sizeof (b_l), b_l,
                           sizeof (r_l), r_l));

          ISPURGED_L  ((3, sizeof (a_l), a_l,
                           sizeof (b_l), b_l,
                           sizeof (r_l), r_l));
          return;
        }

      /* Step 5 */
      while (ISEVEN_L (t_l))
        {
          shr_l (t_l);
        }

      if (-1 == sign_of_t)
        {
          cpy_l (b_l, t_l);
        }
      else
        {
          cpy_l (a_l, t_l);
        }
    }
  while (1);    /*lint !e506 Don't complain about >>constant value boolean<< */
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Extended Euclidean Algorithm                                   */
/*             Greatest Common Divisor d = GCD(a, b) and linear combination   */
/*             d = au + bv                                                    */
/*  Syntax:    void xgcd_l (CLINT a_l, CLINT b_l, CLINT d_l, CLINT u_l,       */
/*                                      int *sign_u, CLINT v_l, int *sign_v); */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    d_l (GCD of a_l and b_l)                                       */
/*             u_l, v_l (Factors of the linear combination  d = au + bv       */
/*             with signs in sign_u and sign_v)                               */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
xgcd_l (CLINT a_l, CLINT b_l, CLINT d_l, CLINT u_l, int *sign_u, CLINT v_l, int *sign_v)
{
  CLINT v1_l, v3_l, t1_l, t3_l, q_l;
  CLINTD tmp_l, tmpu_l, tmpv_l;
  int sign_v1, sign_t1;

  cpy_l (d_l, a_l);
  cpy_l (v3_l, b_l);

  if (EQZ_L (v3_l))                /* b_l == 0 ? */
    {
      SETONE_L (u_l);
      SETZERO_L (v_l);
      *sign_u = 1;
      *sign_v = 1;
      return;
    }

  SETONE_L (tmpu_l);
  *sign_u = 1;
  SETZERO_L (v1_l);
  sign_v1 = 1;

  while (GTZ_L (v3_l))
    {
      div_l (d_l, v3_l, q_l, t3_l);
      mul_l (v1_l, q_l, q_l);
      sign_t1 = ssub (tmpu_l, *sign_u, q_l, sign_v1, t1_l);
      cpy_l (tmpu_l, v1_l);
      *sign_u = sign_v1;
      cpy_l (d_l, v3_l);
      cpy_l (v1_l, t1_l);
      sign_v1 = sign_t1;
      cpy_l (v3_l, t3_l);
    }

  mult (a_l, tmpu_l, tmp_l);
  *sign_v = ssub (d_l, 1, tmp_l, *sign_u, tmp_l);
  div_l (tmp_l, b_l, tmpv_l, tmp_l);

  Assert (EQZ_L (tmp_l));

#ifdef FLINT_DEBUG
  {
    CLINTD x_l, y_l, z_l;
    mult (a_l, tmpu_l, x_l);
    mult (b_l, tmpv_l, y_l);
    sadd (x_l, *sign_u, y_l, *sign_v, z_l);
    Assert (equ_l (z_l, d_l));
  }
#endif

  cpy_l (u_l, tmpu_l);
  cpy_l (v_l, tmpv_l);

  /* Purging of variables */
  PURGEVARS_L ((10, sizeof (sign_v1), &sign_v1,
                    sizeof (sign_t1), &sign_t1,
                    sizeof (v1_l), v1_l,
                    sizeof (v3_l), v3_l,
                    sizeof (t1_l), t1_l,
                    sizeof (t3_l), t3_l,
                    sizeof (q_l), q_l,
                    sizeof (tmp_l), tmp_l,
                    sizeof (tmpu_l), tmpu_l,
                    sizeof (tmpv_l), tmpv_l));

  ISPURGED_L  ((10, sizeof (sign_v1), &sign_v1,
                    sizeof (sign_t1), &sign_t1,
                    sizeof (v1_l), v1_l,
                    sizeof (v3_l), v3_l,
                    sizeof (t1_l), t1_l,
                    sizeof (t3_l), t3_l,
                    sizeof (q_l), q_l,
                    sizeof (tmp_l), tmp_l,
                    sizeof (tmpu_l), tmpu_l,
                    sizeof (tmpv_l), tmpv_l));

  return;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Inverse of a modulo n                                          */
/*  Syntax:    void inv_l (CLINT a_l, CLINT n_l, CLINT g_l, CLINT i_l);       */
/*  Input:     a_l (Operand), n_l (Modulus)                                   */
/*  Output:    g_l (GCD of a_l and n_l),                                      */
/*             i_l (Inverse of a_l mod n_l)                                   */
/*             If gcd > 1 the inverse does not exist, i_l is set to zero then */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
inv_l (CLINT a_l, CLINT n_l, CLINT g_l, CLINT i_l)
{
  CLINT v1_l, v3_l, t1_l, t3_l, q_l;

  if (EQZ_L (a_l))
    {
      if (EQZ_L (n_l))
        {
          SETZERO_L (g_l);
          SETZERO_L (i_l);
          return;
        }
      else
        {
          cpy_l (g_l, n_l);
          SETZERO_L (i_l);
          return;
        }
    }
  else
    {
      if (EQZ_L (n_l))
        {
          cpy_l (g_l, a_l);
          SETZERO_L (i_l);
          return;
        }
    }

  cpy_l (g_l, a_l);
  cpy_l (v3_l, n_l);
  SETZERO_L (v1_l);
  SETONE_L (t1_l);

  do
    {
      div_l (g_l, v3_l, q_l, t3_l);

      if (GTZ_L (t3_l))
        {
          mmul_l (v1_l, q_l, q_l, n_l);
          msub_l (t1_l, q_l, q_l, n_l);
          cpy_l (t1_l, v1_l);
          cpy_l (v1_l, q_l);
          cpy_l (g_l, v3_l);
          cpy_l (v3_l, t3_l);
        }
    }

  while (GTZ_L (t3_l));
  cpy_l (g_l, v3_l);
  if (EQONE_L (g_l))
    {
      cpy_l (i_l, v1_l);
    }
  else
    {
      SETZERO_L (i_l);
    }

  /* Purging of variables */
  PURGEVARS_L ((5, sizeof (v1_l), v1_l,
                   sizeof (v3_l), v3_l,
                   sizeof (t1_l), t1_l,
                   sizeof (t3_l), t3_l,
                   sizeof (q_l), q_l));

  ISPURGED_L  ((5, sizeof (v1_l), v1_l,
                   sizeof (v3_l), v3_l,
                   sizeof (t1_l), t1_l,
                   sizeof (t3_l), t3_l,
                   sizeof (q_l), q_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Least Common Multiple (LCM) of two CLINT operands              */
/*  Syntax:    int lcm_l (CLINT a_l, CLINT b_l, CLINT c_l);                   */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    c_l (LCM of a_l and b_l)                                       */
/*  Returns:   E_CLINT_OK : Everything is O.K.                                */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
lcm_l (CLINT a_l, CLINT b_l, CLINT c_l)
{
  CLINT g_l, junk_l;
  int err;

  if (EQZ_L (a_l) || EQZ_L (b_l))
    {
      SETZERO_L (c_l);
      return E_CLINT_OK;
    }

  gcd_l (a_l, b_l, g_l);
  div_l (a_l, g_l, g_l, junk_l);
  err = mul_l (g_l, b_l, c_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (g_l), g_l,
                   sizeof (junk_l), junk_l));

  ISPURGED_L  ((2, sizeof (g_l), g_l,
                   sizeof (junk_l), junk_l));

  return err;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Test for Squareness of a CLINT value (Cohen, ACCANT, p. 40)    */
/*  Syntax:    unsigned int issqr_l (CLINT n_l, CLINT r_l);                   */
/*  Input:     n_l (argument)                                                 */
/*  Output:    r_l (square root of n_l if n_l is square, 0 else)              */
/*  Returns:   1: n_l is square                                               */
/*             0: else                                                        */
/*                                                                            */
/******************************************************************************/
static const UCHAR q11[11]=
  {1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0};

static const UCHAR q63[63]=
  {1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1,
   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
   0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0};

static const UCHAR q64[64]=
  {1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
   0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
   0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};

static const UCHAR q65[65]=
  {1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
   1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
   0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1};


unsigned int __FLINT_API
issqr_l (CLINT n_l, CLINT r_l)
{
  CLINT q_l;
  USHORT r;

  if (EQZ_L (n_l))
    {
      SETZERO_L (r_l);
      return 1;
    }

  if (1 == q64[*LSDPTR_L (n_l) & 63])      /* q64[n_l mod 64] */
    {
      r = umod_l (n_l, 45045);

      if ((1 == q63[r % 63]) && (1 == q65[r % 65]) && (1 == q11[r % 11]))
        {
          iroot_l (n_l, r_l);
          sqr_l (r_l, q_l);

          if (equ_l (n_l, q_l))
            {
              PURGEVARS_L ((1, sizeof (q_l), q_l));
              ISPURGED_L  ((1, sizeof (q_l), q_l));

              return 1;
            }
        }
    }

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (r), &r,
                   sizeof (q_l), q_l));

  ISPURGED_L  ((2, sizeof (r), &r,
                   sizeof (q_l), q_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Integral part of square root of a CLINT operand                */
/*  Syntax:    void iroot_l (CLINT a_l, CLINT floor_l);                       */
/*  Input:     a_l (Argument)                                                 */
/*  Output:    floor_l (Integral part of square root of a_l)                  */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
iroot_l (CLINT a_l, CLINT floor_l)
{
  CLINT x_l, y_l, r_l;
  unsigned int l;

  if (EQZ_L (a_l))
    {
      SETZERO_L (floor_l);
      return;
    }

  /* Step 1 */
  l = (ld_l (a_l) + 1) >> 1;       /* (ld(a_l) + 2) div 2 */
  SETZERO_L (y_l);
  setbit (y_l, l);

  do
    {
      cpy_l (x_l, y_l);
      /* Step 2 */
      div_l (a_l, x_l, y_l, r_l);
      add_l (y_l, x_l, y_l);
      shr_l (y_l);
    }

  /* Step 3 */
  while (LT_L (y_l, x_l));
  cpy_l (floor_l, x_l);

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (l), &l,
                   sizeof (x_l), x_l,
                   sizeof (y_l), y_l,
                   sizeof (r_l), r_l));

  ISPURGED_L  ((4, sizeof (l), &l,
                   sizeof (x_l), x_l,
                   sizeof (y_l), y_l,
                   sizeof (r_l), r_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Square root modulo p, p prime                                  */
/*  Syntax:    int proot_l (CLINT a_l, CLINT p_l, CLINT x_l);                 */
/*  Input:     a_l (Square), p_l (Prime number > 2)                           */
/*  Output:    x_l (Square root of a_l mod p_l)                               */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             -1 : a_l is not a residual square mod p_l or p_l is even       */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
proot_l (CLINT a_l, CLINT p_l, CLINT x_l)
{
  CLINT b_l, q_l, t_l, y_l, z_l;
  int r, m;

  if (EQZ_L (p_l) || ISEVEN_L (p_l))
    {
      return -1;
    }

  if (EQZ_L (a_l))
    {
      SETZERO_L (x_l);
      return E_CLINT_OK;
    }

  cpy_l (q_l, p_l);
  dec_l (q_l);
  r = twofact_l (q_l, q_l);

  /* Step 1: Search for residual nonsquare */
  cpy_l (z_l, two_l);
  while (jacobi_l (z_l, p_l) == 1)
    {
      inc_l (z_l);
    }
  mexp_l (z_l, q_l, z_l, p_l);

  /* Step 2: Initialization */
  cpy_l (y_l, z_l);
  dec_l (q_l);
  shr_l (q_l);                     /* q_l = (q - 1)/2 */
  mexp_l (a_l, q_l, x_l, p_l);
  msqr_l (x_l, b_l, p_l);
  mmul_l (b_l, a_l, b_l, p_l);
  mmul_l (x_l, a_l, x_l, p_l);

  /* Step 3: Search exponent */
  mod_l (b_l, p_l, q_l);

  while (!equ_l (q_l, one_l))
    {
      m = 0;
      do
        {
          ++m;
          msqr_l (q_l, q_l, p_l);       /* q_l = b^(2^m) */
        }
      while (!equ_l (q_l, one_l));      /* m <= r */

      if (m == r)
        {
          PURGEVARS_L ((5, sizeof (b_l), b_l,
                           sizeof (q_l), q_l,
                           sizeof (t_l), t_l,
                           sizeof (y_l), y_l,
                           sizeof (z_l), z_l));

          ISPURGED_L  ((5, sizeof (b_l), b_l,
                           sizeof (q_l), q_l,
                           sizeof (t_l), t_l,
                           sizeof (y_l), y_l,
                           sizeof (z_l), z_l));
          return -1;
        }

      /* Step 4: Reduction of Exponent */

      mexp2_l (y_l, (USHORT)(r - m - 1), t_l, p_l);         /*lint !e732 */
      msqr_l (t_l, y_l, p_l);    /* Don't complain about >>loss of sign<< */
      mmul_l (x_l, t_l, x_l, p_l);
      mmul_l (b_l, y_l, b_l, p_l);
      cpy_l (q_l, b_l);
      r = m;
    }

  /* Purging of variables */
  PURGEVARS_L ((7, sizeof (r), &r,
                   sizeof (m), &m,
                   sizeof (b_l), b_l,
                   sizeof (q_l), q_l,
                   sizeof (t_l), t_l,
                   sizeof (y_l), y_l,
                   sizeof (z_l), z_l));

  ISPURGED_L  ((7, sizeof (r), &r,
                   sizeof (m), &m,
                   sizeof (b_l), b_l,
                   sizeof (q_l), q_l,
                   sizeof (t_l), t_l,
                   sizeof (y_l), y_l,
                   sizeof (z_l), z_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Square root modulo pq; p, q  prime numbers                     */
/*  Syntax:    int root_l (CLINT a_l, CLINT p_l, CLINT q_l, CLINT x_l);       */
/*  Input:     a_l (Operand), p_l, q_l (Prime numbers)                        */
/*  Output:    x_l (Square root a_l mod p_l * q_l)                            */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             -1: a_l is not residual square mod p_l*q_l                     */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
root_l (CLINT a_l, CLINT p_l, CLINT q_l, CLINT x_l)
{
  CLINT x0_l, x1_l, x2_l, x3_l, xp_l, xq_l, n_l;
  CLINTD u_l, v_l;
  clint *xptr_l;
  int sign_u, sign_v;

  if (0 != proot_l (a_l, p_l, xp_l) || 0 != proot_l (a_l, q_l, xq_l))
    {
      return -1;
    }

  if (EQZ_L (a_l))
    {
      SETZERO_L (x_l);
      return E_CLINT_OK;
    }

  mul_l (p_l, q_l, n_l);
  xgcd_l (p_l, q_l, x0_l, u_l, &sign_u, v_l, &sign_v);
  mul_l (u_l, p_l, u_l);
  mul_l (u_l, xq_l, u_l);
  mul_l (v_l, q_l, v_l);
  mul_l (v_l, xp_l, v_l);
  sign_u = sadd (u_l, sign_u, v_l, sign_v, x0_l);
  smod (x0_l, sign_u, n_l, x0_l);

  sub_l (n_l, x0_l, x1_l);
  msub_l (u_l, v_l, x2_l, n_l);
  sub_l (n_l, x2_l, x3_l);

  xptr_l = MIN_L (x0_l, x1_l);
  xptr_l = MIN_L (xptr_l, x2_l);
  xptr_l = MIN_L (xptr_l, x3_l);
  cpy_l (x_l, xptr_l);

  /* Purging of variables */
  PURGEVARS_L ((11, sizeof (sign_u), &sign_u,
                    sizeof (sign_v), &sign_v,
                    sizeof (x0_l), x0_l,
                    sizeof (x1_l), x1_l,
                    sizeof (x2_l), x2_l,
                    sizeof (x3_l), x3_l,
                    sizeof (xp_l), xp_l,
                    sizeof (xq_l), xq_l,
                    sizeof (n_l), n_l,
                    sizeof (u_l), u_l,
                    sizeof (v_l), v_l));

  ISPURGED_L  ((11, sizeof (sign_u), &sign_u,
                    sizeof (sign_v), &sign_v,
                    sizeof (x0_l), x0_l,
                    sizeof (x1_l), x1_l,
                    sizeof (x2_l), x2_l,
                    sizeof (x3_l), x3_l,
                    sizeof (xp_l), xp_l,
                    sizeof (xq_l), xq_l,
                    sizeof (n_l), n_l,
                    sizeof (u_l), u_l,
                    sizeof (v_l), v_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Ad hoc search for a primitive root modulo p, p prime number > 2*/
/*  Syntax:    int primroot_l(CLINT a_l, unsigned int noofprimes,             */
/*                                               clint *primes_l[]);          */
/*  Input:     noofprimes (Number of distinct prime factors pi of the group   */
/*                         order p - 1 of Z/pZ)                               */
/*             primes_l (Array of CLINT operands, starting with p-1, followed */
/*                       by prime factors pi of p-1 = p1^e1*...*pk^ek,        */
/*                       k = noofprimes)                                      */
/*  Output:    a_l (Primitive root modulo p_l)                                */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             -1: p - 1 is odd => p is not a prime                           */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
primroot_l (CLINT a_l, unsigned int noofprimes, clint *primes_l[])
{
  CLINT p_l, t_l, junk_l;
  ULONG i;

  if (ISODD_L (primes_l[0])) 
    {
      return -1;
    }

  cpy_l (p_l, primes_l[0]);
  inc_l (p_l);               /* p_l = p  (Modulus) */
  SETONE_L (a_l);

  do
    {
      inc_l (a_l);

      /* Test, whether a_l is a square. */
      /* If so, a_l can't be a primitive root and a_l is incremented */
      if (issqr_l (a_l, t_l))
        {
          inc_l (a_l);
        }

      i = 1;

      do
        {
          div_l (primes_l[0], primes_l[i++], t_l, junk_l);  /* t_l <- n/pi */
          mexpkm_l (a_l, t_l, t_l, p_l);
        }

      while ((i <= noofprimes) && !EQONE_L (t_l));

    }
  while (EQONE_L (t_l));

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (i), &i,
                   sizeof (p_l), p_l,
                   sizeof (t_l), t_l,
                   sizeof (junk_l), junk_l));

  ISPURGED_L  ((4, sizeof (i), &i,
                   sizeof (p_l), p_l,
                   sizeof (t_l), t_l,
                   sizeof (junk_l), junk_l));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Solving systems of quadratic congruences                       */
/*             with application of the Chinese Remainder Theorem              */
/*  Syntax:    chinrem_l (unsigned int noofeq, clint** coeff_l, CLINT x_l);   */
/*  Input:     noofeq  (Number of equations to solve)                         */
/*             coeff_l (Array of 2*noofeq pointers to CLINT-Arguments ai, mi, */
/*                      mi coprime (cf. Chap. 10))                            */
/*  Output:    x_l (Solution of congruences)                                  */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             1          : noofeq is 0                                       */
/*             2          : mi not coprime                                    */
/*             E_CLINT_OFL: Overflow                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
chinrem_l (unsigned int noofeq, clint** coeff_l, CLINT x_l)
{
  clint *ai_l, *mi_l;

  CLINT g_l, u_l, v_l, m_l;
  unsigned int i;
  int sign_u, sign_v, err, error = E_CLINT_OK;
  if (0 == noofeq)
    {
      return 1;
    }

  cpy_l (x_l, *(coeff_l++));
  cpy_l (m_l, *(coeff_l++));

  for (i = 1; i < noofeq; i++)
    {
      ai_l = *(coeff_l++);
      mi_l = *(coeff_l++);

      xgcd_l (m_l, mi_l, g_l, u_l, &sign_u, v_l, &sign_v);

      if (!EQONE_L (g_l))
        {
          PURGEVARS_L ((4, sizeof (g_l), g_l,
                           sizeof (u_l), u_l,
                           sizeof (v_l), v_l,
                           sizeof (m_l), m_l));

          ISPURGED_L  ((4, sizeof (g_l), g_l,
                           sizeof (u_l), u_l,
                           sizeof (v_l), v_l,
                           sizeof (m_l), m_l));
          return 2;
        }

      err = mul_l (u_l, m_l, u_l);
      if (E_CLINT_OK == error)
        {
          error = err;
        }
      err = mul_l (u_l, ai_l, u_l);
      if (E_CLINT_OK == error)
        {
          error = err;
        }
      err = mul_l (v_l, mi_l, v_l);
      if (E_CLINT_OK == error)
        {
          error = err;
        }
      err = mul_l (v_l, x_l, v_l);
      if (E_CLINT_OK == error)
        {
          error = err;
        }

      sign_u = sadd (u_l, sign_u, v_l, sign_v, x_l);

      err = mul_l (m_l, mi_l, m_l);
      if (E_CLINT_OK == error)
        {
          error = err;
        }

      smod (x_l, sign_u, m_l, x_l);
    }

  /* Purging of variables */
  PURGEVARS_L ((7, sizeof (i), &i,
                   sizeof (sign_u), &sign_u,
                   sizeof (sign_v), &sign_v,
                   sizeof (g_l), g_l,
                   sizeof (u_l), u_l,
                   sizeof (v_l), v_l,
                   sizeof (m_l), m_l));

  ISPURGED_L  ((7, sizeof (i), &i,
                   sizeof (sign_u), &sign_u,
                   sizeof (sign_v), &sign_v,
                   sizeof (g_l), g_l,
                   sizeof (u_l), u_l,
                   sizeof (v_l), v_l,
                   sizeof (m_l), m_l));

  return error;
}


/******************************************************************************/


static int tab2[] =
{0, 1, 0, -1, 0, -1, 0, 1};


/******************************************************************************/
/*                                                                            */
/*  Function:  Legendre-/Jacobi symbol (a/b)                                  */
/*  Syntax:    int jacobi_l (CLINT aa_l, CLINT bb_l);                         */
/*  Input:     aa_l, bb_l (Arguments)                                         */
/*  Output:    -                                                              */
/*  Returns:   Value of Jacobi symbol aa_l over bb_l                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
jacobi_l (CLINT aa_l, CLINT bb_l)
{
  CLINT a_l, b_l, tmp_l;
  long int k, v;

  /* Step 1 */
  if (EQZ_L (bb_l))
    {
      if (EQONE_L (aa_l))
        {
          return 1;
        }
      else
        {
          return 0;
        }
    }

  /* Step 2 */
  if (ISEVEN_L (aa_l) && ISEVEN_L (bb_l))
    {
      return 0;
    }

  cpy_l (a_l, aa_l);
  cpy_l (b_l, bb_l);

  v = twofact_l (b_l, b_l);
  if ((v & 1) == 0)
    {
      k = 1;
    }
  else
    {
      k = tab2[*LSDPTR_L (a_l) & 7];    /* *LSDPTR_L(a_l) & 7 == a_l % 8 */
    }

  /* Step 3 */
  while (GTZ_L (a_l))
    {
      v = twofact_l (a_l, a_l);
      if ((v & 1) != 0)
        {
          k *= tab2[*LSDPTR_L (b_l) & 7];
        }

      /* Step 4 */
      if (*LSDPTR_L (a_l) & *LSDPTR_L (b_l) & 2)
        {
          k = -k;
        }
      cpy_l (tmp_l, a_l);
      mod_l (b_l, tmp_l, a_l);
      cpy_l (b_l, tmp_l);
    }

  if (GT_L (b_l, one_l))
    {
      k = 0;
    }

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (v), &v,
                   sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  ISPURGED_L  ((4, sizeof (v), &v,
                   sizeof (a_l), a_l,
                   sizeof (b_l), b_l,
                   sizeof (tmp_l), tmp_l));

  return (int)k;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Sieving by dividing by small primes                            */
/*  Syntax:    USHORT sieve_l (CLINT a_l, unsigned int no_of_smallprimes);    */
/*  Input:     a_l (Dividend),                                                */
/*             no_of_smallprimes (Number of small primes to divide by)        */
/*  Output:    -                                                              */
/*  Returns:   Prime factor of a_l, if found                                  */
/*             1 if a_l is prime                                              */
/*             0 if no prime factor was found                                 */
/*                                                                            */
/******************************************************************************/
USHORT __FLINT_API
sieve_l (CLINT a_l, unsigned int no_of_smallprimes)
{
  clint *aptr_l;
  USHORT bv, rv, qv;
  ULONG rhat;
  unsigned int i = 1;

  if (ISEVEN_L (a_l))
    {
      if (equ_l (a_l, two_l))
        {
          return 1;
        }
      else
        {
          return 2;
        }
    }

  no_of_smallprimes = MIN (no_of_smallprimes, NOOFSMALLPRIMES);
  bv = 2;
  do
    {
      rv = 0;
      bv += smallprimes[i];

      Assert (bv < 2000);

      for (aptr_l = MSDPTR_L (a_l); aptr_l >= LSDPTR_L (a_l); aptr_l--)
        {
          qv = (USHORT)((rhat = ((((ULONG)rv) << BITPERDGT) +
                                           (ULONG)*aptr_l)) / bv);
          rv = (USHORT)(rhat - (ULONG)bv * (ULONG)qv);
        }

    }
  while (rv != 0 && ++i <= no_of_smallprimes);

  if (0 == rv)
    {
      if (DIGITS_L (a_l) == 1 && *LSDPTR_L (a_l) == bv)
        {
          bv = 1;
        }
      /* else: result is bv */
    }
  else
    {
      bv = 0;
    }

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (rhat), &rhat,
                   sizeof (i), &i,
                   sizeof (rv), &rv,
                   sizeof (qv), &qv));

  ISPURGED_L  ((4, sizeof (rhat), &rhat,
                   sizeof (i), &i,
                   sizeof (rv), &rv,
                   sizeof (qv), &qv));

  return bv;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Probabilistic primality test acc. to Miller-Rabin (MR-Test)    */
/*  Syntax:    int prime_l (CLINT n_l, unsigned int no_of_smallprimes,        */
/*                                                 unsigned int iterations);  */
/*  Input:     n_l (Number to be tested),                                     */
/*             no_of_smallprimes (Number of small primes for the sieve)       */
/*             iterations (Number of rounds for the MR-Test.                  */
/*                         If iterations == 0 the optimized number of rounds  */
/*                         for an error probability < 2^-80 is used.)         */
/*  Output:    -                                                              */
/*  Returns:   1: n_l probably prime                                          */
/*             0: n_l definitely not prime                                    */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
prime_l (CLINT n_l, unsigned int no_of_smallprimes, unsigned int iterations)
{
  CLINT d_l, x_l, q_l;
  USHORT i, j, k, p;
  int isprime;

  if (EQONE_L (n_l))
    {
      return 0;
    }

  no_of_smallprimes = MIN (no_of_smallprimes, NOOFSMALLPRIMES);
  k = sieve_l (n_l, no_of_smallprimes);
  if (1 == k)
    {
      return 1;
    }

  if (1 < k)
    {
      return 0;
    }
  else
    {
      /* If iterations == 0 was given as a parameter optimize the number */
      /* of iterations for an error probability < 2^-80 (cf. Chap. 10.5) */
      if (0 == iterations)
        {
          k = ld_l (n_l);
          if      (k <   73) iterations = 37;
          else if (k <  105) iterations = 32;
          else if (k <  137) iterations = 25;
          else if (k <  197) iterations = 19;
          else if (k <  220) iterations = 15;
          else if (k <  235) iterations = 13;
          else if (k <  252) iterations = 12;
          else if (k <  273) iterations = 11;
          else if (k <  300) iterations = 10;
          else if (k <  332) iterations =  9;
          else if (k <  375) iterations =  8;
          else if (k <  433) iterations =  7;
          else if (k <  514) iterations =  6;
          else if (k <  638) iterations =  5;
          else if (k <  847) iterations =  4;
          else if (k < 1275) iterations =  3;
          else if (k < 2861) iterations =  2;
          else iterations = 1;
        }

      cpy_l (d_l, n_l);
      dec_l (d_l);
      k = (USHORT)twofact_l (d_l, q_l);
      p = 0; /* Start with base a = 2 */
      i = 0;
      isprime = 1;

      do
        {
          p += smallprimes[i++];

#ifdef FLINT_ASM
          wmexp_l (p, q_l, x_l, n_l);
#else
          wmexpm_l (p, q_l, x_l, n_l);
#endif /* FLINT_ASM */

          if (!EQONE_L (x_l))
            {

              j = 0;

              while (!EQONE_L (x_l) && !equ_l (x_l, d_l) && ++j < k)
                {
                  msqr_l (x_l, x_l, n_l);
                }

              if (!equ_l (x_l, d_l))
                {
                  isprime = 0;
                }
            }
        }
      while ((--iterations > 0) && isprime);

      /* Purging of variables */
      PURGEVARS_L ((7, sizeof (i), &i,
                       sizeof (j), &j,
                       sizeof (k), &k,
                       sizeof (p), &p,
                       sizeof (d_l), d_l,
                       sizeof (x_l), x_l,
                       sizeof (q_l), q_l));

      ISPURGED_L  ((7, sizeof (i), &i,
                       sizeof (j), &j,
                       sizeof (k), &k,
                       sizeof (p), &p,
                       sizeof (d_l), d_l,
                       sizeof (x_l), x_l,
                       sizeof (q_l), q_l));

      return isprime;
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Factor 2^t of a CLINT operand n, such that n = 2^t * b, b odd  */
/*  Syntax:    int twofact_l (CLINT a_l, CLINT b_l);                          */
/*  Input:     a_l (Argument)                                                 */
/*  Output:    b_l (Odd factor of a_l)                                        */
/*  Returns:   Logarithm t of the factor 2^t of a_l                           */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
twofact_l (CLINT a_l, CLINT b_l)
{
  int k = 0;
  if (EQZ_L (a_l))
    {
      SETZERO_L (b_l);
      return 0;
    }

  cpy_l (b_l, a_l);
  while (ISEVEN_L (b_l))
    {
      shr_l (b_l);
      ++k;
    }

  return k;
}


/******************************************************************************/


static char ntable[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c',
  'd', 'e', 'f'};


#ifndef isxdigit
#define isxdigit(__c)    ((('0' <= (__c)) && ((__c) <= '9'))\
                          ?1\
                          :(('a' <= (__c)) && ((__c) <= 'f'))\
                          ?1\
                          :(('A' <= (__c)) && ((__c) <= 'F'))\
                          ?1\
                          :0)
#endif


/******************************************************************************/
/*                                                                            */
/*  Function:  Representation of a CLINT operand as character string          */
/*  Syntax:    char *xclint2str_l (CLINT n_l, USHORT base, int showbase);     */
/*  Input:     n_l (Argument to be represented)                               */
/*             base (Base of representation)                                  */
/*             showbase (==0: no prefix;                                      */
/*                       !=0: prefix 0b, 0 oder 0x for base 2, 8 oder 16)     */
/*  Output:    -                                                              */
/*  Returns:   Pointer to character string                                    */
/*             NULL if base < 2 or base > 16                                  */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
xclint2str_l (CLINT n_l, USHORT base, int showbase)
{
  CLINTD u_l, r_l;
  int i = 0;
  static char N[CLINTMAXBIT + 3];

  if (2U > base || base > 16U)
    {
      return (char *)NULL;
    }

  cpy_l (u_l, n_l);
  do
    {
      (void)udiv_l (u_l, base, u_l, r_l);
      if (EQZ_L (r_l))
        {
          N[i++] = '0';
        }
      else
        {
          N[i++] = (char)ntable[*LSDPTR_L (r_l) & 0xff];
        }
    }
  while (GTZ_L (u_l));

  if (showbase)
    {
      switch (base)
        {
          case 2:
            N[i++] = 'b';
            N[i++] = '0';
            break;
          case 8:
            N[i++] = '0';
            break;
          case 16:
            N[i++] = 'x';
            N[i++] = '0';
            break;
        }                          /*lint !e744 default-statement pointless */
    }
  N[i] = '\0';

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (u_l), u_l,
                   sizeof (r_l), r_l));

  ISPURGED_L  ((2, sizeof (u_l), u_l,
                   sizeof (r_l), r_l));

  return strrev_l (N);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a character string into CLINT-number             */
/*  Syntax:    int str2clint_l (CLINT n_l, char *str, USHORT b);              */
/*  Input:     str (Pointer to character string),                             */
/*             base (Base to which number in str is represented)              */
/*  Output:    n_l (CLINT value converted from str)                           */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_BOR: Basis invalid                                     */
/*             E_CLINT_OFL: Overflow                                          */
/*             E_CLINT_NPT: str is NULL-Pointer                               */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
str2clint_l (CLINT n_l, char *str, USHORT base)
{
  USHORT n;
  int error = E_CLINT_OK;

  if (str == NULL)
    {
      return E_CLINT_NPT;          /* Error: Base invalid */
    }

  if (2 > base || base > 16)
    {
      return E_CLINT_BOR;          /* Error: Base invalid */
    }

  SETZERO_L (n_l);

  if (*str == '0')
    {
      if ((tolower_l(*(str+1)) == 'x') ||
          (tolower_l(*(str+1)) == 'b'))      /* Ignore prefixes */
        {
          ++str;
          ++str;
        }
    }

  while (isxdigit ((int)*str) || isspace ((int)*str))
    {
      if (!isspace ((int)*str))
        {
          n = (USHORT)tolower_l (*str);
          switch (n)
            {
              case 'a':
              case 'b':
              case 'c':
              case 'd':
              case 'e':
              case 'f':
                n -= (USHORT)('a' - 10);
                break;
              default:
                n -= (USHORT)'0';
            }

          if (n >= base)
            {
              error = E_CLINT_BOR;
              break;
            }

          if ((error = umul_l (n_l, base, n_l)) != E_CLINT_OK)
            {
              break;
            }
          if ((error = uadd_l (n_l, n, n_l)) != E_CLINT_OK)
            {
              break;
            }
        }
      ++str;
    }

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Representation of CLINT operand as array of bytes (IEEE P1363) */
/*  Syntax:    UCHAR *clint2byte_l (CLINT n_l, int *len);                     */
/*  Input:     n_l (Argument to be represented)                               */
/*  Output:    len (Number of bytes in byte array)                            */
/*  Returns:   Pointer to byte array with representation of n_l               */
/*             Significance of digits increasing from right to left           */
/*             NULL, if len is NULL-pointer                                   */
/*                                                                            */
/******************************************************************************/
UCHAR * __FLINT_API
clint2byte_l (CLINT n_l, int *len)
{
  CLINTD u_l, r_l;
  int i = 0, j;
  UCHAR help;
  static UCHAR bytes[CLINTMAXBYTE];

  if (len == NULL)
    {
      return NULL;
    }

  cpy_l (u_l, n_l);
  do
    {
      (void)udiv_l (u_l, 0x100, u_l, r_l);
      if (GTZ_L (r_l))
        {
          bytes[i++] = (UCHAR)*LSDPTR_L (r_l);
        }
      else
        {
          bytes[i++] = 0;
        }
    }
  while (GTZ_L (u_l));

  *len = i;

  for (i = 0, j = *len - 1; i < j; i++, j--)
    {
      help = bytes[i];
      bytes[i] = bytes[j];
      bytes[j] = help;
    }

  /* Purging of variables */
  PURGEVARS_L ((5, sizeof (i), &i,
                   sizeof (j), &j,
                   sizeof (help), &help,
                   sizeof (u_l), u_l,
                   sizeof (r_l), r_l));

  ISPURGED_L  ((5, sizeof (i), &i,
                   sizeof (j), &j,
                   sizeof (help), &help,
                   sizeof (u_l), u_l,
                   sizeof (r_l), r_l));

  return bytes;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of an array of bytes to CLINT value (IEEE P1363)    */
/*  Syntax:    int byte2clint_l (CLINT n_l, UCHAR *bytestr, int len);         */
/*  Input:     bytestr (Pointer to array of UCHAR, significance of bytes      */
/*                      increasing from right to left)                        */
/*             len  (Number of bytes in bytestr)                              */
/*  Output:    n_l (CLINT value with converted from bytestr)                  */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_OFL: Overflow                                          */
/*             E_CLINT_NPT: bytestr is NULL-Pointer                           */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
byte2clint_l (CLINT n_l, UCHAR *bytestr, int len)
{
  int error = E_CLINT_OK, i;

  if (bytestr == NULL)
    {
      return E_CLINT_NPT;
    }

  if ((unsigned int)len > CLINTMAXBYTE)
    {
      return E_CLINT_OFL;
    }

  SETZERO_L (n_l);

  for (i = 0; i < len; i++, bytestr++)
    {
      if ((error = umul_l (n_l, 0x100, n_l)) != E_CLINT_OK)
        {
          break;
        }

      if ((error = uadd_l (n_l, *bytestr, n_l)) != E_CLINT_OK)
        {
          break;
        }
    }

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of an USHORT value to CLINT format                  */
/*  Syntax:    void u2clint_l (CLINT num_l, USHORT u);                        */
/*  Input:     u (Value to be converted)                                      */
/*  Output:    num_l (CLINT variable with value u)                            */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
u2clint_l (CLINT num_l, USHORT u)
{
  *LSDPTR_L (num_l) = u;
  SETDIGITS_L (num_l, 1);
  RMLDZRS_L (num_l);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of an ULONG value to CLINT format                   */
/*  Syntax:    void ul2clint_l (CLINT num_l, USHORT u);                       */
/*  Input:     ul (Value to be converted)                                     */
/*  Output:    num_l (CLINT variable with value ul)                           */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ul2clint_l (CLINT num_l, ULONG ul)
{
  *LSDPTR_L (num_l) = (USHORT)(ul & 0xffff);
  *(LSDPTR_L (num_l) + 1) = (USHORT)((ul >> 16) & 0xffff);
  SETDIGITS_L (num_l, 2);
  RMLDZRS_L (num_l);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a CLINT operand to a character string            */
/*             with representation to basis 16 (hexadezimal representation)   */
/*  Syntax:    char *fhexstr_l (CLINT n_l);                                   */
/*  Input:     n_l (CLINT value to be represented)                            */
/*  Output:    Pointer to a character string representing n_l to base 16      */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
fhexstr_l (CLINT n_l)
{
  return xclint2str_l (n_l, 16, 0);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a CLINT operand to a character string            */
/*             with representation to base 10 (Decimal representation)        */
/*  Syntax:    char *fdecstr_l (CLINT n_l);                                   */
/*  Input:     n_l (CLINT value to be represented)                            */
/*  Output:    Pointer to a character string representing n_l to base 10      */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
fdecstr_l (CLINT n_l)
{
  return xclint2str_l (n_l, 10, 0);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a CLINT operand to a character string            */
/*             with represenation to base 8 (octal representation)            */
/*  Syntax:    char *foctstr_l (CLINT n_l);                                   */
/*  Input:     n_l (CLINT value to be represented)                            */
/*  Output:    Pointer to a character string representing n_l to base 8       */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
foctstr_l (CLINT n_l)
{
  return xclint2str_l (n_l, 8, 0);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a CLINT operand to a character string            */
/*             with represenation to base 2 (binary representation)           */
/*  Syntax:    char *fbinstr_l (CLINT n_l);                                   */
/*  Input:     n_l (CLINT value to be represented)                            */
/*  Output:    Pointer to a character string representing n_l to base 2       */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
fbinstr_l (CLINT n_l)
{
  return xclint2str_l (n_l, 2, 0);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Show version of FLINT-Library                                  */
/*  Syntax:    unsigned long version_l (void);                                */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Value representing the version of FLINT as defined in flint.h  */
/*             in the most significant word.                                  */
/*             If assembler functions are used, the high-order byte of the    */
/*             least significant word shows the value 0x61 (ASCII 'a'), else  */
/*             without assembler support the high-order byte has value 0.     */
/*             If flint.c was compiled in security mode (default), the low-   */
/*             order byte of the least significant word shows the value 0x73  */
/*             (ASCII 's'), else 0.                                           */
/*                                                                            */
/******************************************************************************/
ULONG __FLINT_API
version_l (void)
{
  return (ULONG)((FLINT_VERSION << BITPERDGT) +
                 (_FLINT_ASM << 8) + _FLINT_SECMOD);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Show version of FLINT-Library as character string              */
/*  Syntax:    unsigned long verstr_l (void);                                 */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Pointer to character string with version                       */
/*             If assembler functions are used, the character 'a' is appended */
/*             to the version number.                                         */
/*             If flint.c was compiled in security mode, the character 's'    */
/*             is appended to the version string (default).                   */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
verstr_l (void)
{
  static char s[10];
  sprintf(s, "%d.%.2d", FLINT_VERMAJ, FLINT_VERMIN);

#ifdef FLINT_ASM
  strcat(s, "a");
#endif

#ifdef FLINT_SECURE
  strcat(s, "s");
#endif

  return (char *)s;
}


/******************************************************************************/
/* Register-Handling                                                          */
/* Protected Interface                                                        */

struct clint_registers
{
  unsigned int created;
  unsigned int noofregs;
  clint **reg_l;
};


/******************************************************************************/

static struct clint_registers registers =
{0, 0, 0};

static USHORT NoofRegs = NOOFREGS;

static int
allocate_reg_l (void)
{
  USHORT i, j;

  if ((registers.reg_l = (clint **)malloc (sizeof (clint *) * NoofRegs)) == NULL)
    {
      return E_CLINT_MAL;
    }

  for (i = 0; i < NoofRegs; i++)
    {
      if ((registers.reg_l[i] = (clint *)malloc (CLINTMAXBYTE)) == NULL)
        {
          for (j = 0; j < i; j++)
            {
              free (registers.reg_l[j]);
            }
          return E_CLINT_MAL;      /* Error: malloc */
        }
    }

  return E_CLINT_OK;
}


/******************************************************************************/


static void
destroy_reg_l (void)
{
  unsigned int i;

  for (i = 0; i < registers.noofregs; i++)
    {
      memset (registers.reg_l[i], 0, CLINTMAXBYTE);
      free (registers.reg_l[i]);
      registers.reg_l[i] = NULL;
    }
  free (registers.reg_l);
}


/******************************************************************************/
/* Register-Handling Public Interface                                         */


/******************************************************************************/
/*                                                                            */
/*  Function:  Creation of a register of CLINT type                           */
/*  Syntax:    clint * create_l (void);                                       */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Pointer to register                                            */
/*                                                                            */
/******************************************************************************/
clint * __FLINT_API
create_l (void)
{
  return (clint *)malloc (CLINTMAXBYTE);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion of a register                                         */
/*  Syntax:    void purge_l (CLINT reg_l);                                    */
/*  Input:     reg_l (Register)                                               */
/*  Output:    All digits of reg_l are overwritten with 0, thus leaving reg_l */
/*             with value 0                                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
purge_l (CLINT reg_l)
{
  if (reg_l != NULL)
    {
      memset (reg_l, 0, CLINTMAXBYTE);
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion of a CLINT variable with double length                */
/*  Syntax:    void purged_l (CLINTD reg_l);                                  */
/*  Input:     reg_l (CLINT variable with double length)                      */
/*  Output:    All digits of reg_l are overwritten with 0, thus leaving reg_l */
/*             with value 0                                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
purged_l (CLINTD reg_l)
{
  if (reg_l != NULL)
    {
      memset (reg_l, 0, sizeof (CLINTD));
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion of a CLINT variable with quadruple length             */
/*  Syntax:    void purgeq_l (CLINTQ reg_l);                                  */
/*  Input:     reg_l (CLINT variable with quadruple length)                   */
/*  Output:    All digits of reg_l are overwritten with 0, thus leaving reg_l */
/*             with value 0                                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
purgeq_l (CLINTQ reg_l)
{
  if (reg_l != NULL)
    {
      memset (reg_l, 0, sizeof (CLINTQ));
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion and freeing a CLINT register created by create_l()    */
/*  Syntax:    void free_l (CLINT reg_l);                                     */
/*  Input:     reg_l (Register)                                               */
/*  Output:    -                                                              */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
free_l (CLINT reg_l)
{
  if (reg_l != NULL)
    {
      memset (reg_l, 0, CLINTMAXBYTE);
      free (reg_l);
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Adjusting the number of registers in a set                     */
/*  Syntax:    void set_noofregs_l (unsigned int nregs);                      */
/*  Input:     nregs (Number of registers in the set)                         */
/*  Output:    -                                                              */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
set_noofregs_l (unsigned int nregs)
{
  NoofRegs = (USHORT)nregs;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Creation of a set of CLINT registers, increment of a semaphore */
/*  Syntax:    int create_reg_l (void);                                       */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_MAL: Error in malloc()                                 */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
create_reg_l (void)
{
  int error = E_CLINT_OK;

  if (registers.created == 0)
    {
      error = allocate_reg_l ();
      registers.noofregs = NoofRegs;
    }

  if (!error)
    {
      ++registers.created;
    }

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Get a pointer to a register of the set of registers            */
/*  Syntax:    clint * get_reg_l (unsigned int reg);                          */
/*  Input:     reg (Number of the requested register)                         */
/*  Output:    -                                                              */
/*  Returns:   Pointer to the requested register if it exists, else NULL      */
/*                                                                            */
/******************************************************************************/
clint * __FLINT_API
get_reg_l (unsigned int reg)
{
  if (!registers.created || (reg >= registers.noofregs))
    {
      return (clint *)NULL;
    }

  return registers.reg_l[reg];
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion of a single register in a set of registers            */
/*  Syntax:    int purge_reg_l (unsigned int reg);                            */
/*  Input:     reg (Number of the register to be deleted)                     */
/*  Output:    All digits of the requested register are overwritten with 0,   */
/*             thus leaving the register with value 0                         */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_NOR: Registers not allocated                           */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
purge_reg_l (unsigned int reg)
{
  if (!registers.created || (reg >= registers.noofregs))
    {
      return E_CLINT_NOR;
    }

  memset (registers.reg_l[reg], 0, CLINTMAXBYTE);
  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion of a set of CLINT registers                           */
/*  Syntax:    int purgeall_reg_l (void);                                     */
/*  Input:     -                                                              */
/*  Output:    All digits of all registers of the set are overwritten         */
/*             with 0, thus leaving the registers with value 0                */
/*  Returns:   E_CLINT_OK : Everything O.K.                                   */
/*             E_CLINT_NOR: Registers not allocated                           */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
purgeall_reg_l (void)
{
  unsigned int i;

  if (registers.created)
    {
      for (i = 0; i < registers.noofregs; i++)
        {
          memset (registers.reg_l[i], 0, CLINTMAXBYTE);
        }
      return E_CLINT_OK;
    }

  return E_CLINT_NOR;              /* Error: Register not allocated */
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Deletion and freeing a set of CLINT registers,                 */
/*             if semaphore == 1                                              */
/*  Syntax:    void free_reg_l (void);                                        */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
free_reg_l (void)
{
  if (registers.created == 1)
    {
      destroy_reg_l ();
    }

  if (registers.created)
    {
      --registers.created;
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Check of validity of a variable as CLINT type                  */
/*  Syntax:    int vcheck_l (CLINT n_l);                                      */
/*  Input:     n_l (CLINT variable to be checked)                             */
/*  Output:    -                                                              */
/*  Returns:   E_VCHECK_OK  : Everything O.K.                                 */
/*             E_VCHECK_MEM: n_l is NULL-Pointer                              */
/*             E_VCHECK_OFL: Value out of range                               */
/*             E_VCHECK_LDZ: n_l has leading zeros                            */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
vcheck_l (CLINT n_l)
{
  int error = E_VCHECK_OK;
  if (n_l == NULL)
    {
      error = E_VCHECK_MEM;        /* n_l is NULL-pointer */
    }
  else
    {
      if (((unsigned int)DIGITS_L (n_l)) > CLINTMAXDIGIT)
        {
          error = E_VCHECK_OFL;    /* Overflow  */
        }
      else
        {
          if ((DIGITS_L (n_l) > 0) && (n_l[DIGITS_L (n_l)] == 0))
            {
              error = E_VCHECK_LDZ;     /* Leading zeros */
            }
        }
    }

  return error;
}


/******************************************************************************/


static clint SEED64[10];
static clint A64[] =
{0x0004, 0x7f2d, 0x4c95, 0xf42d, 0x5851};

static clint BUFF64[100];


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a 64-bit pseudorandom number of CLINT type       */
/*  Syntax:    clint * rand64_l (void);                                       */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Pointer to 64-bit pseudorandom number of CLINT type            */
/*                                                                            */
/******************************************************************************/
clint * __FLINT_API
rand64_l (void)
{
  mul_l (SEED64, A64, SEED64);
  inc_l (SEED64);

  SEED64[0] = MIN (SEED64[0], 4);       /* Reduction modulo 2^64 */
  return ((clint *)SEED64);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type UCHAR              */
/*  Syntax:    UCHAR ucrand64_l (void);                                       */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Pseudorandom number of type unsigned char                      */
/*                                                                            */
/******************************************************************************/
UCHAR __FLINT_API
ucrand64_l (void)
{
  rand64_l();
  return (UCHAR)(SEED64[SEED64[0]] >> (BITPERDGT - 8));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type USHORT             */
/*  Syntax:    USHORT usrand64_l (void);                                      */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Pseudorandom number of type unsigned short                     */
/*                                                                            */
/******************************************************************************/
USHORT __FLINT_API
usrand64_l (void)
{
  rand64_l();
  return SEED64[SEED64[0]];
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type ULONG              */
/*  Syntax:    ULONG ulrand64_l (void);                                       */
/*  Input:     -                                                              */
/*  Output:    -                                                              */
/*  Returns:   Pseudorandom number of type unsigned long                      */
/*                                                                            */
/******************************************************************************/
ULONG __FLINT_API
ulrand64_l (void)
{
  ULONG val;
  USHORT l;
  rand64_l();

  l = SEED64[0];
  switch (l)
    {
      case 4:
      case 3:
      case 2:
        val = (ULONG)SEED64[l-1];
        val += ((ULONG)SEED64[l] << BITPERDGT);
        break;
      case 1:
        val = (ULONG)SEED64[l];
        break;
      default:
        val = 0;
    }

  return val;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Initialization of the 64-bit pseudorandon number generator     */
/*  Syntax:    clint * seed64_l (CLINT seed_l);                               */
/*  Input:     seed_l (Seed)                                                  */
/*  Output:    -                                                              */
/*  Returns:   Pointer to the internal buffer BUFF64, where the previously    */
/*             generated value is stored                                      */
/*                                                                            */
/******************************************************************************/
clint * __FLINT_API
seed64_l (CLINT seed_l)
{
  int i;
  cpy_l (BUFF64, SEED64);
  for (i = 0; i <= MIN (DIGITS_L (seed_l), 4); i++)
    {
      SEED64[i] = seed_l[i];
    }

  return BUFF64;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Initialization of the 64-bit pseudorandon number generator     */
/*             with a value of type ULONG                                     */
/*  Syntax:    clint * seed64_l (ULONG seed_l);                               */
/*  Input:     seed_l (Seed)                                                  */
/*  Output:    -                                                              */
/*  Returns:   Pointer to the internal buffer BUFF64, where the previously    */
/*             generated value is stored                                      */
/*                                                                            */
/******************************************************************************/
clint * __FLINT_API
ulseed64_l (ULONG seed)
{
  cpy_l (BUFF64, SEED64);
  ul2clint_l (SEED64, seed);

  return BUFF64;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type CLINT              */
/*             (Prior initialization by calling seed64_l() required)          */
/*  Syntax:    void rand_l (CLINT a_l, int l);                                */
/*  Input:     l (Number of binary digits to be generated)                    */
/*  Output:    a_l (Generated pseudorandom number)                            */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
rand_l (CLINT a_l, int l)
{
  USHORT i, j;
  USHORT ls, lr;

  l = MIN ((unsigned int)l, CLINTMAXBIT);
  ls = (USHORT)l >> LDBITPERDGT;
  lr = (USHORT)l & ((USHORT)BITPERDGT - 1);

  for (i = 1; i <= ls; i++)
    {
      a_l[i] = usrand64_l ();
    }

  if (lr > 0)
    {
      ++ls;
      a_l[ls] = usrand64_l ();
      j = 1U << (lr - 1);                         /* j <- 2^(lr - 1) */
      a_l[ls] = (a_l[ls] | j) & ((j << 1) - 1);   /* Set bit lr to 1, higher bits to 0 */
    }
  else
    {
      a_l[ls] |= BASEDIV2;
    }

  SETDIGITS_L (a_l, ls);

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (i), &i,
                   sizeof (j), &j,
                   sizeof (ls), &ls,
                   sizeof (lr), &lr));

  ISPURGED_L  ((4, sizeof (i), &i,
                   sizeof (j), &j,
                   sizeof (ls), &ls,
                   sizeof (lr), &lr));
}
/******************************************************************************/



/******************************************************************************
 * Private arithmetic kernel functions                                        *
 ******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*  Function:  Addition kernel function                                       */
/*             w/o overflow detection, w/o checking for leading zeros         */
/*  Syntax:    void add (CLINT a_l, CLINT b_l, CLINT s_l);                    */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    s_l (Sum)                                                      */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
add (CLINT a_l, CLINT b_l, CLINT s_l)
{
  clint *msdptra_l, *msdptrb_l;
  clint *aptr_l, *bptr_l, *sptr_l = LSDPTR_L (s_l);
  ULONG carry = 0L;

  if (DIGITS_L (a_l) < DIGITS_L (b_l))
    {
      aptr_l = LSDPTR_L (b_l);
      bptr_l = LSDPTR_L (a_l);
      msdptra_l = MSDPTR_L (b_l);
      msdptrb_l = MSDPTR_L (a_l);
      SETDIGITS_L (s_l, DIGITS_L (b_l));
    }
  else
    {
      aptr_l = LSDPTR_L (a_l);
      bptr_l = LSDPTR_L (b_l);
      msdptra_l = MSDPTR_L (a_l);
      msdptrb_l = MSDPTR_L (b_l);
      SETDIGITS_L (s_l, DIGITS_L (a_l));
    }

  while (bptr_l <= msdptrb_l)
    {
      *sptr_l++ = (USHORT)(carry = (ULONG)*aptr_l++ + (ULONG)*bptr_l++
                                   + (ULONG)(USHORT)(carry >> BITPERDGT));
    }
  while (aptr_l <= msdptra_l)
    {
      *sptr_l++ = (USHORT)(carry = (ULONG)*aptr_l++
               + (ULONG)(USHORT)(carry >> BITPERDGT));
    }
  if (carry & BASE)
    {
      *sptr_l = 1;
      INCDIGITS_L (s_l);
    }

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (carry), &carry));
  ISPURGED_L  ((1, sizeof (carry), &carry));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Subtraction kernel function                                    */
/*             w/o overflow detection, w/o checking for leading zeros         */
/*  Syntax:    void sub (CLINT a_l, CLINT b_l, CLINT d_l);                    */
/*  Input:     a_l, b_l (Operands)                                            */
/*  Output:    d_l (Difference)                                               */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sub (CLINT a_l, CLINT b_l, CLINT d_l)
{
  clint *msdptra_l, *msdptrb_l;
  clint *aptr_l = LSDPTR_L (a_l), *bptr_l = LSDPTR_L (b_l), *dptr_l = LSDPTR_L (d_l);
  ULONG carry = 0L;

  msdptra_l = MSDPTR_L (a_l);
  msdptrb_l = MSDPTR_L (b_l);

  SETDIGITS_L (d_l, DIGITS_L (a_l));

  while (bptr_l <= msdptrb_l)
    {
      *dptr_l++ = (USHORT)(carry = (ULONG)*aptr_l++ - (ULONG)*bptr_l++
                                         - ((carry & BASE) >> BITPERDGT));
    }

  while (aptr_l <= msdptra_l)
    {
      *dptr_l++ = (USHORT)(carry = (ULONG)*aptr_l++
                     - ((carry & BASE) >> BITPERDGT));
    }

  RMLDZRS_L (d_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (carry), &carry));
  ISPURGED_L  ((1, sizeof (carry), &carry));
}


#if !defined FLINT_ASM
/******************************************************************************/
/*                                                                            */
/*  Function:  Multiplication kernel function                                 */
/*             w/o overflow detection, w/o checking for leading zeros         */
/*             accumulator mode not supported                                 */
/*  Syntax:    void mult (CLINT aa_l, CLINT bb_l, CLINT p_l);                 */
/*  Input:     aa_l, bb_l (Factors)                                           */
/*  Output:    p_l (Product)                                                  */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
mult (CLINT aa_l, CLINT bb_l, CLINT p_l) /* Allow for double length result    */
{
  register clint *cptr_l, *bptr_l;
  clint *a_l, *b_l, *aptr_l, *csptr_l, *msdptra_l, *msdptrb_l;
  USHORT av;
  ULONG carry;

  if (EQZ_L (aa_l) || EQZ_L (bb_l))
    {
      SETZERO_L (p_l);
      return;
    }

  if (DIGITS_L (aa_l) < DIGITS_L (bb_l))
    {
      a_l = bb_l;
      b_l = aa_l;
    }
  else
    {
      a_l = aa_l;
      b_l = bb_l;
    }

  msdptra_l = MSDPTR_L (a_l);
  msdptrb_l = MSDPTR_L (b_l);

  carry = 0;
  av = *LSDPTR_L (a_l);
  for (bptr_l = LSDPTR_L (b_l), cptr_l = LSDPTR_L (p_l); bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
    {
      *cptr_l = (USHORT)(carry = (ULONG)av * (ULONG)*bptr_l +
                           (ULONG)(USHORT)(carry >> BITPERDGT));
    }
  *cptr_l = (USHORT)(carry >> BITPERDGT);

  for (csptr_l = LSDPTR_L (p_l) + 1, aptr_l = LSDPTR_L (a_l) + 1; aptr_l <= msdptra_l; csptr_l++, aptr_l++)
    {
      carry = 0;
      av = *aptr_l;
      for (bptr_l = LSDPTR_L (b_l), cptr_l = csptr_l; bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
        {
          *cptr_l = (USHORT)(carry = (ULONG)av * (ULONG)*bptr_l +
              (ULONG)*cptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
        }
      *cptr_l = (USHORT)(carry >> BITPERDGT);
    }

  SETDIGITS_L (p_l, DIGITS_L (a_l) + DIGITS_L (b_l));
  RMLDZRS_L (p_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (carry), &carry,
                   sizeof (av), &av));

  ISPURGED_L  ((2, sizeof (carry), &carry,
                   sizeof (av), &av));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Multiplication kernel function (CLINT type) * (USHORT type)    */
/*             w/o overflow detection, w/o checking for leading zeros         */
/*             accumulator mode not supported                                 */
/*  Syntax:    void umul (CLINT a_l, USHORT b, CLINT p_l);                    */
/*  Input:     a_l, b (Factors)                                               */
/*  Output:    p_l (Product)                                                  */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
umul (CLINT a_l, USHORT b, CLINT p_l)   /* Allow for double length result     */
{
  register clint *aptr_l, *cptr_l;
  clint *msdptra_l;
  ULONG carry;

  if (EQZ_L (a_l) || 0 == b)
    {
      SETZERO_L (p_l);
      return;
    }

  msdptra_l = MSDPTR_L (a_l);

  carry = 0;
  for (aptr_l = LSDPTR_L (a_l), cptr_l = LSDPTR_L (p_l); aptr_l <= msdptra_l; aptr_l++, cptr_l++)
    {
      *cptr_l = (USHORT)(carry = (ULONG)b * (ULONG)*aptr_l +
                          (ULONG)(USHORT)(carry >> BITPERDGT));
    }
  *cptr_l = (USHORT)(carry >> BITPERDGT);

  SETDIGITS_L (p_l, DIGITS_L (a_l) + 1);
  RMLDZRS_L (p_l);

  /* Purging of variables */
  PURGEVARS_L ((1, sizeof (carry), &carry));
  ISPURGED_L  ((1, sizeof (carry), &carry));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Squaring kernel function                                       */
/*             w/o overflow detection, w/o checking for leading zeros         */
/*             accumulator mode not supported                                 */
/*  Syntax:    void sqr (CLINT a_l, CLINT r_l);                               */
/*  Input:     a_l (Factor)                                                   */
/*  Output:    p_l (Square)                                                   */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sqr (CLINT a_l, CLINT p_l)              /* Allow for double length result     */
{
  register clint *cptr_l, *bptr_l;
  clint *aptr_l, *csptr_l, *msdptra_l, *msdptrb_l, *msdptrc_l;
  USHORT av;
  ULONG carry;

  if (EQZ_L (a_l))
    {
      SETZERO_L (p_l);
      return;
    }

  msdptrb_l = MSDPTR_L (a_l);
  msdptra_l = msdptrb_l - 1;
  *LSDPTR_L (p_l) = 0;
  carry = 0;
  av = *LSDPTR_L (a_l);
  for (bptr_l = LSDPTR_L (a_l) + 1, cptr_l = LSDPTR_L (p_l) + 1; bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
    {
      *cptr_l = (USHORT)(carry = (ULONG)av * (ULONG)*bptr_l +
                           (ULONG)(USHORT)(carry >> BITPERDGT));
    }
  *cptr_l = (USHORT)(carry >> BITPERDGT);

  for (aptr_l = LSDPTR_L (a_l) + 1, csptr_l = LSDPTR_L (p_l) + 3; aptr_l <= msdptra_l; aptr_l++, csptr_l += 2)
    {
      carry = 0;
      av = *aptr_l;
      for (bptr_l = aptr_l + 1, cptr_l = csptr_l; bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
        {
          *cptr_l = (USHORT)(carry = (ULONG)av * (ULONG)*bptr_l +
              (ULONG)*cptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
        }
      *cptr_l = (USHORT)(carry >> BITPERDGT);
    }

  msdptrc_l = cptr_l;
  carry = 0;
  for (cptr_l = LSDPTR_L (p_l); cptr_l <= msdptrc_l; cptr_l++)
    {
      *cptr_l = (USHORT)(carry = (((ULONG)*cptr_l) << 1) +
                        (ULONG)(USHORT)(carry >> BITPERDGT));
    }
  *cptr_l = (USHORT)(carry >> BITPERDGT);

  carry = 0;
  for (bptr_l = LSDPTR_L (a_l), cptr_l = LSDPTR_L (p_l); bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
    {
      *cptr_l = (USHORT)(carry = (ULONG)*bptr_l * (ULONG)*bptr_l +
                (ULONG)*cptr_l + (ULONG)(USHORT)(carry >> BITPERDGT));
      cptr_l++;
      *cptr_l = (USHORT)(carry = (ULONG)*cptr_l + (carry >> BITPERDGT));
    }

  SETDIGITS_L (p_l, DIGITS_L (a_l) << 1);
  RMLDZRS_L (p_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (carry), &carry,
                   sizeof (av), &av));

  ISPURGED_L  ((2, sizeof (carry), &carry,
                   sizeof (av), &av));
}

#endif /* FLINT_ASM */


/******************************************************************************/
/*                                                                            */
/*  Function:  Addition with sign                                             */
/*  Syntax:    int sadd (CLINT a_l, int sign_a, CLINT b_l, int sign_b,        */
/*                                                            CLINT c_l);     */
/*  Input:     a_l (Operand), sign_a (Sign a_l), b_l (Operand),               */
/*             sign_b (Sign b_l)                                              */
/*  Output:    c_l (Sum)                                                      */
/*  Returns:   Sign of sum c_l                                                */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
sadd (CLINT a_l, int sign_a, CLINT b_l, int sign_b, CLINT c_l)
{
  int sign_c;

  Assert (sign_a == 1 || sign_a == -1);
  Assert (sign_b == 1 || sign_b == -1);

  if (1 == sign_a)
    {
      if (1 == sign_b)            /* a + b */
        {
          add (a_l, b_l, c_l);
          sign_c = 1;
        }
      else                        /* -1 == sign_b, a - b */
        {
          if (LT_L (a_l, b_l))
            {
              sub (b_l, a_l, c_l);
              sign_c = -1;
            }
          else
            {
              sub (a_l, b_l, c_l);
              sign_c = 1;
            }
        }
    }
  else                            /* -1 == sign_a */
    {
      if (1 == sign_b)            /* b - a */
        {
          if (LT_L (b_l, a_l))
            {
              sub (a_l, b_l, c_l);
              sign_c = -1;
            }
          else
            {
              sub (b_l, a_l, c_l);
              sign_c = 1;
            }
        }
      else                        /* -1 == sign_b, -(a + b) */
        {
          add (a_l, b_l, c_l);
          sign_c = -1;
        }
    }

  return sign_c;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Subtraction with sign                                          */
/*  Syntax:    int ssub (CLINT a_l, int sign_a, CLINT b_l, int sign_b,        */
/*                                                            CLINT c_l);     */
/*  Input:     a_l (Operand), sign_a (Sign a_l), b_l (Operand),               */
/*             sign_b (Sign b_l)                                              */
/*  Output:    c_l (Difference)                                               */
/*  Returns:   Sign of difference c_l                                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ssub (CLINT a_l, int sign_a, CLINT b_l, int sign_b, CLINT c_l)
{
  int sign_c;

  Assert (sign_a == 1 || sign_a == -1);
  Assert (sign_b == 1 || sign_b == -1);

  if (1 == sign_a)
    {
      if (1 == sign_b)            /* a - b */
        {
          if (LT_L (a_l, b_l))
            {
              sub (b_l, a_l, c_l);
              sign_c = -1;
            }
          else
            {
              sub (a_l, b_l, c_l);
              sign_c = 1;
            }
        }
      else                        /* -1 == sign_b, a + b */
        {
          add (a_l, b_l, c_l);
          sign_c = 1;
        }
    }
  else                            /* -1 == sign_a */
    {
      if (1 == sign_b)            /* -(a + b) */
        {
          add (a_l, b_l, c_l);
          sign_c = -1;
        }
      else                        /* -1 == sign_b, b - a) */
        {
          if (LT_L (b_l, a_l))
            {
              sub (a_l, b_l, c_l);
              sign_c = -1;
            }
          else
            {
              sub (b_l, a_l, c_l);
              sign_c = 1;
            }
        }
    }

  return sign_c;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Reduction a mod b, a signed                                    */
/*  Syntax:    int smod (CLINT a_l, int sign_a, CLINT b_l, CLINT c_l);        */
/*  Input:     a_l (Dividend), sign_a (sign a_l), b_l (Divisor)               */
/*  Output:    c_l (Remainder a_l mod b_l)                                    */
/*  Returns:   sign of remainder = 1                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
smod (CLINT a_l, int sign_a, CLINT b_l, CLINT c_l)
{
  CLINT q_l, r_l;

  if (EQZ_L (b_l))
    {
      return E_CLINT_DBZ;
    }

  div_l (a_l, b_l, q_l, r_l);

  if ((-1 == sign_a) && GTZ_L (r_l))
    {
      sub_l (b_l, r_l, r_l);
    }

  cpy_l (c_l, r_l);

  /* Purging of variables */
  PURGEVARS_L ((2, sizeof (q_l), q_l,
                   sizeof (r_l), r_l));

  ISPURGED_L  ((2, sizeof (q_l), q_l,
                   sizeof (r_l), r_l));

  return 1;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Sieve of Eratosthenes                                          */
/*  Syntax:    ULONG * genprimes (ULONG N);                                   */
/*  Input:     N (Upper bound for prime numbers to be generated)              */
/*  Output:    -                                                              */
/*  Returns:   Pointer to array of type ULONG, containing prime numbers <= N  */
/*             Array position 0 stores number of primes found                 */
/*             NULL: Memory allocation error                                  */
/*                                                                            */
/******************************************************************************/
ULONG * __FLINT_API
genprimes (ULONG N)
{
  ULONG i, k, p, s, L, B, count;
  char *f;
  ULONG *primes;

  B = (1 + ul_iroot (N)) >> 1;
  L = N >> 1;
  if (((N&1) == 0) && (N > 0))
    {
      --L;
    }

  if ((f = (char *)malloc ((size_t)L+1)) == NULL)
    {
      return (ULONG *)NULL;
    }

  for (i = 1; i <= L; i++)
    {
      f[i] = 1;
    }

  p = 3;
  s = 4;
  for (i = 1; i <= B; i++)
    {
      if (f[i])
        {
          for (k = s; k <= L; k += p)
            {
              f[k] = 0;
            }
        }
      s += p + p + 2;
      p += 2;
    }

  for (count = i = 1; i <= L; i++)
    {
      count += f[i];
    }

  if ((primes = (ULONG*)malloc ((size_t)(count+1) * sizeof (ULONG))) == NULL)
    {
      free (f);
      return (ULONG *)NULL;
    }

  for (count = i = 1; i <= L; i++)
    {
      if (f[i])
        {
          ++count;
          primes[count] = (i << 1) + 1;      /*lint !e796 !e797 */
        }
    }

  if (N < 2)
    {
      primes[0] = 0;
    }
  else
    {
      primes[0] = count;
      primes[1] = 2;                         /*lint !e796 */
    }
  free (f);

  return primes;
}



/******************************************************************************/
/*                                                                            */
/*                              Auxiliary Functions                           */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*  Function:  Integer square root of a number of type ULONG                  */
/*  Syntax:    ULONG ul_iroot (ULONG n);                                      */
/*  Input:     n                                                              */
/*  Output:    -                                                              */
/*  Returns:   Integer square root of n                                       */
/*                                                                            */
/******************************************************************************/
static ULONG
ul_iroot (ULONG n)
{
  ULONG x = 1, y = 0;
  if (0 == n)
    {
      return 0;
    }

  while (y <= n)
    {
      x = x << 1;
      y = x * x;
    }
  do
    {
      y = x;
      x = (x * x + n) / (2 * x);
    }
  while (x < y);

  y = 0;
  return x;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Testing and setting of a single bit                            */
/*  Syntax:    int setbit (CLINT a_l, unsigned int pos);                      */
/*  Input:     a_l (Argument),                                                */
/*             pos (Position of the bit to be set in a_l, leftmost position   */
/*             is 0)                                                          */
/*  Output:    a_l, bit in position pos set to 1                              */
/*  Returns:   1: bit in position pos had value 1 before it was set           */
/*             0: else                                                        */
/*                                                                            */
/******************************************************************************/
static int 
setbit (CLINT a_l, unsigned int pos)
{
  int res = 0;
  unsigned int i;
  USHORT shortpos = (USHORT)(pos >> LDBITPERDGT);
  USHORT bitpos = (USHORT)(pos & (BITPERDGT - 1));
  USHORT m = (USHORT)(1U << bitpos);

  if (shortpos >= DIGITS_L (a_l))
    {
      /* Fill up with 0 to the requested bitposition */
      for (i = DIGITS_L (a_l) + 1; i <= shortpos + 1U; i++)
        {
          a_l[i] = 0;
        }

      /* Set new length */
      SETDIGITS_L (a_l, shortpos + 1);
    }

  /* Test bit */
  if (a_l[shortpos + 1] & m)
    {
      res = 1;
    }

  /* Set bit */
  a_l[shortpos + 1] |= m;

  /* Purging of variables */
  PURGEVARS_L ((4, sizeof (i), &i,
                   sizeof (shortpos), &shortpos,
                   sizeof (bitpos), &bitpos,
                   sizeof (m), &m));

  ISPURGED_L  ((4, sizeof (i), &i,
                   sizeof (shortpos), &shortpos,
                   sizeof (bitpos), &bitpos,
                   sizeof (m), &m));

  return res;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Reversal of a character string                                 */
/*  Syntax:    char *strrev_l (char *str);                                    */
/*  Input:     str (Pointer to character string)                              */
/*  Output:    str (Reversed character string)                                */
/*  Returns:   Pointer to reversed character string                           */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
strrev_l (char *str)
{
  char help;
  char *anfang = str;
  char *ende = str + strlen (str) - 1;  /* '\0' bleibt am Platz */

  for (; ende > anfang; ende--, anfang++)
    {
      help = *anfang;
      *anfang = *ende;
      *ende = help;
    }

  return str;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Safe tolower function                                          */
/*  Syntax:    int tolower_l (int c);                                         */
/*  Input:     c (ASCII character)                                            */
/*  Output:    -                                                              */
/*  Returns:   Lowercase character, if c is uppercase                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
tolower_l (int c)
{
  if (isupper (c))
    {
      return tolower (c);
    }
  else
    {
      return c;
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Safe toupper function                                          */
/*  Syntax:    int toupper_l (int c);                                         */
/*  Input:     c (ASCII character)                                            */
/*  Output:    -                                                              */
/*  Returns:   Uppercase character if c is lowercase                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
toupper_l (int c)
{
  if (islower (c))
    {
      return toupper (c);
    }
  else
    {
      return c;
    }
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a character string to lowercase                  */
/*  Syntax:    int strupr_l (char *str);                                      */
/*  Input:     str (Pointer to ASCIIZ character string)                       */
/*  Output:    Converted character string                                     */
/*  Returns:   Pointer to converted character string                          */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
strlwr_l (char *str)
{
  unsigned int i;

  for (i = 0 ; i < (unsigned int)strlen (str); i++)
    {
      str[i] = (char)tolower_l (str[i]);
    }

  return str;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Conversion of a character string to uppercase                  */
/*  Syntax:    int strupr_l (char *str);                                      */
/*  Input:     str (Pointer to ASCIIZ character string)                       */
/*  Output:    Converted character string                                     */
/*  Returns:   Pointer to converted character string                          */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
strupr_l (char *str)
{
  unsigned int i;
  for (i = 0 ; i < (unsigned int)strlen (str) ; i++)
    {
      str[i] = (char)toupper_l (str[i]);
    }
  return str;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Representation of a value of type ULONG as a character string  */
/*  Syntax:    char *ultoa_l (char *str, ULONG ul, int base);                 */
/*  Input:     str (Pointer to buffer for character string)                   */
/*             ul (Value of type ULONG to be represented as string)           */
/*             base (Base for the representation, 2 <= base <= 16)            */
/*  Output:    ASCIIZ string with character representation of ul to base      */
/*  Returns:   Pointer str to buffer                                          */
/*             NULL, if base < 2 or base > 16                                 */
/*                                                                            */
/******************************************************************************/
char * __FLINT_API
ultoa_l (char *str, ULONG ul, int base)
{
  int i = 0, j;

  if ((base < 2) || (base > 16))
    {
      return (char *)NULL;
    }

  do
    {
      j = ul % (unsigned int)(base + '0');
      if (j > (int)'9')
        {
          j += (int)('a' - '9') - 1;
        }
      str[i++] = j;
    }
  while ((ul /= (unsigned int)base) != 0);
  str[i] = '\0';
  return (strrev_l (str));
}


#ifdef FLINT_SECURE
#include <stdarg.h>
/******************************************************************************/
/*                                                                            */
/*  Function:  Purging of variables                                           */
/*             This function can be called for purging variables by           */
/*             overwriting with 0. This works even in cases where compiler    */
/*             optimization is used, which typically causes assignments to    */
/*             variables or calls to memset() for automatic CLINT variables   */
/*             at the end of of a function to be ignored.                     */
/*  Syntax:    static void purgevars_l (int noofvars, ...);                   */
/*  Input:     noofvars (Number of following pairs of arguments)              */
/*             ... (noofvars pairs of (sizeof (var), (type *)var))            */
/*  Output:    purged variable *var                                           */
/*  Returns:   -                                                              */
/*                                                                            */
/******************************************************************************/
static void purgevars_l (int noofvars, ...)
{
  va_list ap;
  size_t size;
  va_start (ap, noofvars);
  for (; noofvars > 0; --noofvars)
    {
      switch (size = va_arg (ap, size_t))
        {
          case 1:  *va_arg (ap, char *) = 0;
                   break;
          case 2:  *va_arg (ap, short *) = 0;
                   break;
          case 4:  *va_arg (ap, long *) = 0;
                   break;
          default: Assert (size >= CLINTMAXBYTE);
                   memset (va_arg(ap, char *), 0, size);
        }
    }
  va_end (ap);
}

#ifdef FLINT_DEBUG
/******************************************************************************/
/*                                                                            */
/*  Function:  Check whether variables are purged with 0                      */
/*  Syntax:    static int ispurged_l (int noofvars, ...);                     */
/*  Input:     noofvars (Number of followign pairs of arguments)              */
/*             ... (noofvars pairs of (sizeof (var), (typ *)var))             */
/*  Output:    -                                                              */
/*  Returns:   1 if the arguments are purged with 0                           */
/*             0 else                                                         */
/*                                                                            */
/******************************************************************************/
static int ispurged_l (int noofvars, ...)
{
  va_list ap;
  size_t size;
  char *cptr;
  va_start (ap, noofvars);
  for (; noofvars > 0; --noofvars)
    {
      size = va_arg (ap, size_t);
      cptr = va_arg(ap, char *);
      for (; size > 0; size--)
        {
          if (0 != *cptr++) return 0;
        }
    }
  va_end (ap);
  return 1;
}
#endif /* FLINT_DEBUG */
#endif /* FLINT_SECURE */

/******************************************************************************/

/* Differences of the first 6542 prime numbers                                */
/* 2, 2 + 1 = 3, 3 + 2 = 5, 5 + 2 = 7, 7 + 4 = 11 etc until 65519 + 2 = 65521 */

USHORT __FLINT_API_DATA smallprimes[NOOFSMALLPRIMES] =
{   2, 1, 2, 2, 4, 2, 4, 2, 4, 6, 2, 6, 4, 2, 4, 6, 6, 2, 6, 4, 2,
    6, 4, 6, 8, 4, 2, 4, 2, 4, 14, 4, 6, 2, 10, 2, 6, 6, 4, 6, 6,
    2, 10, 2, 4, 2, 12, 12, 4, 2, 4, 6, 2, 10, 6, 6, 6, 2, 6, 4, 2,
    10, 14, 4, 2, 4, 14, 6, 10, 2, 4, 6, 8, 6, 6, 4, 6, 8, 4, 8, 10,
    2, 10, 2, 6, 4, 6, 8, 4, 2, 4, 12, 8, 4, 8, 4, 6, 12, 2, 18, 6,
    10, 6, 6, 2, 6, 10, 6, 6, 2, 6, 6, 4, 2, 12, 10, 2, 4, 6, 6, 2,
    12, 4, 6, 8, 10, 8, 10, 8, 6, 6, 4, 8, 6, 4, 8, 4, 14, 10, 12, 2,
    10, 2, 4, 2, 10, 14, 4, 2, 4, 14, 4, 2, 4, 20, 4, 8, 10, 8, 4, 6,
    6, 14, 4, 6, 6, 8, 6, 12, 4, 6, 2, 10, 2, 6, 10, 2, 10, 2, 6, 18,
    4, 2, 4, 6, 6, 8, 6, 6, 22, 2, 10, 8, 10, 6, 6, 8, 12, 4, 6, 6,
    2, 6, 12, 10, 18, 2, 4, 6, 2, 6, 4, 2, 4, 12, 2, 6, 34, 6, 6, 8,
    18, 10, 14, 4, 2, 4, 6, 8, 4, 2, 6, 12, 10, 2, 4, 2, 4, 6, 12, 12,
    8, 12, 6, 4, 6, 8, 4, 8, 4, 14, 4, 6, 2, 4, 6, 2, 6, 10, 20, 6,
    4, 2, 24, 4, 2, 10, 12, 2, 10, 8, 6, 6, 6, 18, 6, 4, 2, 12, 10, 12,
    8, 16, 14, 6, 4, 2, 4, 2, 10, 12, 6, 6, 18, 2, 16, 2, 22, 6, 8, 6,
    4, 2, 4, 8, 6, 10, 2, 10, 14, 10, 6, 12, 2, 4, 2, 10, 12, 2, 16, 2,
    6, 4, 2, 10, 8, 18, 24, 4, 6, 8, 16, 2, 4, 8, 16, 2, 4, 8, 6, 6,
    4, 12, 2, 22, 6, 2, 6, 4, 6, 14, 6, 4, 2, 6, 4, 6, 12, 6, 6, 14,
    4, 6, 12, 8, 6, 4, 26, 18, 10, 8, 4, 6, 2, 6, 22, 12, 2, 16, 8, 4,
    12, 14, 10, 2, 4, 8, 6, 6, 4, 2, 4, 6, 8, 4, 2, 6, 10, 2, 10, 8,
    4, 14, 10, 12, 2, 6, 4, 2, 16, 14, 4, 6, 8, 6, 4, 18, 8, 10, 6, 6,
    8, 10, 12, 14, 4, 6, 6, 2, 28, 2, 10, 8, 4, 14, 4, 8, 12, 6, 12, 4,
    6, 20, 10, 2, 16, 26, 4, 2, 12, 6, 4, 12, 6, 8, 4, 8, 22, 2, 4, 2,
    12, 28, 2, 6, 6, 6, 4, 6, 2, 12, 4, 12, 2, 10, 2, 16, 2, 16, 6, 20,
    16, 8, 4, 2, 4, 2, 22, 8, 12, 6, 10, 2, 4, 6, 2, 6, 10, 2, 12, 10,
    2, 10, 14, 6, 4, 6, 8, 6, 6, 16, 12, 2, 4, 14, 6, 4, 8, 10, 8, 6,
    6, 22, 6, 2, 10, 14, 4, 6, 18, 2, 10, 14, 4, 2, 10, 14, 4, 8, 18, 4,
    6, 2, 4, 6, 2, 12, 4, 20, 22, 12, 2, 4, 6, 6, 2, 6, 22, 2, 6, 16,
    6, 12, 2, 6, 12, 16, 2, 4, 6, 14, 4, 2, 18, 24, 10, 6, 2, 10, 2, 10,
    2, 10, 6, 2, 10, 2, 10, 6, 8, 30, 10, 2, 10, 8, 6, 10, 18, 6, 12, 12,
    2, 18, 6, 4, 6, 6, 18, 2, 10, 14, 6, 4, 2, 4, 24, 2, 12, 6, 16, 8,
    6, 6, 18, 16, 2, 4, 6, 2, 6, 6, 10, 6, 12, 12, 18, 2, 6, 4, 18, 8,
    24, 4, 2, 4, 6, 2, 12, 4, 14, 30, 10, 6, 12, 14, 6, 10, 12, 2, 4, 6,
    8, 6, 10, 2, 4, 14, 6, 6, 4, 6, 2, 10, 2, 16, 12, 8, 18, 4, 6, 12,
    2, 6, 6, 6, 28, 6, 14, 4, 8, 10, 8, 12, 18, 4, 2, 4, 24, 12, 6, 2,
    16, 6, 6, 14, 10, 14, 4, 30, 6, 6, 6, 8, 6, 4, 2, 12, 6, 4, 2, 6,
    22, 6, 2, 4, 18, 2, 4, 12, 2, 6, 4, 26, 6, 6, 4, 8, 10, 32, 16, 2,
    6, 4, 2, 4, 2, 10, 14, 6, 4, 8, 10, 6, 20, 4, 2, 6, 30, 4, 8, 10,
    6, 6, 8, 6, 12, 4, 6, 2, 6, 4, 6, 2, 10, 2, 16, 6, 20, 4, 12, 14,
    28, 6, 20, 4, 18, 8, 6, 4, 6, 14, 6, 6, 10, 2, 10, 12, 8, 10, 2, 10,
    8, 12, 10, 24, 2, 4, 8, 6, 4, 8, 18, 10, 6, 6, 2, 6, 10, 12, 2, 10,
    6, 6, 6, 8, 6, 10, 6, 2, 6, 6, 6, 10, 8, 24, 6, 22, 2, 18, 4, 8,
    10, 30, 8, 18, 4, 2, 10, 6, 2, 6, 4, 18, 8, 12, 18, 16, 6, 2, 12, 6,
    10, 2, 10, 2, 6, 10, 14, 4, 24, 2, 16, 2, 10, 2, 10, 20, 4, 2, 4, 8,
    16, 6, 6, 2, 12, 16, 8, 4, 6, 30, 2, 10, 2, 6, 4, 6, 6, 8, 6, 4,
    12, 6, 8, 12, 4, 14, 12, 10, 24, 6, 12, 6, 2, 22, 8, 18, 10, 6, 14, 4,
    2, 6, 10, 8, 6, 4, 6, 30, 14, 10, 2, 12, 10, 2, 16, 2, 18, 24, 18, 6,
    16, 18, 6, 2, 18, 4, 6, 2, 10, 8, 10, 6, 6, 8, 4, 6, 2, 10, 2, 12,
    4, 6, 6, 2, 12, 4, 14, 18, 4, 6, 20, 4, 8, 6, 4, 8, 4, 14, 6, 4,
    14, 12, 4, 2, 30, 4, 24, 6, 6, 12, 12, 14, 6, 4, 2, 4, 18, 6, 12, 8,
    6, 4, 12, 2, 12, 30, 16, 2, 6, 22, 14, 6, 10, 12, 6, 2, 4, 8, 10, 6,
    6, 24, 14, 6, 4, 8, 12, 18, 10, 2, 10, 2, 4, 6, 20, 6, 4, 14, 4, 2,
    4, 14, 6, 12, 24, 10, 6, 8, 10, 2, 30, 4, 6, 2, 12, 4, 14, 6, 34, 12,
    8, 6, 10, 2, 4, 20, 10, 8, 16, 2, 10, 14, 4, 2, 12, 6, 16, 6, 8, 4,
    8, 4, 6, 8, 6, 6, 12, 6, 4, 6, 6, 8, 18, 4, 20, 4, 12, 2, 10, 6,
    2, 10, 12, 2, 4, 20, 6, 30, 6, 4, 8, 10, 12, 6, 2, 28, 2, 6, 4, 2,
    16, 12, 2, 6, 10, 8, 24, 12, 6, 18, 6, 4, 14, 6, 4, 12, 8, 6, 12, 4,
    6, 12, 6, 12, 2, 16, 20, 4, 2, 10, 18, 8, 4, 14, 4, 2, 6, 22, 6, 14,
    6, 6, 10, 6, 2, 10, 2, 4, 2, 22, 2, 4, 6, 6, 12, 6, 14, 10, 12, 6,
    8, 4, 36, 14, 12, 6, 4, 6, 2, 12, 6, 12, 16, 2, 10, 8, 22, 2, 12, 6,
    4, 6, 18, 2, 12, 6, 4, 12, 8, 6, 12, 4, 6, 12, 6, 2, 12, 12, 4, 14,
    6, 16, 6, 2, 10, 8, 18, 6, 34, 2, 28, 2, 22, 6, 2, 10, 12, 2, 6, 4,
    8, 22, 6, 2, 10, 8, 4, 6, 8, 4, 12, 18, 12, 20, 4, 6, 6, 8, 4, 2,
    16, 12, 2, 10, 8, 10, 2, 4, 6, 14, 12, 22, 8, 28, 2, 4, 20, 4, 2, 4,
    14, 10, 12, 2, 12, 16, 2, 28, 8, 22, 8, 4, 6, 6, 14, 4, 8, 12, 6, 6,
    4, 20, 4, 18, 2, 12, 6, 4, 6, 14, 18, 10, 8, 10, 32, 6, 10, 6, 6, 2,
    6, 16, 6, 2, 12, 6, 28, 2, 10, 8, 16, 6, 8, 6, 10, 24, 20, 10, 2, 10,
    2, 12, 4, 6, 20, 4, 2, 12, 18, 10, 2, 10, 2, 4, 20, 16, 26, 4, 8, 6,
    4, 12, 6, 8, 12, 12, 6, 4, 8, 22, 2, 16, 14, 10, 6, 12, 12, 14, 6, 4,
    20, 4, 12, 6, 2, 6, 6, 16, 8, 22, 2, 28, 8, 6, 4, 20, 4, 12, 24, 20,
    4, 8, 10, 2, 16, 2, 12, 12, 34, 2, 4, 6, 12, 6, 6, 8, 6, 4, 2, 6,
    24, 4, 20, 10, 6, 6, 14, 4, 6, 6, 2, 12, 6, 10, 2, 10, 6, 20, 4, 26,
    4, 2, 6, 22, 2, 24, 4, 6, 2, 4, 6, 24, 6, 8, 4, 2, 34, 6, 8, 16,
    12, 2, 10, 2, 10, 6, 8, 4, 8, 12, 22, 6, 14, 4, 26, 4, 2, 12, 10, 8,
    4, 8, 12, 4, 14, 6, 16, 6, 8, 4, 6, 6, 8, 6, 10, 12, 2, 6, 6, 16,
    8, 6, 6, 12, 10, 2, 6, 18, 4, 6, 6, 6, 12, 18, 8, 6, 10, 8, 18, 4,
    14, 6, 18, 10, 8, 10, 12, 2, 6, 12, 12, 36, 4, 6, 8, 4, 6, 2, 4, 18,
    12, 6, 8, 6, 6, 4, 18, 2, 4, 2, 24, 4, 6, 6, 14, 30, 6, 4, 6, 12,
    6, 20, 4, 8, 4, 8, 6, 6, 4, 30, 2, 10, 12, 8, 10, 8, 24, 6, 12, 4,
    14, 4, 6, 2, 28, 14, 16, 2, 12, 6, 4, 20, 10, 6, 6, 6, 8, 10, 12, 14,
    10, 14, 16, 14, 10, 14, 6, 16, 6, 8, 6, 16, 20, 10, 2, 6, 4, 2, 4, 12,
    2, 10, 2, 6, 22, 6, 2, 4, 18, 8, 10, 8, 22, 2, 10, 18, 14, 4, 2, 4,
    18, 2, 4, 6, 8, 10, 2, 30, 4, 30, 2, 10, 2, 18, 4, 18, 6, 14, 10, 2,
    4, 20, 36, 6, 4, 6, 14, 4, 20, 10, 14, 22, 6, 2, 30, 12, 10, 18, 2, 4,
    14, 6, 22, 18, 2, 12, 6, 4, 8, 4, 8, 6, 10, 2, 12, 18, 10, 14, 16, 14,
    4, 6, 6, 2, 6, 4, 2, 28, 2, 28, 6, 2, 4, 6, 14, 4, 12, 14, 16, 14,
    4, 6, 8, 6, 4, 6, 6, 6, 8, 4, 8, 4, 14, 16, 8, 6, 4, 12, 8, 16,
    2, 10, 8, 4, 6, 26, 6, 10, 8, 4, 6, 12, 14, 30, 4, 14, 22, 8, 12, 4,
    6, 8, 10, 6, 14, 10, 6, 2, 10, 12, 12, 14, 6, 6, 18, 10, 6, 8, 18, 4,
    6, 2, 6, 10, 2, 10, 8, 6, 6, 10, 2, 18, 10, 2, 12, 4, 6, 8, 10, 12,
    14, 12, 4, 8, 10, 6, 6, 20, 4, 14, 16, 14, 10, 8, 10, 12, 2, 18, 6, 12,
    10, 12, 2, 4, 2, 12, 6, 4, 8, 4, 44, 4, 2, 4, 2, 10, 12, 6, 6, 14,
    4, 6, 6, 6, 8, 6, 36, 18, 4, 6, 2, 12, 6, 6, 6, 4, 14, 22, 12, 2,
    18, 10, 6, 26, 24, 4, 2, 4, 2, 4, 14, 4, 6, 6, 8, 16, 12, 2, 42, 4,
    2, 4, 24, 6, 6, 2, 18, 4, 14, 6, 28, 18, 14, 6, 10, 12, 2, 6, 12, 30,
    6, 4, 6, 6, 14, 4, 2, 24, 4, 6, 6, 26, 10, 18, 6, 8, 6, 6, 30, 4,
    12, 12, 2, 16, 2, 6, 4, 12, 18, 2, 6, 4, 26, 12, 6, 12, 4, 24, 24, 12,
    6, 2, 12, 28, 8, 4, 6, 12, 2, 18, 6, 4, 6, 6, 20, 16, 2, 6, 6, 18,
    10, 6, 2, 4, 8, 6, 6, 24, 16, 6, 8, 10, 6, 14, 22, 8, 16, 6, 2, 12,
    4, 2, 22, 8, 18, 34, 2, 6, 18, 4, 6, 6, 8, 10, 8, 18, 6, 4, 2, 4,
    8, 16, 2, 12, 12, 6, 18, 4, 6, 6, 6, 2, 6, 12, 10, 20, 12, 18, 4, 6,
    2, 16, 2, 10, 14, 4, 30, 2, 10, 12, 2, 24, 6, 16, 8, 10, 2, 12, 22, 6,
    2, 16, 20, 10, 2, 12, 12, 18, 10, 12, 6, 2, 10, 2, 6, 10, 18, 2, 12, 6,
    4, 6, 2, 24, 28, 2, 4, 2, 10, 2, 16, 12, 8, 22, 2, 6, 4, 2, 10, 6,
    20, 12, 10, 8, 12, 6, 6, 6, 4, 18, 2, 4, 12, 18, 2, 12, 6, 4, 2, 16,
    12, 12, 14, 4, 8, 18, 4, 12, 14, 6, 6, 4, 8, 6, 4, 20, 12, 10, 14, 4,
    2, 16, 2, 12, 30, 4, 6, 24, 20, 24, 10, 8, 12, 10, 12, 6, 12, 12, 6, 8,
    16, 14, 6, 4, 6, 36, 20, 10, 30, 12, 2, 4, 2, 28, 12, 14, 6, 22, 8, 4,
    18, 6, 14, 18, 4, 6, 2, 6, 34, 18, 2, 16, 6, 18, 2, 24, 4, 2, 6, 12,
    6, 12, 10, 8, 6, 16, 12, 8, 10, 14, 40, 6, 2, 6, 4, 12, 14, 4, 2, 4,
    2, 4, 8, 6, 10, 6, 6, 2, 6, 6, 6, 12, 6, 24, 10, 2, 10, 6, 12, 6,
    6, 14, 6, 6, 52, 20, 6, 10, 2, 10, 8, 10, 12, 12, 2, 6, 4, 14, 16, 8,
    12, 6, 22, 2, 10, 8, 6, 22, 2, 22, 6, 8, 10, 12, 12, 2, 10, 6, 12, 2,
    4, 14, 10, 2, 6, 18, 4, 12, 8, 18, 12, 6, 6, 4, 6, 6, 14, 4, 2, 12,
    12, 4, 6, 18, 18, 12, 2, 16, 12, 8, 18, 10, 26, 4, 6, 8, 6, 6, 4, 2,
    10, 20, 4, 6, 8, 4, 20, 10, 2, 34, 2, 4, 24, 2, 12, 12, 10, 6, 2, 12,
    30, 6, 12, 16, 12, 2, 22, 18, 12, 14, 10, 2, 12, 12, 4, 2, 4, 6, 12, 2,
    16, 18, 2, 40, 8, 16, 6, 8, 10, 2, 4, 18, 8, 10, 8, 12, 4, 18, 2, 18,
    10, 2, 4, 2, 4, 8, 28, 2, 6, 22, 12, 6, 14, 18, 4, 6, 8, 6, 6, 10,
    8, 4, 2, 18, 10, 6, 20, 22, 8, 6, 30, 4, 2, 4, 18, 6, 30, 2, 4, 8,
    6, 4, 6, 12, 14, 34, 14, 6, 4, 2, 6, 4, 14, 4, 2, 6, 28, 2, 4, 6,
    8, 10, 2, 10, 2, 10, 2, 4, 30, 2, 12, 12, 10, 18, 12, 14, 10, 2, 12, 6,
    10, 6, 14, 12, 4, 14, 4, 18, 2, 10, 8, 4, 8, 10, 12, 18, 18, 8, 6, 18,
    16, 14, 6, 6, 10, 14, 4, 6, 2, 12, 12, 4, 6, 6, 12, 2, 16, 2, 12, 6,
    4, 14, 6, 4, 2, 12, 18, 4, 36, 18, 12, 12, 2, 4, 2, 4, 8, 12, 4, 36,
    6, 18, 2, 12, 10, 6, 12, 24, 8, 6, 6, 16, 12, 2, 18, 10, 20, 10, 2, 6,
    18, 4, 2, 40, 6, 2, 16, 2, 4, 8, 18, 10, 12, 6, 2, 10, 8, 4, 6, 12,
    2, 10, 18, 8, 6, 4, 20, 4, 6, 36, 6, 2, 10, 6, 24, 6, 14, 16, 6, 18,
    2, 10, 20, 10, 8, 6, 4, 6, 2, 10, 2, 12, 4, 2, 4, 8, 10, 6, 12, 18,
    14, 12, 16, 8, 6, 16, 8, 4, 2, 6, 18, 24, 18, 10, 12, 2, 4, 14, 10, 6,
    6, 6, 18, 12, 2, 28, 18, 14, 16, 12, 14, 24, 12, 22, 6, 2, 10, 8, 4, 2,
    4, 14, 12, 6, 4, 6, 14, 4, 2, 4, 30, 6, 2, 6, 10, 2, 30, 22, 2, 4,
    6, 8, 6, 6, 16, 12, 12, 6, 8, 4, 2, 24, 12, 4, 6, 8, 6, 6, 10, 2,
    6, 12, 28, 14, 6, 4, 12, 8, 6, 12, 4, 6, 14, 6, 12, 10, 6, 6, 8, 6,
    6, 4, 2, 4, 8, 12, 4, 14, 18, 10, 2, 16, 6, 20, 6, 10, 8, 4, 30, 36,
    12, 8, 22, 12, 2, 6, 12, 16, 6, 6, 2, 18, 4, 26, 4, 8, 18, 10, 8, 10,
    6, 14, 4, 20, 22, 18, 12, 8, 28, 12, 6, 6, 8, 6, 12, 24, 16, 14, 4, 14,
    12, 6, 10, 12, 20, 6, 4, 8, 18, 12, 18, 10, 2, 4, 20, 10, 14, 4, 6, 2,
    10, 24, 18, 2, 4, 20, 16, 14, 10, 14, 6, 4, 6, 20, 6, 10, 6, 2, 12, 6,
    30, 10, 8, 6, 4, 6, 8, 40, 2, 4, 2, 12, 18, 4, 6, 8, 10, 6, 18, 18,
    2, 12, 16, 8, 6, 4, 6, 6, 2, 52, 14, 4, 20, 16, 2, 4, 6, 12, 2, 6,
    12, 12, 6, 4, 14, 10, 6, 6, 14, 10, 14, 16, 8, 6, 12, 4, 8, 22, 6, 2,
    18, 22, 6, 2, 18, 6, 16, 14, 10, 6, 12, 2, 6, 4, 8, 18, 12, 16, 2, 4,
    14, 4, 8, 12, 12, 30, 16, 8, 4, 2, 6, 22, 12, 8, 10, 6, 6, 6, 14, 6,
    18, 10, 12, 2, 10, 2, 4, 26, 4, 12, 8, 4, 18, 8, 10, 14, 16, 6, 6, 8,
    10, 6, 8, 6, 12, 10, 20, 10, 8, 4, 12, 26, 18, 4, 12, 18, 6, 30, 6, 8,
    6, 22, 12, 2, 4, 6, 6, 2, 10, 2, 4, 6, 6, 2, 6, 22, 18, 6, 18, 12,
    8, 12, 6, 10, 12, 2, 16, 2, 10, 2, 10, 18, 6, 20, 4, 2, 6, 22, 6, 6,
    18, 6, 14, 12, 16, 2, 6, 6, 4, 14, 12, 4, 2, 18, 16, 36, 12, 6, 14, 28,
    2, 12, 6, 12, 6, 4, 2, 16, 30, 8, 24, 6, 30, 10, 2, 18, 4, 6, 12, 8,
    22, 2, 6, 22, 18, 2, 10, 2, 10, 30, 2, 28, 6, 14, 16, 6, 20, 16, 2, 6,
    4, 32, 4, 2, 4, 6, 2, 12, 4, 6, 6, 12, 2, 6, 4, 6, 8, 6, 4, 20,
    4, 32, 10, 8, 16, 2, 22, 2, 4, 6, 8, 6, 16, 14, 4, 18, 8, 4, 20, 6,
    12, 12, 6, 10, 2, 10, 2, 12, 28, 12, 18, 2, 18, 10, 8, 10, 48, 2, 4, 6,
    8, 10, 2, 10, 30, 2, 36, 6, 10, 6, 2, 18, 4, 6, 8, 16, 14, 16, 6, 14,
    4, 20, 4, 6, 2, 10, 12, 2, 6, 12, 6, 6, 4, 12, 2, 6, 4, 12, 6, 8,
    4, 2, 6, 18, 10, 6, 8, 12, 6, 22, 2, 6, 12, 18, 4, 14, 6, 4, 20, 6,
    16, 8, 4, 8, 22, 8, 12, 6, 6, 16, 12, 18, 30, 8, 4, 2, 4, 6, 26, 4,
    14, 24, 22, 6, 2, 6, 10, 6, 14, 6, 6, 12, 10, 6, 2, 12, 10, 12, 8, 18,
    18, 10, 6, 8, 16, 6, 6, 8, 16, 20, 4, 2, 10, 2, 10, 12, 6, 8, 6, 10,
    20, 10, 18, 26, 4, 6, 30, 2, 4, 8, 6, 12, 12, 18, 4, 8, 22, 6, 2, 12,
    34, 6, 18, 12, 6, 2, 28, 14, 16, 14, 4, 14, 12, 4, 6, 6, 2, 36, 4, 6,
    20, 12, 24, 6, 22, 2, 16, 18, 12, 12, 18, 2, 6, 6, 6, 4, 6, 14, 4, 2,
    22, 8, 12, 6, 10, 6, 8, 12, 18, 12, 6, 10, 2, 22, 14, 6, 6, 4, 18, 6,
    20, 22, 2, 12, 24, 4, 18, 18, 2, 22, 2, 4, 12, 8, 12, 10, 14, 4, 2, 18,
    16, 38, 6, 6, 6, 12, 10, 6, 12, 8, 6, 4, 6, 14, 30, 6, 10, 8, 22, 6,
    8, 12, 10, 2, 10, 2, 6, 10, 2, 10, 12, 18, 20, 6, 4, 8, 22, 6, 6, 30,
    6, 14, 6, 12, 12, 6, 10, 2, 10, 30, 2, 16, 8, 4, 2, 6, 18, 4, 2, 6,
    4, 26, 4, 8, 6, 10, 2, 4, 6, 8, 4, 6, 30, 12, 2, 6, 6, 4, 20, 22,
    8, 4, 2, 4, 72, 8, 4, 8, 22, 2, 4, 14, 10, 2, 4, 20, 6, 10, 18, 6,
    20, 16, 6, 8, 6, 4, 20, 12, 22, 2, 4, 2, 12, 10, 18, 2, 22, 6, 18, 30,
    2, 10, 14, 10, 8, 16, 50, 6, 10, 8, 10, 12, 6, 18, 2, 22, 6, 2, 4, 6,
    8, 6, 6, 10, 18, 2, 22, 2, 16, 14, 10, 6, 2, 12, 10, 20, 4, 14, 6, 4,
    36, 2, 4, 6, 12, 2, 4, 14, 12, 6, 4, 6, 2, 6, 4, 20, 10, 2, 10, 6,
    12, 2, 24, 12, 12, 6, 6, 4, 24, 2, 4, 24, 2, 6, 4, 6, 8, 16, 6, 2,
    10, 12, 14, 6, 34, 6, 14, 6, 4, 2, 30, 22, 8, 4, 6, 8, 4, 2, 28, 2,
    6, 4, 26, 18, 22, 2, 6, 16, 6, 2, 16, 12, 2, 12, 4, 6, 6, 14, 10, 6,
    8, 12, 4, 18, 2, 10, 8, 16, 6, 6, 30, 2, 10, 18, 2, 10, 8, 4, 8, 12,
    24, 40, 2, 12, 10, 6, 12, 2, 12, 4, 2, 4, 6, 18, 14, 12, 6, 4, 14, 30,
    4, 8, 10, 8, 6, 10, 18, 8, 4, 14, 16, 6, 8, 4, 6, 2, 10, 2, 12, 4,
    2, 4, 6, 8, 4, 6, 32, 24, 10, 8, 18, 10, 2, 6, 10, 2, 4, 18, 6, 12,
    2, 16, 2, 22, 6, 6, 8, 18, 4, 18, 12, 8, 6, 4, 20, 6, 30, 22, 12, 2,
    6, 18, 4, 62, 4, 2, 12, 6, 10, 2, 12, 12, 28, 2, 4, 14, 22, 6, 2, 6,
    6, 10, 14, 4, 2, 10, 6, 8, 10, 14, 10, 6, 2, 12, 22, 18, 8, 10, 18, 12,
    2, 12, 4, 12, 2, 10, 2, 6, 18, 6, 6, 34, 6, 2, 12, 4, 6, 18, 18, 2,
    16, 6, 6, 8, 6, 10, 18, 8, 10, 8, 10, 2, 4, 18, 26, 12, 22, 2, 4, 2,
    22, 6, 6, 14, 16, 6, 20, 10, 12, 2, 18, 42, 4, 24, 2, 6, 10, 12, 2, 6,
    10, 8, 4, 6, 12, 12, 8, 4, 6, 12, 30, 20, 6, 24, 6, 10, 12, 2, 10, 20,
    6, 6, 4, 12, 14, 10, 18, 12, 8, 6, 12, 4, 14, 10, 2, 12, 30, 16, 2, 12,
    6, 4, 2, 4, 6, 26, 4, 18, 2, 4, 6, 14, 54, 6, 52, 2, 16, 6, 6, 12,
    26, 4, 2, 6, 22, 6, 2, 12, 12, 6, 10, 18, 2, 12, 12, 10, 18, 12, 6, 8,
    6, 10, 6, 8, 4, 2, 4, 20, 24, 6, 6, 10, 14, 10, 2, 22, 6, 14, 10, 26,
    4, 18, 8, 12, 12, 10, 12, 6, 8, 16, 6, 8, 6, 6, 22, 2, 10, 20, 10, 6,
    44, 18, 6, 10, 2, 4, 6, 14, 4, 26, 4, 2, 12, 10, 8, 4, 8, 12, 4, 12,
    8, 22, 8, 6, 10, 18, 6, 6, 8, 6, 12, 4, 8, 18, 10, 12, 6, 12, 2, 6,
    4, 2, 16, 12, 12, 14, 10, 14, 6, 10, 12, 2, 12, 6, 4, 6, 2, 12, 4, 26,
    6, 18, 6, 10, 6, 2, 18, 10, 8, 4, 26, 10, 20, 6, 16, 20, 12, 10, 8, 10,
    2, 16, 6, 20, 10, 20, 4, 30, 2, 4, 8, 16, 2, 18, 4, 2, 6, 10, 18, 12,
    14, 18, 6, 16, 20, 6, 4, 8, 6, 4, 6, 12, 8, 10, 2, 12, 6, 4, 2, 6,
    10, 2, 16, 12, 14, 10, 6, 8, 6, 28, 2, 6, 18, 30, 34, 2, 16, 12, 2, 18,
    16, 6, 8, 10, 8, 10, 8, 10, 44, 6, 6, 4, 20, 4, 2, 4, 14, 28, 8, 6,
    16, 14, 30, 6, 30, 4, 14, 10, 6, 6, 8, 4, 18, 12, 6, 2, 22, 12, 8, 6,
    12, 4, 14, 4, 6, 2, 4, 18, 20, 6, 16, 38, 16, 2, 4, 6, 2, 40, 42, 14,
    4, 6, 2, 24, 10, 6, 2, 18, 10, 12, 2, 16, 2, 6, 16, 6, 8, 4, 2, 10,
    6, 8, 10, 2, 18, 16, 8, 12, 18, 12, 6, 12, 10, 6, 6, 18, 12, 14, 4, 2,
    10, 20, 6, 12, 6, 16, 26, 4, 18, 2, 4, 32, 10, 8, 6, 4, 6, 6, 14, 6,
    18, 4, 2, 18, 10, 8, 10, 8, 10, 2, 4, 6, 2, 10, 42, 8, 12, 4, 6, 18,
    2, 16, 8, 4, 2, 10, 14, 12, 10, 20, 4, 8, 10, 38, 4, 6, 2, 10, 20, 10,
    12, 6, 12, 26, 12, 4, 8, 28, 8, 4, 8, 24, 6, 10, 8, 6, 16, 12, 8, 10,
    12, 8, 22, 6, 2, 10, 2, 6, 10, 6, 6, 8, 6, 4, 14, 28, 8, 16, 18, 8,
    4, 6, 20, 4, 18, 6, 2, 24, 24, 6, 6, 12, 12, 4, 2, 22, 2, 10, 6, 8,
    12, 4, 20, 18, 6, 4, 12, 24, 6, 6, 54, 8, 6, 4, 26, 36, 4, 2, 4, 26,
    12, 12, 4, 6, 6, 8, 12, 10, 2, 12, 16, 18, 6, 8, 6, 12, 18, 10, 2, 54,
    4, 2, 10, 30, 12, 8, 4, 8, 16, 14, 12, 6, 4, 6, 12, 6, 2, 4, 14, 12,
    4, 14, 6, 24, 6, 6, 10, 12, 12, 20, 18, 6, 6, 16, 8, 4, 6, 20, 4, 32,
    4, 14, 10, 2, 6, 12, 16, 2, 4, 6, 12, 2, 10, 8, 6, 4, 2, 10, 14, 6,
    6, 12, 18, 34, 8, 10, 6, 24, 6, 2, 10, 12, 2, 30, 10, 14, 12, 12, 16, 6,
    6, 2, 18, 4, 6, 30, 14, 4, 6, 6, 2, 6, 4, 6, 14, 6, 4, 8, 10, 12,
    6, 32, 10, 8, 22, 2, 10, 6, 24, 8, 4, 30, 6, 2, 12, 16, 8, 6, 4, 6,
    8, 16, 14, 6, 6, 4, 2, 10, 12, 2, 16, 14, 4, 2, 4, 20, 18, 10, 2, 10,
    6, 12, 30, 8, 18, 12, 10, 2, 6, 6, 4, 12, 12, 2, 4, 12, 18, 24, 2, 10,
    6, 8, 16, 8, 6, 12, 10, 14, 6, 12, 6, 6, 4, 2, 24, 4, 6, 8, 6, 4,
    2, 4, 6, 14, 4, 8, 10, 24, 24, 12, 2, 6, 12, 22, 30, 2, 6, 18, 10, 6,
    6, 8, 4, 2, 6, 10, 8, 10, 6, 8, 16, 6, 14, 6, 4, 24, 8, 10, 2, 12,
    6, 4, 36, 2, 22, 6, 8, 6, 10, 8, 6, 12, 10, 14, 10, 6, 18, 12, 2, 12,
    4, 26, 10, 14, 16, 18, 8, 18, 12, 12, 6, 16, 14, 24, 10, 12, 8, 22, 6, 2,
    10, 60, 6, 2, 4, 8, 16, 14, 10, 6, 24, 6, 12, 18, 24, 2, 30, 4, 2, 12,
    6, 10, 2, 4, 14, 6, 16, 2, 10, 8, 22, 20, 6, 4, 32, 6, 18, 4, 2, 4,
    2, 4, 8, 52, 14, 22, 2, 22, 20, 10, 8, 10, 2, 6, 4, 14, 4, 6, 20, 4,
    6, 2, 12, 12, 6, 12, 16, 2, 12, 10, 8, 4, 6, 2, 28, 12, 8, 10, 12, 2,
    4, 14, 28, 8, 6, 4, 2, 4, 6, 2, 12, 58, 6, 14, 10, 2, 6, 28, 32, 4,
    30, 8, 6, 4, 6, 12, 12, 2, 4, 6, 6, 14, 16, 8, 30, 4, 2, 10, 8, 6,
    4, 6, 26, 4, 12, 2, 10, 18, 12, 12, 18, 2, 4, 12, 8, 12, 10, 20, 4, 8,
    16, 12, 8, 6, 16, 8, 10, 12, 14, 6, 4, 8, 12, 4, 20, 6, 40, 8, 16, 6,
    36, 2, 6, 4, 6, 2, 22, 18, 2, 10, 6, 36, 14, 12, 4, 18, 8, 4, 14, 10,
    2, 10, 8, 4, 2, 18, 16, 12, 14, 10, 14, 6, 6, 42, 10, 6, 6, 20, 10, 8,
    12, 4, 12, 18, 2, 10, 14, 18, 10, 18, 8, 6, 4, 14, 6, 10, 30, 14, 6, 6,
    4, 12, 38, 4, 2, 4, 6, 8, 12, 10, 6, 18, 6, 50, 6, 4, 6, 12, 8, 10,
    32, 6, 22, 2, 10, 12, 18, 2, 6, 4, 30, 8, 6, 6, 18, 10, 2, 4, 12, 20,
    10, 8, 24, 10, 2, 6, 22, 6, 2, 18, 10, 12, 2, 30, 18, 12, 28, 2, 6, 4,
    6, 14, 6, 12, 10, 8, 4, 12, 26, 10, 8, 6, 16, 2, 10, 18, 14, 6, 4, 6,
    14, 16, 2, 6, 4, 12, 20, 4, 20, 4, 6, 12, 2, 36, 4, 6, 2, 10, 2, 22,
    8, 6, 10, 12, 12, 18, 14, 24, 36, 4, 20, 24, 10, 6, 2, 28, 6, 18, 8, 4,
    6, 8, 6, 4, 2, 12, 28, 18, 14, 16, 14, 18, 10, 8, 6, 4, 6, 6, 8, 22,
    12, 2, 10, 18, 6, 2, 18, 10, 2, 12, 10, 18, 32, 6, 4, 6, 6, 8, 6, 6,
    10, 20, 6, 12, 10, 8, 10, 14, 6, 10, 14, 4, 2, 22, 18, 2, 10, 2, 4, 20,
    4, 2, 34, 2, 12, 6, 10, 2, 10, 18, 6, 14, 12, 12, 22, 8, 6, 16, 6, 8,
    4, 12, 6, 8, 4, 36, 6, 6, 20, 24, 6, 12, 18, 10, 2, 10, 26, 6, 16, 8,
    6, 4, 24, 18, 8, 12, 12, 10, 18, 12, 2, 24, 4, 12, 18, 12, 14, 10, 2, 4,
    24, 12, 14, 10, 6, 2, 6, 4, 6, 26, 4, 6, 6, 2, 22, 8, 18, 4, 18, 8,
    4, 24, 2, 12, 12, 4, 2, 52, 2, 18, 6, 4, 6, 12, 2, 6, 12, 10, 8, 4,
    2, 24, 10, 2, 10, 2, 12, 6, 18, 40, 6, 20, 16, 2, 12, 6, 10, 12, 2, 4,
    6, 14, 12, 12, 22, 6, 8, 4, 2, 16, 18, 12, 2, 6, 16, 6, 2, 6, 4, 12,
    30, 8, 16, 2, 18, 10, 24, 2, 6, 24, 4, 2, 22, 2, 16, 2, 6, 12, 4, 18,
    8, 4, 14, 4, 18, 24, 6, 2, 6, 10, 2, 10, 38, 6, 10, 14, 6, 6, 24, 4,
    2, 12, 16, 14, 16, 12, 2, 6, 10, 26, 4, 2, 12, 6, 4, 12, 8, 12, 10, 18,
    6, 14, 28, 2, 6, 10, 2, 4, 14, 34, 2, 6, 22, 2, 10, 14, 4, 2, 16, 8,
    10, 6, 8, 10, 8, 4, 6, 2, 16, 6, 6, 18, 30, 14, 6, 4, 30, 2, 10, 14,
    4, 20, 10, 8, 4, 8, 18, 4, 14, 6, 4, 24, 6, 6, 18, 18, 2, 36, 6, 10,
    14, 12, 4, 6, 2, 30, 6, 4, 2, 6, 28, 20, 4, 20, 12, 24, 16, 18, 12, 14,
    6, 4, 12, 32, 12, 6, 10, 8, 10, 6, 18, 2, 16, 14, 6, 22, 6, 12, 2, 18,
    4, 8, 30, 12, 4, 12, 2, 10, 38, 22, 2, 4, 14, 6, 12, 24, 4, 2, 4, 14,
    12, 10, 2, 16, 6, 20, 4, 20, 22, 12, 2, 4, 2, 12, 22, 24, 6, 6, 2, 6,
    4, 6, 2, 10, 12, 12, 6, 2, 6, 16, 8, 6, 4, 18, 12, 12, 14, 4, 12, 6,
    8, 6, 18, 6, 10, 12, 14, 6, 4, 8, 22, 6, 2, 28, 18, 2, 18, 10, 6, 14,
    10, 2, 10, 14, 6, 10, 2, 22, 6, 8, 6, 16, 12, 8, 22, 2, 4, 14, 18, 12,
    6, 24, 6, 10, 2, 12, 22, 18, 6, 20, 6, 10, 14, 4, 2, 6, 12, 22, 14, 12,
    4, 6, 8, 22, 2, 10, 12, 8, 40, 2, 6, 10, 8, 4, 42, 20, 4, 32, 12, 10,
    6, 12, 12, 2, 10, 8, 6, 4, 8, 4, 26, 18, 4, 8, 28, 6, 18, 6, 12, 2,
    10, 6, 6, 14, 10, 12, 14, 24, 6, 4, 20, 22, 2, 18, 4, 6, 12, 2, 16, 18,
    14, 6, 6, 4, 6, 8, 18, 4, 14, 30, 4, 18, 8, 10, 2, 4, 8, 12, 4, 12,
    18, 2, 12, 10, 2, 16, 8, 4, 30, 2, 6, 28, 2, 10, 2, 18, 10, 14, 4, 26,
    6, 18, 4, 20, 6, 4, 8, 18, 4, 12, 26, 24, 4, 20, 22, 2, 18, 22, 2, 4,
    12, 2, 6, 6, 6, 4, 6, 14, 4, 24, 12, 6, 18, 2, 12, 28, 14, 4, 6, 8,
    22, 6, 12, 18, 8, 4, 20, 6, 4, 6, 2, 18, 6, 4, 12, 12, 8, 28, 6, 8,
    10, 2, 24, 12, 10, 24, 8, 10, 20, 12, 6, 12, 12, 4, 14, 12, 24, 34, 18, 8,
    10, 6, 18, 8, 4, 8, 16, 14, 6, 4, 6, 24, 2, 6, 4, 6, 2, 16, 6, 6,
    20, 24, 4, 2, 4, 14, 4, 18, 2, 6, 12, 4, 14, 4, 2, 18, 16, 6, 6, 2,
    16, 20, 6, 6, 30, 4, 8, 6, 24, 16, 6, 6, 8, 12, 30, 4, 18, 18, 8, 4,
    26, 10, 2, 22, 8, 10, 14, 6, 4, 18, 8, 12, 28, 2, 6, 4, 12, 6, 24, 6,
    8, 10, 20, 16, 8, 30, 6, 6, 4, 2, 10, 14, 6, 10, 32, 22, 18, 2, 4, 2,
    4, 8, 22, 8, 18, 12, 28, 2, 16, 12, 18, 14, 10, 18, 12, 6, 32, 10, 14, 6,
    10, 2, 10, 2, 6, 22, 2, 4, 6, 8, 10, 6, 14, 6, 4, 12, 30, 24, 6, 6,
    8, 6, 4, 2, 4, 6, 8, 6, 6, 22, 18, 8, 4, 2, 18, 6, 4, 2, 16, 18,
    20, 10, 6, 6, 30, 2, 12, 28, 6, 6, 6, 2, 12, 10, 8, 18, 18, 4, 8, 18,
    10, 2, 28, 2, 10, 14, 4, 2, 30, 12, 22, 26, 10, 8, 6, 10, 8, 16, 14, 6,
    6, 10, 14, 6, 4, 2, 10, 12, 2, 6, 10, 8, 4, 2, 10, 26, 22, 6, 2, 12,
    18, 4, 26, 4, 8, 10, 6, 14, 10, 2, 18, 6, 10, 20, 6, 6, 4, 24, 2, 4,
    8, 6, 16, 14, 16, 18, 2, 4, 12, 2, 10, 2, 6, 12, 10, 6, 6, 20, 6, 4,
    6, 38, 4, 6, 12, 14, 4, 12, 8, 10, 12, 12, 8, 4, 6, 14, 10, 6, 12, 2,
    10, 18, 2, 18, 10, 8, 10, 2, 12, 4, 14, 28, 2, 16, 2, 18, 6, 10, 6, 8,
    16, 14, 30, 10, 20, 6, 10, 24, 2, 28, 2, 12, 16, 6, 8, 36, 4, 8, 4, 14,
    12, 10, 8, 12, 4, 6, 8, 4, 6, 14, 22, 8, 6, 4, 2, 10, 6, 20, 10, 8,
    6, 6, 22, 18, 2, 16, 6, 20, 4, 26, 4, 14, 22, 14, 4, 12, 6, 8, 4, 6,
    6, 26, 10, 2, 18, 18, 4, 2, 16, 2, 18, 4, 6, 8, 4, 6, 12, 2, 6, 6,
    28, 38, 4, 8, 16, 26, 4, 2, 10, 12, 2, 10, 8, 6, 10, 12, 2, 10, 2, 24,
    4, 30, 26, 6, 6, 18, 6, 6, 22, 2, 10, 18, 26, 4, 18, 8, 6, 6, 12, 16,
    6, 8, 16, 6, 8, 16, 2, 42, 58, 8, 4, 6, 2, 4, 8, 16, 6, 20, 4, 12,
    12, 6, 12, 2, 10, 2, 6, 22, 2, 10, 6, 8, 6, 10, 14, 6, 6, 4, 18, 8,
    10, 8, 16, 14, 10, 2, 10, 2, 12, 6, 4, 20, 10, 8, 52, 8, 10, 6, 2, 10,
    8, 10, 6, 6, 8, 10, 2, 22, 2, 4, 6, 14, 4, 2, 24, 12, 4, 26, 18, 4,
    6, 14, 30, 6, 4, 6, 2, 22, 8, 4, 6, 2, 22, 6, 8, 16, 6, 14, 4, 6,
    18, 8, 12, 6, 12, 24, 30, 16, 8, 34, 8, 22, 6, 14, 10, 18, 14, 4, 12, 8,
    4, 36, 6, 6, 2, 10, 2, 4, 20, 6, 6, 10, 12, 6, 2, 40, 8, 6, 28, 6,
    2, 12, 18, 4, 24, 14, 6, 6, 10, 20, 10, 14, 16, 14, 16, 6, 8, 36, 4, 12,
    12, 6, 12, 50, 12, 6, 4, 6, 6, 8, 6, 10, 2, 10, 2, 18, 10, 14, 16, 8,
    6, 4, 20, 4, 2, 10, 6, 14, 18, 10, 38, 10, 18, 2, 10, 2, 12, 4, 2, 4,
    14, 6, 10, 8, 40, 6, 20, 4, 12, 8, 6, 34, 8, 22, 8, 12, 10, 2, 16, 42,
    12, 8, 22, 8, 22, 8, 6, 34, 2, 6, 4, 14, 6, 16, 2, 22, 6, 8, 24, 22,
    6, 2, 12, 4, 6, 14, 4, 8, 24, 4, 6, 6, 2, 22, 20, 6, 4, 14, 4, 6,
    6, 8, 6, 10, 6, 8, 6, 16, 14, 6, 6, 22, 6, 24, 32, 6, 18, 6, 18, 10,
    8, 30, 18, 6, 16, 12, 6, 12, 2, 6, 4, 12, 8, 6, 22, 8, 6, 4, 14, 10,
    18, 20, 10, 2, 6, 4, 2, 28, 18, 2, 10, 6, 6, 6, 14, 40, 24, 2, 4, 8,
    12, 4, 20, 4, 32, 18, 16, 6, 36, 8, 6, 4, 6, 14, 4, 6, 26, 6, 10, 14,
    18, 10, 6, 6, 14, 10, 6, 6, 14, 6, 24, 4, 14, 22, 8, 12, 10, 8, 12, 18,
    10, 18, 8, 24, 10, 8, 4, 24, 6, 18, 6, 2, 10, 30, 2, 10, 2, 4, 2, 40,
    2, 28, 8, 6, 6, 18, 6, 10, 14, 4, 18, 30, 18, 2, 12, 30, 6, 30, 4, 18,
    12, 2, 4, 14, 6, 10, 6, 8, 6, 10, 12, 2, 6, 12, 10, 2, 18, 4, 20, 4,
    6, 14, 6, 6, 22, 6, 6, 8, 18, 18, 10, 2, 10, 2, 6, 4, 6, 12, 18, 2,
    10, 8, 4, 18, 2, 6, 6, 6, 10, 8, 10, 6, 18, 12, 8, 12, 6, 4, 6, 14,
    16, 2, 12, 4, 6, 38, 6, 6, 16, 20, 28, 20, 10, 6, 6, 14, 4, 26, 4, 14,
    10, 18, 14, 28, 2, 4, 14, 16, 2, 28, 6, 8, 6, 34, 8, 4, 18, 2, 16, 8,
    6, 40, 8, 18, 4, 30, 6, 12, 2, 30, 6, 10, 14, 40, 14, 10, 2, 12, 10, 8,
    4, 8, 6, 6, 28, 2, 4, 12, 14, 16, 8, 30, 16, 18, 2, 10, 18, 6, 32, 4,
    18, 6, 2, 12, 10, 18, 2, 6, 10, 14, 18, 28, 6, 8, 16, 2, 4, 20, 10, 8,
    18, 10, 2, 10, 8, 4, 6, 12, 6, 20, 4, 2, 6, 4, 20, 10, 26, 18, 10, 2,
    18, 6, 16, 14, 4, 26, 4, 14, 10, 12, 14, 6, 6, 4, 14, 10, 2, 30, 18, 22, 2};


