/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testsub.c        Revision: 19.12.2000                               */
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
#define CLINTRNDLNDIGIT (1 + ulrand64_l()% CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))

static int neu_test (unsigned int nooftests);
static int subadd_test (unsigned int nooftests);
static int ass_test (unsigned int nooftests);
static int usub_test (unsigned int nooftests);
static int dec_test (unsigned int nooftests);
static int underflow_test (void);
static int check (CLINT a_l, CLINT b_l, int test, int line);
static void ldzrand_l (CLINT n_l, int bits);

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());

  initrand64_lt ();
  create_reg_l ();

  neu_test (1000);
  subadd_test (1000);
  ass_test (1000);
  usub_test (1000);
  underflow_test ();
  dec_test (1000);

  printf ("All tests in %s passed.\n", __FILE__);
  free_reg_l ();

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (unsigned int nooftests)
{
  unsigned long i = 1;

  printf ("Test with 0 as neutral element...\n");

  /* Test with 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  sub_l (r1_l, r2_l, r3_l);
  sub_l (r2_l, r1_l, r4_l);
  check (r3_l, nul_l, i++, __LINE__);
  check (r4_l, nul_l, i++, __LINE__);

  /* max_l - 0 = max_l */
  setmax_l (r1_l);
  setzero_l (r2_l);
  sub_l (r1_l, r2_l, r3_l);
  check (r3_l, r1_l, i++, __LINE__);

  /* max_l - (max_l - 1) = 1 */
  setmax_l (r1_l);
  sub_l (r1_l, one_l, r2_l);
  sub_l (r1_l, r2_l, r3_l);
  check (r3_l, one_l, i++, __LINE__);

  setzero_l (r2_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      sub_l (r1_l, r2_l, r3_l);
      check (r3_l, r1_l, i, __LINE__);
    }

  setzero_l (r2_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      sub_l (r1_l, r2_l, r3_l);
      check (r3_l, r1_l, i, __LINE__);
    }



  /* Test with 0 and leading zeros */
  ldzrand_l (r2_l, 0);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      sub_l (r1_l, r2_l, r3_l);
      check (r3_l, r1_l, i, __LINE__);
    }

  /* Test accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      sub_l (r1_l, r1_l, r1_l);
      check (r1_l, nul_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r10_l, r1_l);
      sub_l (r1_l, r2_l, r1_l);
      sub_l (r10_l, r2_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r10_l, r2_l);
      sub_l (r1_l, r2_l, r2_l);
      sub_l (r1_l, r10_l, r3_l);
      check (r2_l, r3_l, i, __LINE__);
    }
  return 0;
}


static int subadd_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test with addition and subtraction as inverse operations...\n");

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      if (ge_l (r1_l, r2_l))
        {
          sub_l (r1_l, r2_l, r3_l);
          add_l (r3_l, r2_l, r3_l);
          check (r3_l, r1_l, i, __LINE__);
        }
    }
  return 0;
}



static int ass_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Test for associative law...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      rand_l (r3_l, CLINTRNDLN);
      add_l (r2_l, r3_l, r4_l); /* r4_l = r2_l + r3_l */
      add_l (r4_l, r1_l, r5_l); /* r5_l = r1_l + r2_l + r3_l */

      sub_l (r5_l, r3_l, r6_l); /* r6_l = r5_l - r3_l */

      sub_l (r6_l, r2_l, r7_l); /* r7_l = (r5_l - r3_l) - r2_l */

      sub_l (r5_l, r4_l, r8_l); /* r8_l = r5_l - (r3_l + r2_l) */

      check (r7_l, r8_l, i, __LINE__);
    }
  return 0;
}


static int usub_test (unsigned int nooftests)
{
  unsigned int i = 1, k;
  USHORT n;
  CLINT n_l;

  printf ("Test usub_l()...\n");

  /* Test with 0 */
  rand_l (r1_l, CLINTRNDLN);
  n = 0;
  usub_l (r1_l, n, r3_l);
  check (r3_l, r1_l, i++, __LINE__);

  /* Test accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      n = usrand64_l ();
      u2clint_l (n_l, n);

      k = CLINTRNDLN;
      rand_l (r1_l, MAX (( ld_l (n_l) + 1 ), k)); /*lint !e666*/
      cpy_l (r2_l, r1_l);
      usub_l (r1_l, n, r1_l);
      sub_l (r2_l, n_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);
    }

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      n = usrand64_l ();
      u2clint_l (n_l, n);
      usub_l (r1_l, n, r3_l);
      sub_l (r1_l, n_l, r4_l);
      check (r3_l, r4_l, i, __LINE__);
    }
  return 0;
}


static int dec_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;
  CLINT k_l;

  printf ("Test dec_l()...\n");

  /* Test with 1 */
  setone_l (r1_l);
  dec_l (r1_l);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test underrun word boundary */
  setdigits_l (r1_l, 2);
  r1_l[1] = 0;
  r1_l[2] = 1;
  cpy_l (r2_l, r1_l);
  dec_l (r2_l);
  if (ld_l (r2_l) != 16 && digits_l (r2_l) != 1)
    {
      fprintf (stderr, "Error in dec_l(): ld_l(r1_l) != 16 in line %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);   /*lint !e666*/
      disperr_l ("--r1_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  sub_l (r1_l, one_l, r1_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      k = usrand64_l () % 1001;
      j = CLINTRNDLN;
      rand_l (r1_l, MAX (k + 1, j));
      cpy_l (r2_l, r1_l);
      cpy_l (r3_l, r1_l);
      u2clint_l (k_l, ( USHORT ) k);
      for (j = 1; j <= k; j++)
        {
          dec_l (r1_l);
          sub_l (r2_l, one_l, r2_l);
        }
      sub_l (r3_l, k_l, r3_l);
      check (r1_l, r2_l, i, __LINE__);
      check (r2_l, r3_l, i, __LINE__);
    }
  return 0;
}


static int underflow_test (void)
{
  printf ("Test underflow in sub_l()...\n");

  setmax_l (r0_l);

  if (E_CLINT_UFL != sub_l (nul_l, one_l, r2_l)) /* 0 - 1 = max_l */
    {
      fprintf (stderr, "Error in sub_l: Underflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in reduction mod 2^CLINTMAXBIT in line %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }

  if (E_CLINT_UFL != sub_l (one_l, two_l, r2_l)) /* 1 - 2 = max_l */
    {
      fprintf (stderr, "Error in sub_l: Underflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in reduction mod 2^CLINTMAXBIT in line %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  if (E_CLINT_UFL != sub_l (nul_l, r0_l, r2_l)) /* 0 - max_l = 1 */
    {
      fprintf (stderr, "Error in sub_l: Underflow not detected in line %d\n", __LINE__);
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
      fprintf (stderr, "Error in vcheck_l(a_l) in test %d/line %d\n", test, line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }

  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Error in sub_l() in test %d/line %d\n", test, line);
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

