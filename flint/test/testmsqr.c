/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testmsqr.c       Revision: 23.01.2002                               */
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
/*  23.01.2002:                                                               */
/*    Replaced line                                                           */
/*      check (r2_l, one_l, i++, __LINE__);                                   */
/*    by conditional test                                                     */
/*      if (EQONE_L(r10_l))                                                   */
/*        check (r2_l, nul_l, i++, __LINE__);                                 */
/*      else                                                                  */
/*        check (r2_l, one_l, i++, __LINE__);                                 */
/*    to care for the rare situations where square modulo 1 == 0              */
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
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))
#define oddrand_l(n_l,bits) do { rand_l((n_l),(bits)); n_l[1] |= 1; } while (eqz_l(n_l))

static int neu_test (void);
static int sqrmul_test (unsigned int nooftests);
static int pot_test (unsigned int nooftests);
static int dist_test (unsigned int nooftests);
static int binom_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);
static int sqrmon_test (int nooftests);
static void ldzrand_l (CLINT n_l, int bits);


int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  neu_test ();
  sqrmul_test (1000);
  pot_test (1000);
  binom_test (1000);
  dist_test (1000);
  sqrmon_test (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (void)
{
  unsigned long i = 1;

  /* Test with 0 */

  setzero_l (r1_l);
  if (0 == msqr_l (r1_l, r1_l, r1_l))
    {
      fprintf (stderr, "Error in msqr_l(): Reduction by 0 not detected in line %d\n", __LINE__);
      exit (-1);
    }

  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  msqr_l (r1_l, r2_l, r10_l);
  check (r2_l, nul_l, i++, __LINE__);


  /* Test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  msqr_l (r1_l, r2_l, r10_l);
  check (r2_l, nul_l, i++, __LINE__);


  printf ("Neutral element...\n");

  /* Test with 1 as neutral element */
  setone_l (r1_l);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  msqr_l (r1_l, r2_l, r10_l);
  if (EQONE_L(r10_l))
    check (r2_l, nul_l, i++, __LINE__);
  else
    check (r2_l, one_l, i++, __LINE__);

  setmax_l (r1_l);
  msqr_l (r1_l, r2_l, r10_l);
  mmul_l (r1_l, r1_l, r3_l, r10_l);
  check (r3_l, r2_l, i++, __LINE__);

  /* Test with modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  SETONE_L (r10_l);
  msqr_l (r1_l, r3_l, r10_l);
  check (r3_l, nul_l, i, __LINE__);

  return 0;
}


static int sqrmul_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test against multiplication...\n");

  /* Test vs. multiplication in accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r11_l, r10_l);
      msqr_l (r1_l, r10_l, r10_l);
      mmul_l (r1_l, r1_l, r3_l, r11_l);
      check (r3_l, r10_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r11_l, r1_l);
      msqr_l (r1_l, r1_l, r10_l);
      mmul_l (r11_l, r11_l, r3_l, r10_l);
      check (r3_l, r1_l, i, __LINE__);
    }
  return 0;
}



static int pot_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Test exponentiation rules...\n");

  /* Test with maximal number of digits */
  rand_l (r1_l, CLINTMAXBIT);
  rand_l (r2_l, CLINTMAXBIT);
  nzrand_l (r10_l, CLINTMAXBIT);    /*lint !e666*/
  msqr_l (r1_l, r3_l, r10_l);
  msqr_l (r2_l, r4_l, r10_l);
  mmul_l (r3_l, r4_l, r4_l, r10_l); /* r4_l = r1_l^2 * r2_l^2 */

  mmul_l (r1_l, r2_l, r5_l, r10_l);
  msqr_l (r5_l, r5_l, r10_l);       /* r5_l = (r1_l * r2_l)^2 */

  check (r5_l, r4_l, i, __LINE__);


  /* Tests with random number of digits */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN);     /*lint !e666*/
      msqr_l (r1_l, r3_l, r10_l);
      msqr_l (r2_l, r4_l, r10_l);
      mmul_l (r3_l, r4_l, r4_l, r10_l); /* r4_l = r1_l^2 * r2_l^2 */

      mmul_l (r1_l, r2_l, r5_l, r10_l);
      msqr_l (r5_l, r5_l, r10_l);       /* r5_l = (r1_l * r2_l)^2 */

      check (r5_l, r4_l, i, __LINE__);
    }
  return 0;
}



static int binom_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Test binomial rules...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      madd_l (r1_l, r2_l, r3_l, r10_l);
      msqr_l (r3_l, r3_l, r10_l);   /* r3_l = (r1_l + r2_l)^2 */

      msqr_l (r1_l, r4_l, r10_l);
      msqr_l (r2_l, r5_l, r10_l);
      mmul_l (r1_l, r2_l, r6_l, r10_l);
      mmul_l (r6_l, two_l, r6_l, r10_l);
      madd_l (r4_l, r5_l, r4_l, r10_l);
      madd_l (r4_l, r6_l, r4_l, r10_l); /* r4_l = r1_l^2 + 2*r1_l*r2_l + r2_l^2 */

      check (r3_l, r4_l, i, __LINE__);
    }
  return 0;
}


static int dist_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test distributive law...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      madd_l (r0_l, r1_l, r2_l, r10_l);
      mmul_l (r0_l, r2_l, r4_l, r10_l);

      msqr_l (r0_l, r5_l, r10_l);
      mmul_l (r0_l, r1_l, r6_l, r10_l);
      madd_l (r5_l, r6_l, r7_l, r10_l);
      check (r4_l, r7_l, i, __LINE__);
    }
  return 0;
}


/* Test of Montgomery-Squaring */
static int sqrmon_test (int nooftests)
{
  USHORT logB_r, nprime, i = 0;
  clint *rptr_l;

  printf ("Test sqrmon_l()...\n");

  if (( rptr_l = ( clint * ) malloc (2 + CLINTMAXBYTE)) == NULL)
    {
      fprintf (stderr, "Error in malloc in line %d\n", __LINE__);
      exit (-1);
    }

  rand_l (r0_l, CLINTMAXBIT);
  do
    {
      oddrand_l (r10_l, CLINTMAXBIT);         /* n odd */
    }
  while (GE_L (r0_l, r10_l));                 /* n > a */

  logB_r = DIGITS_L (r10_l);
  SETZERO_L (rptr_l);
  setbit_l (rptr_l, logB_r << LDBITPERDGT);

  if (DIGITS_L (rptr_l) > CLINTMAXDIGIT)      /* rptr_l < r10_l <= max_l */
    {
      mod_l (rptr_l, r10_l, rptr_l);
    }

  Assert (DIGITS_L (rptr_l) <= CLINTMAXDIGIT + 1);

  mmul_l (r0_l, rptr_l, r1_l, r10_l); /* a' = a*r mod n */
  nprime = invmon_l (r10_l);          /* n^2 > a'*a'    */

  sqrmon_l (r1_l, r10_l, nprime, logB_r, r3_l);        /* p' <- a'x a' */
  mulmon_l (r3_l, one_l, r10_l, nprime, logB_r, r3_l); /* p  <- p'x 1  */

  msqr_l (r0_l, r4_l, r10_l);         /* a * a mod n */

  check (r3_l, r4_l, i, __LINE__);


  for ( i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, usrand64_l () % CLINTMAXBIT);
      do
        {
          oddrand_l (r10_l, usrand64_l () % CLINTMAXBIT);   /* n odd */
        }
      while (GE_L (r0_l, r10_l));                           /* n > a */

      logB_r = DIGITS_L (r10_l);
      SETZERO_L (rptr_l);
      setbit_l (rptr_l, logB_r << LDBITPERDGT);

      if (DIGITS_L (rptr_l) > CLINTMAXDIGIT)    /* rptr_l < r10_l <= MaxB */
        {
          mod_l (rptr_l, r10_l, rptr_l);
        }

      Assert (DIGITS_L (rptr_l) <= CLINTMAXDIGIT + 1);

      mmul_l (r0_l, rptr_l, r1_l, r10_l); /* a' = a*r mod n */
      nprime = invmon_l (r10_l);          /* n^2 > a'*a'    */

      sqrmon_l (r1_l, r10_l, nprime, logB_r, r3_l);        /* p' <- a'x a' */
      mulmon_l (r3_l, one_l, r10_l, nprime, logB_r, r3_l); /* p  <- p'x 1  */

      msqr_l (r0_l, r4_l, r10_l);         /* a * a mod n */

      check (r3_l, r4_l, i, __LINE__);
    }

  free (rptr_l);
  return (0);
}


/* Auxiliary functions */

static int check (CLINT a_l, CLINT b_l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Error in vcheck_l(a_l) in test %d/line %d\n", test, line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }

  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Error in msqr_l() in test %d/line %d\n", test, line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
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


