/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testsqr.c        Revision: 19.12.2000                               */
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

static int neu_test (void);
static int sqrmul_test (unsigned int nooftests);
static int pot_test (unsigned int nooftests);
static int binom_test (unsigned int nooftests);
static int dist_test (unsigned int nooftests);
static int overflow_test (void);
static int check (CLINT a_l, CLINT b_l, int test, int line);
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
  overflow_test ();

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (void)
{
  /* Test with 0 */
  setzero_l (r1_l);
  sqr_l (r1_l, r2_l);
  check (r2_l, nul_l, 1, __LINE__);

  /* Test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  sqr_l (r1_l, r2_l);
  check (r2_l, nul_l, 2, __LINE__);


  printf ("Neutral element...\n");

  /* Test with 1 as neutral element */
  setone_l (r1_l);
  sqr_l (r1_l, r2_l);
  check (r2_l, one_l, 3, __LINE__);

  return 0;
}


static int sqrmul_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test vs. multiplication...\n");

  /* Test with max_l */
  setmax_l (r1_l);
  sqr_l (r1_l, r2_l);
  mul_l (r1_l, r1_l, r3_l);
  check (r3_l, r2_l, 1, __LINE__);

  /* Test against multiplication in accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r10_l, r1_l);
      sqr_l (r1_l, r1_l);
      mul_l (r10_l, r10_l, r3_l);
      check (r3_l, r1_l, i, __LINE__);
    }
  return 0;
}



static int pot_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Testing exponentiation laws...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      sqr_l (r1_l, r3_l);
      sqr_l (r2_l, r4_l);
      mul_l (r3_l, r4_l, r4_l); /* r4_l = r1_l^2 * r2_l^2 */

      mul_l (r1_l, r2_l, r5_l);
      sqr_l (r5_l, r5_l); /* r5_l = (r1_l * r2_l)^2 */

      check (r5_l, r4_l, i, __LINE__);
    }
  return 0;
}



static int binom_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Testing binomial laws...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      add_l (r1_l, r2_l, r3_l);
      sqr_l (r3_l, r3_l); /* r3_l = (r1_l + r2_l)^2 */

      sqr_l (r1_l, r4_l);
      sqr_l (r2_l, r5_l);
      mul_l (r1_l, r2_l, r6_l);
      mul_l (r6_l, two_l, r6_l);
      add_l (r4_l, r5_l, r4_l);
      add_l (r4_l, r6_l, r4_l); /* r4_l = r1_l^2 + 2*r1_l*r2_l + r2_l^2 */

      check (r3_l, r4_l, i, __LINE__);
    }
  return 0;
}


static int dist_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Testing distribution laws...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, CLINTRNDLN);
      add_l (r0_l, r1_l, r2_l);
      mul_l (r0_l, r2_l, r4_l);

      sqr_l (r0_l, r5_l);
      mul_l (r0_l, r1_l, r6_l);
      add_l (r5_l, r6_l, r7_l);
      check (r4_l, r7_l, i, __LINE__);
    }
  return 0;
}


static int overflow_test (void)
{
  printf ("Testing overflow in sqr_l()...\n");

  setmax_l (r0_l);

  if (E_CLINT_OFL != sqr_l (r0_l, r2_l)) /* max_l * max_l = 1 */
    {
      fprintf (stderr, "Error in sqr_l: Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (one_l, r2_l))
    {
      fprintf (stderr, "Error in reduction mod 2^CLINTMAXBIT in line %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
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
      fprintf (stderr, "Error in sqr_l() in line %d\n", line);
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


