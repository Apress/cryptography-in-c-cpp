/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testmexp.c       Revision: 20.01.2002                               */
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
/*  History                                                                   */
/*                                                                            */
/*  20.01.2002:                                                               */
/*    Replaced lines                                                          */
/*      check (r3_l, one_l, i++, __LINE__);                                   */
/*    by conditional tests                                                    */
/*      if (EQONE_L(r10_l))                                                   */
/*        check (r3_l, nul_l, i++, __LINE__);                                 */
/*      else                                                                  */
/*        check (r3_l, one_l, i++, __LINE__);                                 */
/*    to care for situations where power modulo 1 == zero                     */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "flint.h"

#define NO_ASSERTS

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_alloc.h"
#endif

#include "_assert.h"


#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNS (1 + ulrand64_l() % 512)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))
#define oddrand_l(n_l,bits) do { rand_l((n_l),(bits)); n_l[1] |= 1; } while (eqz_l(n_l))

/*lint -esym(666,nzrand_l,oddrand_l) */

static int trivial_test (void);
static int square_test (unsigned int);
static int explaw_test1 (unsigned int);
static int explaw_test2 (unsigned int);
static int accu_test (void);
static int mexp5m_test (unsigned int);
static int mexpkm_test (unsigned int);
static int wmexpm_test (unsigned int);
static int umexpm_test (unsigned int);
static int check (CLINT, CLINT, int, int);
static int check1 (CLINT, CLINT, USHORT, USHORT, USHORT, int, int);

static void ldzrand_l (CLINT n_l, int bits);


int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  trivial_test ();
  accu_test ();
  square_test (100);
  explaw_test1 (10);
  explaw_test2 (10);
  mexp5m_test (10);
  mexpkm_test (10);
  wmexpm_test (50);
  umexpm_test (50);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int trivial_test (void)
{
  int i = 1;
  USHORT k;

  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  ldzrand_l (r10_l, 0);

  printf ("Tests with small values: 0, 1, 2 etc....\n");

  if (mexp5_l (r1_l, r2_l, r1_l, r10_l) == 0)
    {
      fprintf (stderr, "Error in mexp5_l: Reduction by zero not detected!\n");
      exit (1);
    }
  if (mexpk_l (r1_l, r2_l, r1_l, r10_l) == 0)
    {
      fprintf (stderr, "Error in mexpk_l: Reduction by zero not detected!\n");
      exit (1);
    }
  if (umexp_l (r1_l, usrand64_l (), r1_l, r10_l) == 0)
    {
      fprintf (stderr, "Error in umexp_l: Reduction by zero not detected!\n");
      exit (1);
    }
  if (wmexp_l (usrand64_l (), r2_l, r1_l, r10_l) == 0)
    {
      fprintf (stderr, "Error in wmexp_l: Reduction by zero not detected!\n");
      exit (1);
    }
  if (mexp2_l (r1_l, usrand64_l (), r1_l, r10_l) == 0)
    {
      fprintf (stderr, "Error in mexp2_l: Reduction by zero not detected!\n");
      exit (1);
    }

  /* Special case: 0 with leading zeros */
  ldzrand_l (r1_l, 0);
  rand_l (r2_l, CLINTRNDLN);
  nzrand_l (r10_l, CLINTRNDLN);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, usrand64_l (), r3_l, r10_l);
  check (r3_l, nul_l, 3, __LINE__);
  wmexp_l (0, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexp2_l (r1_l, usrand64_l (), r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case leading zeros */
  ldzrand_l (r1_l, 1);
  ldzrand_l (r2_l, 1);
  nzrand_l (r10_l, CLINTRNDLN);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  umexp_l (r1_l, 1, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  wmexp_l (1, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  mexp2_l (r1_l, 0, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 0*/
  nzrand_l (r10_l, CLINTRNDLN);
  mexp5_l (nul_l, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  mexpk_l (nul_l, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  umexp_l (nul_l, 0, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  wmexp_l (0, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  mexp2_l (nul_l, 0, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 2*/
  setzero_l (r1_l);
  nzrand_l (r10_l, CLINTRNDLN);
  mexp5_l (r1_l, two_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexpk_l (r1_l, two_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 2, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  wmexp_l (0, two_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexp2_l (r1_l, 1, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 0 */
  rand_l (r1_l, CLINTRNDLN);
  nzrand_l (r10_l, CLINTRNDLN);
  setzero_l (r2_l);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  umexp_l (r1_l, 0, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  wmexp_l (usrand64_l (), r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  mexp2_l (r1_l, 0, r3_l, r10_l);
  mod_l (r1_l, r10_l, r1_l);
  check (r3_l, r1_l, i++, __LINE__);

  /* Special case Base >= Modul, Exponent = 1 */
  do
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN);
    }
  while (LT_L (r1_l, r10_l));   /* Base >= Modulus ==> Reduction required */
  setone_l (r2_l);
  mod_l (r1_l, r10_l, r4_l);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);
  umexp_l (r1_l, 1, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);
  mexp2_l (r1_l, 0, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);
  do
    {
      k = usrand64_l ();
    }
  while (k < 3);
  u2clint_l (r10_l, (USHORT)(k - 1)); /* Base > Modulus ==> Reduction */
  wmexp_l (k, r2_l, r3_l, r10_l);
  u2clint_l (r4_l, k);
  mod_l (r4_l, r10_l, r4_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Exponent = 0, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETZERO_L (r2_l);
  u2clint_l (r10_l, 1);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 0, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 2, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  wmexp_l (2, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 1, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETONE_L (r2_l);
  u2clint_l (r10_l, 1);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 1, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 2, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  wmexp_l (2, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexp2_l (r1_l, 0, r3_l, r10_l); /* Exponent is 2^0 = 1 */
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case random exponent, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  u2clint_l (r10_l, 1);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 1, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 2, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  wmexp_l (2, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexp2_l (r1_l, 0, r3_l, r10_l); /* Exponent is 2^0 = 1 */
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 1, Modulus = 3 */
  setone_l (r2_l);
  u2clint_l (r10_l, 3);
  mexp5_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  mexpk_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  umexp_l (two_l, 1, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  wmexp_l (2, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  mexp2_l (two_l, 0, r3_l, r10_l); /* Exponent is 2^0 = 1 */
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3, Modulus = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r10_l, 3);
  mexp5_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  mexpk_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  umexp_l (two_l, 3, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);
  wmexp_l (2, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base = 100, Exponent = 3, Modulus = 10 */
  u2clint_l (r1_l, 100);
  u2clint_l (r2_l, 3);
  u2clint_l (r10_l, 10);
  mexp5_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  mexpk_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  umexp_l (r1_l, 3, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  wmexp_l (100, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  return 0;
}


static int square_test (unsigned int nooftests)
{
  unsigned i = 0;
  USHORT k;

  printf ("Tests with squares...\n");

  /* Tests against function msqr_l() */
  for (i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN);
      msqr_l (r0_l, r6_l, r10_l);
      mexp5_l (r0_l, two_l, r3_l, r10_l);
      check (r3_l, r6_l, i, __LINE__);
      mexpk_l (r0_l, two_l, r3_l, r10_l);
      check (r3_l, r6_l, i, __LINE__);
      umexp_l (r0_l, 2, r3_l, r10_l);
      check (r3_l, r6_l, i, __LINE__);
      mexp2_l (r0_l, 1, r3_l, r10_l);
      check (r3_l, r6_l, i, __LINE__);

      k = usrand64_l ();
      u2clint_l (r6_l, k);
      msqr_l (r6_l, r6_l, r10_l);
      wmexp_l (k, two_l, r3_l, r10_l);
      check (r3_l, r6_l, i, __LINE__);
    }
  return 0;
}


static int explaw_test1 (unsigned int nooftests)
{
  unsigned i = 0;
  USHORT k, l, n;

  printf ("Test 1 exponentiation law (this takes some time...)\n");

  /* 1. Test exponentiation law: a^(b+c) = a^b * a^c */
  rand_l (r0_l, CLINTMAXBIT);
  rand_l (r1_l, CLINTMAXBIT - 1);
  rand_l (r2_l, CLINTMAXBIT - 1);
  nzrand_l (r10_l, CLINTMAXBIT);
  add_l (r1_l, r2_l, r3_l);
  mexp5_l (r0_l, r1_l, r4_l, r10_l); /* a^b        */
  mexpk_l (r0_l, r2_l, r5_l, r10_l); /* a^c        */
  mmul_l (r4_l, r5_l, r5_l, r10_l);  /* a^b * a^c  */
  mexp5_l (r0_l, r3_l, r6_l, r10_l); /* a^(b+c)    */
  check (r5_l, r6_l, i, __LINE__);


  for (i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN);
      add_l (r1_l, r2_l, r3_l);
      mexp5_l (r0_l, r1_l, r4_l, r10_l); /* a^b        */
      mexpk_l (r0_l, r2_l, r5_l, r10_l); /* a^c        */
      mmul_l (r4_l, r5_l, r5_l, r10_l);  /* a^b * a^c  */
      mexp5_l (r0_l, r3_l, r6_l, r10_l); /* a^(b+c)    */
      check (r5_l, r6_l, i, __LINE__);

      do
        {
          n = 1 + (usrand64_l () % (CLINTMAXBIT - 1));  /* n > 0 */
          l = usrand64_l () % n;
          k = n - l; /* n = k + l < CLINTMAXBIT */
        }
      while (k == 0);
      umexp_l (r0_l, k, r4_l, r10_l);   /* a^b        */
      umexp_l (r0_l, l, r5_l, r10_l);   /* a^c        */
      mmul_l (r4_l, r5_l, r5_l, r10_l); /* a^b * a^c  */
      umexp_l (r0_l,n , r6_l, r10_l);   /* a^(b+c)    */
      check1 (r5_l, r6_l, n, k, l, i, __LINE__);

      wmexp_l (n, r1_l, r4_l, r10_l);   /* a^b        */
      wmexp_l (n, r2_l, r5_l, r10_l);   /* a^c        */
      mmul_l (r4_l, r5_l, r5_l, r10_l); /* a^b * a^c  */
      wmexp_l (n, r3_l, r6_l, r10_l);   /* a^(b+c)    */
      check1 (r5_l, r6_l, n, k, l, i, __LINE__);

      /* Test of mexp2_l(): a^(2^k + 2^l) = (a^2^k)*a^(2^j)      */
      mexp2_l (r0_l, k, r4_l, r10_l);    /* a^(2^k)              */
      mexp2_l (r0_l, l, r5_l, r10_l);    /* a^(2^k)              */
      mmul_l (r4_l, r5_l, r5_l, r10_l);  /* a^b * a^c            */
      setmax_l (r11_l);
      umexp_l (two_l, k, r7_l, r11_l);   /* r7_l = 2^k           */
      umexp_l (two_l, l, r8_l, r11_l);   /* r7_l = 2^l           */
      add_l (r7_l, r8_l, r7_l);          /* r7_l = 2^k + 2^l     */
      mexpk_l (r0_l, r7_l, r6_l, r10_l); /* r6_l = a^(2^k + 2^l) */
      check1 (r5_l, r6_l, n, k, l, i, __LINE__);
    }
  return 0;
}


static int explaw_test2 (unsigned int nooftests)
{
  unsigned i = 0;
  USHORT k, l, n;

  printf ("Test 2 exponentiation law (takes some time too...)\n");

  /* 2. Test exponentiation law: a^(b*c) = (a^b)^c */
  rand_l (r0_l, CLINTMAXBIT);
  rand_l (r1_l, CLINTMAXBIT / 2);
  rand_l (r2_l, CLINTMAXBIT / 2);
  nzrand_l (r10_l, CLINTMAXBIT);
  mul_l (r1_l, r2_l, r3_l);          /* r3_l = r1_l * r2_l */
  mexp5_l (r0_l, r1_l, r4_l, r10_l); /* a^b                */
  mexpk_l (r4_l, r2_l, r5_l, r10_l); /* (a^b)^c            */
  mexp5_l (r0_l, r3_l, r6_l, r10_l); /* a^(b*c)            */
  check (r5_l, r6_l, i, __LINE__);

  for (i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, usrand64_l () % CLINTMAXBIT / 2);
      rand_l (r2_l, usrand64_l () % CLINTMAXBIT / 2);
      nzrand_l (r10_l, CLINTRNDLN);
      mul_l (r1_l, r2_l, r3_l);          /* r3_l = r1_l * r2_l */
      mexp5_l (r0_l, r1_l, r4_l, r10_l); /* a^b                */
      mexpk_l (r4_l, r2_l, r5_l, r10_l); /* (a^b)^c            */
      mexp5_l (r0_l, r3_l, r6_l, r10_l); /* a^(b*c)            */
      check (r5_l, r6_l, i, __LINE__);

      mexpk_l (r0_l, r1_l, r4_l, r10_l); /* a^b        */
      mexp5_l (r4_l, r2_l, r5_l, r10_l); /* (a^b)^c    */
      mexpk_l (r0_l, r3_l, r6_l, r10_l); /* a^(b*c)    */
      check (r5_l, r6_l, i, __LINE__);

      k = usrand64_l () % 256;
      l = usrand64_l () % 256;                           /* k * l < 2^16 */
      umexp_l (r0_l, k, r4_l, r10_l);                    /* a^b          */
      umexp_l (r4_l, l, r5_l, r10_l);                    /* (a^b)^c      */
      umexp_l (r0_l, ( USHORT ) ( k * l ), r6_l, r10_l); /* a^(b*c)      */
      check (r5_l, r6_l, i, __LINE__);

      n = k + l;
      wmexp_l (n, r1_l, r4_l, r10_l);    /* a^b        */
      mexpk_l (r4_l, r2_l, r5_l, r10_l); /* (a^b)^c    */
      wmexp_l (n, r3_l, r6_l, r10_l);    /* a^(b*c)    */
      check (r5_l, r6_l, i, __LINE__);

      k = usrand64_l () % CLINTMAXBIT / 2;
      l = usrand64_l () % CLINTMAXBIT / 2;                /* n = k + l < CLINTMAXBIT */
      mexp2_l (r0_l, k, r4_l, r10_l);                     /* a^(2^k)         */
      mexp2_l (r4_l, l, r5_l, r10_l);                     /* (a^(2^k))^(2^l) */
      setmax_l (r11_l);
      umexp_l (two_l, ( USHORT ) ( k + l ), r7_l, r11_l); /* r7_l = 2^(k+l)         */
      mexpk_l (r0_l, r7_l, r6_l, r10_l);                  /* r6_l = a^((2^k)*(2^l)) */
      check (r5_l, r6_l, i, __LINE__);                    /*      = a^(2^(k+l))     */

      /* Test of mexp2_l(): e = (2^k)*u ==> a^e = (a^(2^k))^u  */
      k = twofact_l (r1_l, r2_l);        /* e = (2^k)*u        */
      mexp2_l (r0_l, k, r3_l, r10_l);    /* a^(2^k)            */
      mexpk_l (r3_l, r2_l, r3_l, r10_l); /* (a^(2^k))^u        */
      mexp5_l (r0_l, r1_l, r0_l, r10_l); /* a^e                */
      check (r3_l, r0_l, i, __LINE__);

    }
  return 0;
}


static int accu_test (void)
{
  USHORT k;
  printf ("Test accumulator mode...\n");

  rand_l (r0_l, 768);
  nzrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  cpy_l (r2_l, r0_l);
  mexp5_l (r0_l, r0_l, r0_l, r10_l);
  mexp5_l (r1_l, r1_l, r10_l, r10_l);
  check (r0_l, r10_l, 1, __LINE__);

  rand_l (r0_l, 768);
  nzrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  cpy_l (r2_l, r0_l);
  mexpk_l (r0_l, r0_l, r0_l, r10_l);
  mexpk_l (r1_l, r1_l, r10_l, r10_l);
  check (r0_l, r10_l, 2, __LINE__);

  rand_l (r0_l, 768);
  nzrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  k = usrand64_l ();
  umexp_l (r0_l, k, r0_l, r10_l);
  umexp_l (r1_l, k, r10_l, r10_l);
  check (r0_l, r10_l, 3, __LINE__);

  rand_l (r0_l, 768);
  nzrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  k = usrand64_l ();
  wmexp_l (k, r0_l, r0_l, r10_l);
  wmexp_l (k, r1_l, r10_l, r10_l);
  check (r0_l, r10_l, 4, __LINE__);

  rand_l (r0_l, 768);
  nzrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  k = usrand64_l () % 768;
  mexp2_l (r0_l, k, r0_l, r10_l);
  mexp2_l (r1_l, k, r10_l, r10_l);
  check (r0_l, r10_l, 5, __LINE__);

  return 0;
}


static int mexp5m_test (unsigned int nooftests)
{
  unsigned int i = 1;
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  ldzrand_l (r10_l, 0);

  printf ("Tests of mexp5m_l() with small values: 0, 1, 2 etc....\n");

  if (mexp5m_l (r1_l, r2_l, r1_l, r10_l) != E_CLINT_DBZ)
    {
      fprintf (stderr, "Error in mexp5m_l: Reduction by zero not detected!\n");
      exit (1);
    }

  nzrand_l (r10_l, CLINTRNDLN);
  r10_l[1] &= (BASEMINONE - 1);

  if (mexp5m_l (r1_l, r2_l, r1_l, r10_l) != E_CLINT_MOD)
    {
      fprintf (stderr, "Error in mexp5m_l: Reduction by even modulus not detected!\n");
      exit (1);
    }


  /* Special case 0 with leading zeros */
  ldzrand_l (r1_l, 0);
  rand_l (r2_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTRNDLN);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case leading zeros */
  ldzrand_l (r1_l, 1);
  ldzrand_l (r2_l, 1);
  oddrand_l (r10_l, CLINTRNDLN);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 0*/
  oddrand_l (r10_l, CLINTRNDLN);
  mexp5m_l (nul_l, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);
  wmexpm_l (0, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 2*/
  setzero_l (r1_l);
  oddrand_l (r10_l, CLINTRNDLN);
  mexp5m_l (r1_l, two_l, r3_l, r10_l);
  check (r3_l, nul_l, 3, __LINE__);
  wmexpm_l (0, two_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 0 */
  rand_l (r1_l, CLINTRNDLN);
  setzero_l (r2_l);
  oddrand_l (r10_l, CLINTRNDLN);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base > Modulus, Exponent = 1 */
  do
    {
      rand_l (r1_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
    }
  while (LT_L (r1_l, r10_l)); /* Base >= Modulus ==> Reduction required */
  setone_l (r2_l);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  mexpk_l (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r4_l, 8);
  oddrand_l (r10_l, CLINTRNDLN);
  mod_l (r4_l, r10_l, r4_l);
  mexp5m_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Exponent = 0, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETZERO_L (r2_l);
  u2clint_l (r10_l, 1);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 1, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETONE_L (r2_l);
  u2clint_l (r10_l, 1);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case random exponent,  Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  u2clint_l (r10_l, 1);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 1, Modulus = 3 */
  u2clint_l (r10_l, 3);
  mexp5m_l (two_l, one_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3, Modulus = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r10_l, 3);
  mexp5m_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  printf ("Test accumulator mode of mexp5m_l()...\n");

  rand_l (r0_l, 768);
  oddrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  mexp5m_l (r0_l, r0_l, r0_l, r10_l);
  mexp5m_l (r1_l, r1_l, r10_l, r10_l);
  check (r0_l, r10_l, i++, __LINE__);

  printf ("Tests of mexp5m_l() against mexpk_l()...\n");

  /* Modulus with maximum number of digits */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTMAXDIGIT);
  mexp5m_l (r1_l, r2_l, r3_l, r10_l);
  mexpk_l (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  for (; i <= 500; i++)
    {
      rand_l (r1_l, CLINTRNDLNS);
      rand_l (r2_l, CLINTRNDLNS);
      oddrand_l (r10_l, CLINTRNDLNS);
      mexp5m_l (r1_l, r2_l, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  for (i = 1; i < nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
      mexp5m_l (r1_l, r2_l, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  return 0;
}


static int mexpkm_test (unsigned int nooftests)
{
  unsigned int i = 1;
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  ldzrand_l (r10_l, 0);

  printf ("Tests of mexpkm_l() with small values: 0, 1, 2 etc....\n");

  if (mexpkm_l (r1_l, r2_l, r1_l, r10_l) != E_CLINT_DBZ)
    {
      fprintf (stderr, "Error in mexpkm_l: Reduction by zero not detected!\n");
      exit (1);
    }

  nzrand_l (r10_l, CLINTRNDLN);
  r10_l[1] &= (BASEMINONE - 1);

  if (mexpkm_l (r1_l, r2_l, r1_l, r10_l) != E_CLINT_MOD)
    {
      fprintf (stderr, "Error in mexpkm_l: Reduction by even modulus not detected!\n");
      exit (1);
    }


  /* Special case 0 with leading zeros */
  ldzrand_l (r1_l, 0);
  rand_l (r2_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTRNDLN);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case leading zeros */
  ldzrand_l (r1_l, 1);
  ldzrand_l (r2_l, 1);
  oddrand_l (r10_l, CLINTRNDLN);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 0*/
  oddrand_l (r10_l, CLINTRNDLN);
  mexpkm_l (nul_l, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 2*/
  setzero_l (r1_l);
  oddrand_l (r10_l, CLINTRNDLN);
  mexpkm_l (r1_l, two_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 0 */
  rand_l (r1_l, CLINTRNDLN);
  setzero_l (r2_l);
  oddrand_l (r10_l, CLINTRNDLN);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base > Modulus, Exponent = 1 */
  do
    {
      rand_l (r1_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
    }
  while (LT_L (r1_l, r10_l)); /* Base >= Modulus ==> Reduction required */
  setone_l (r2_l);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  mexpk_l  (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r4_l, 8);
  oddrand_l (r10_l, CLINTRNDLN);
  mod_l (r4_l, r10_l, r4_l);
  mexpkm_l (two_l, r2_l, r3_l, r10_l);
  mod_l (r4_l, r10_l, r4_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Exponent = 0, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETZERO_L (r2_l);
  u2clint_l (r10_l, 1);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 1, Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETONE_L (r2_l);
  u2clint_l (r10_l, 1);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case random exponent,  Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  u2clint_l (r10_l, 1);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 1, Modulus = 3 */
  setone_l (r2_l);
  u2clint_l (r10_l, 3);
  mexpkm_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3, Modulus = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r10_l, 3);
  mexpkm_l (two_l, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);


  printf ("Akkumulator-Test of mexpkm_l()...\n");

  rand_l (r0_l, 768);
  oddrand_l (r10_l, 1024);
  cpy_l (r1_l, r0_l);
  mexpkm_l (r0_l, r0_l, r0_l, r10_l);
  mexpkm_l (r1_l, r1_l, r10_l, r10_l);
  check (r0_l, r10_l, i++, __LINE__);


  printf ("Tests of mexpkm_l() against mexpk_l()...\n");

  /* Modulus with maximum number of digits */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTMAXDIGIT);
  mexpkm_l (r1_l, r2_l, r3_l, r10_l);
  mexpk_l (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  for (; i < 500; i++)
    {
      rand_l (r1_l, CLINTRNDLNS);
      rand_l (r2_l, CLINTRNDLNS);
      oddrand_l (r10_l, CLINTRNDLNS);
      mexpkm_l (r1_l, r2_l, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  for (i = 1; i < nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
      mexpkm_l (r1_l, r2_l, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  return 0;
}


static int wmexpm_test (unsigned int nooftests)
{
  unsigned int i = 1;
  USHORT k;
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  ldzrand_l (r10_l, 0);

  printf ("Tests of wmexpm_l() with small values: 0, 1, 2 etc....\n");

  if (wmexpm_l (1, r2_l, r1_l, r10_l) != E_CLINT_DBZ)
    {
      fprintf (stderr, "Error in wmexpm_l: Reduction by zero not detected!\n");
      exit (1);
    }

  nzrand_l (r10_l, CLINTRNDLN);
  r10_l[1] &= (BASEMINONE - 1);

  if (wmexpm_l (1, r2_l, r1_l, r10_l) != E_CLINT_MOD)
    {
      fprintf (stderr, "Error in wmexpm_l: Reduction by even modulus not detected!\n");
      exit (1);
    }


  /* Special case 0 with leading zeros */
  rand_l (r2_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTRNDLN);
  wmexpm_l (0, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case leading zeros */
  ldzrand_l (r2_l, 1);
  oddrand_l (r10_l, CLINTRNDLN);
  wmexpm_l (1, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 0*/
  oddrand_l (r10_l, CLINTRNDLN);
  wmexpm_l (0, nul_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 2*/
  oddrand_l (r10_l, CLINTRNDLN);
  wmexpm_l (0, two_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 0 */
  setzero_l (r2_l);
  oddrand_l (r10_l, CLINTRNDLN);
  wmexpm_l (1, r2_l, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base > Modulus, Exponent = 1 */
  setone_l (r2_l);
  do
    {
      k = usrand64_l ();
    }
  while (k < 3 || (k&1));             /* k is even */
  u2clint_l (r10_l, (USHORT)(k - 1)); /* Base > Modulus ==> Reduction */
  wmexpm_l (k, r2_l, r3_l, r10_l);
  u2clint_l (r4_l, k);
  mod_l (r4_l, r10_l, r4_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r4_l, 8);
  oddrand_l (r10_l, CLINTRNDLN);
  mod_l (r4_l, r10_l, r4_l);
  wmexpm_l (2, r2_l, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Modulus = 1 */
  rand_l (r2_l, CLINTRNDLN);
  u2clint_l (r10_l, 1);
  wmexpm_l (2, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 1, Modulus = 3 */
  setone_l (r2_l);
  u2clint_l (r10_l, 3);
  wmexpm_l (2, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3, Modulus = 3 */
  u2clint_l (r2_l, 3);
  u2clint_l (r10_l, 3);
  wmexpm_l (2, r2_l, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);


  printf ("Test accumulator mode of wmexpm_l()...\n");

  k = usrand64_l ();
  oddrand_l (r10_l, 1024);
  rand_l (r0_l, 768);
  cpy_l (r1_l, r0_l);
  wmexpm_l (k, r0_l, r0_l, r10_l);
  wmexpm_l (k, r1_l, r10_l, r10_l);
  check (r0_l, r10_l, i++, __LINE__);


  printf ("Tests of wmexpm_l() against mexpk_l()...\n");

  /* Modulus with maximum number of digits */
  k = usrand64_l ();
  u2clint_l (r1_l, k);
  rand_l (r2_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTMAXDIGIT);
  wmexpm_l (k, r2_l, r3_l, r10_l);
  mexpk_l (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  for (; i <= 500; i++)
    {
      k = usrand64_l ();
      u2clint_l (r1_l, k);
      rand_l (r2_l, CLINTRNDLNS);
      oddrand_l (r10_l, CLINTRNDLNS);
      wmexpm_l (k, r2_l, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  for (i = 0; i < nooftests; i++)
    {
      k = usrand64_l ();
      u2clint_l (r1_l, k);
      rand_l (r2_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
      wmexpm_l (k, r2_l, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  return 0;
}


static int umexpm_test (unsigned int nooftests)
{
  unsigned int i = 1;
  USHORT e;
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  ldzrand_l (r10_l, 0);

  printf ("Tests of umexpm_l() with small values: 0, 1, 2 etc....\n");

  if (umexpm_l (r1_l, 1, r3_l, r10_l) != E_CLINT_DBZ)
    {
      fprintf (stderr, "Error in umexpm_l: Reduction by zero not detected!\n");
      exit (1);
    }

  nzrand_l (r10_l, CLINTRNDLN);
  r10_l[1] &= (BASEMINONE - 1);

  if (umexpm_l (r1_l, 1, r3_l, r10_l) != E_CLINT_MOD)
    {
      fprintf (stderr, "Error in umexpm_l: Reduction by even modulus not detected!\n");
      exit (1);
    }


  /* Special case 0 with leading zeros */
  do
    {
      e = usrand64_l ();
    }
  while (0 == e);

  oddrand_l (r10_l, CLINTRNDLN);
  umexpm_l (nul_l, e, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case leading zeros */
  ldzrand_l (r1_l, 1);
  e = usrand64_l ();
  u2clint_l (r2_l, e);
  oddrand_l (r10_l, CLINTRNDLN);
  umexpm_l (r1_l, e, r3_l, r10_l);
  mexpk_l (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 0*/
  oddrand_l (r10_l, CLINTRNDLN);
  umexpm_l (nul_l, 0, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 0, Exponent = 2*/
  oddrand_l (r10_l, CLINTRNDLN);
  umexpm_l (nul_l, 2, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Exponent = 0 */
  rand_l (r1_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTRNDLN);
  umexpm_l (r1_l, 0, r3_l, r10_l);
  if (EQONE_L(r10_l))
    check (r3_l, nul_l, i++, __LINE__);
  else
    check (r3_l, one_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3 */
  u2clint_l (r4_l, 8);
  oddrand_l (r10_l, CLINTRNDLN);
  mod_l (r4_l, r10_l, r4_l);
  umexpm_l (two_l, 3, r3_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Special case Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  u2clint_l (r10_l, 1);
  umexpm_l (r1_l, 2, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 1, Modulus = 3 */
  u2clint_l (r10_l, 3);
  umexpm_l (two_l, 1, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base = 2, Exponent = 3, Modulus = 3 */
  u2clint_l (r10_l, 3);
  umexpm_l (two_l, 3, r3_l, r10_l);
  check (r3_l, two_l, i++, __LINE__);

  /* Special case Base >= Modulus, Exponent = 1 */
  setone_l (r2_l);
  do
    {
      rand_l (r1_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
    }
  while (LT_L (r1_l, r10_l));
  umexpm_l (r1_l, 1, r3_l, r10_l);
  mod_l (r1_l, r10_l, r4_l);
  check (r3_l, r4_l, i++, __LINE__);

  /* Test accumulator mode */
  printf ("Test accumulator mode of umexpm_l()...\n");

  e = usrand64_l ();
  oddrand_l (r10_l, 1024);
  rand_l (r0_l, 768);
  cpy_l (r1_l, r0_l);
  umexpm_l (r0_l, e, r0_l, r10_l);
  umexpm_l (r1_l, e, r10_l, r10_l);
  check (r0_l, r10_l, i++, __LINE__);


  printf ("Tests of umexpm_l() against mexpk_l()...\n");

  /* Modulus with maximum number of digits */
  e = usrand64_l ();
  u2clint_l (r2_l, e);
  rand_l (r1_l, CLINTRNDLN);
  oddrand_l (r10_l, CLINTMAXDIGIT);
  umexpm_l (r1_l, e, r3_l, r10_l);
  mexpk_l (r1_l, r2_l, r4_l, r10_l);
  check (r3_l, r4_l, i++, __LINE__);

  for (; i <= 500; i++)
    {
      e = usrand64_l ();
      u2clint_l (r2_l, e);
      rand_l (r1_l, CLINTRNDLNS);
      oddrand_l (r10_l, CLINTRNDLNS);
      umexpm_l (r1_l, e, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  for (i = 0; i < nooftests; i++)
    {
      e = usrand64_l ();
      u2clint_l (r2_l, e);
      rand_l (r1_l, CLINTRNDLN);
      oddrand_l (r10_l, CLINTRNDLN);
      umexpm_l (r1_l, e, r3_l, r10_l);
      mexpk_l (r1_l, r2_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  return 0;
}


static int check (CLINT a_l, CLINT b_l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Error in vcheck_l(a_l) in line %d\n", line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }

  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Error in ?mexp?_l() in test %d/line %d\n", test, line);
      disperr_l ("a_l = ", r1_l);
      disperr_l ("e_l = ", r2_l);
      disperr_l ("m_l = ", r10_l);
      disperr_l ("p1_l = ", a_l);
      disperr_l ("p2_l = ", b_l);
      exit (-1);
    }
  return 0;
}


static int check1 (CLINT a_l, CLINT b_l, USHORT n, USHORT k, USHORT l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Error in vcheck_l(a_l) in line %d\n", line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }

  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Error in ?mexp?_l() in test %d/line %d\n", test, line);
      disperr_l ("a_l = ", r0_l);
      disperr_l ("m_l = ", r10_l);
      disperr_l ("p1_l = ", a_l);
      disperr_l ("p2_l = ", b_l);
      printf ("n = %d, k = %d, l = %d\n", n, k, l);
      exit (-1);
    }
  return 0;
}


static void ldzrand_l (CLINT n_l, int bits)
{
  unsigned int i;
  rand_l (n_l, bits);
  if (digits_l (n_l) == CLINTMAXDIGIT)
    {
      decdigits_l (n_l);
    }

  for (i = digits_l (n_l) + 1; i <= CLINTMAXDIGIT; i++)
    {
      n_l[i] = 0;
    }
  i = digits_l (n_l);
  do
    {
      setdigits_l (n_l, digits_l (n_l) + ( ulrand64_l () % ( CLINTMAXDIGIT - i + 1 )));
    }
  while (vcheck_l (n_l) != E_VCHECK_LDZ);
}

