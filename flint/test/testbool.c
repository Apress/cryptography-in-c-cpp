/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testbbs.c        Revision: 19.12.2000                               */
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

static int and_test (unsigned int nooftests);
static int or_test (unsigned int nooftests);
static int xor_test (unsigned int nooftests);

static int check (CLINT a_l, CLINT b_l, int test, int line);

/*lint -esym(666,swap_l)*/

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  and_test (1000);
  or_test (1000);
  xor_test (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int xor_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Test xor_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  xor_l (nul_l, r1_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  xor_l (r1_l, nul_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  SETONE_L (r0_l);
  rand_l (r1_l, CLINTRNDLN);
  xor_l (r0_l, r1_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] ^= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }

  rand_l (r1_l, CLINTRNDLN);
  xor_l (r1_l, r0_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] ^= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }


  setmax_l (r0_l);
  xor_l (r0_l, r0_l, r0_l);
  check (r0_l, nul_l, i, __LINE__);


  /* General tests */

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      xor_l (r0_l, r0_l, r0_l);
      check (r0_l, nul_l, i, __LINE__);
      xor_l (r1_l, r0_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r0_l);
      cpy_l (r3_l, r1_l);
      swap_l (r0_l, r1_l);
      check (r0_l, r3_l, i, __LINE__);
      check (r1_l, r2_l, i, __LINE__);
    }
  return 0;
}


static int or_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Test or_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  or_l (nul_l, r1_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  or_l (r1_l, nul_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  or_l (nul_l, nul_l, r2_l);
  check (r2_l, nul_l, i, __LINE__);

  or_l (one_l, nul_l, r2_l);
  check (r2_l, one_l, i, __LINE__);

  SETONE_L (r0_l);
  rand_l (r1_l, CLINTRNDLN);
  or_l (r0_l, r1_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] |= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }

  rand_l (r1_l, CLINTRNDLN);
  or_l (r1_l, r0_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] |= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }

  setmax_l (r0_l);
  setmax_l (r1_l);
  or_l (r0_l, r0_l, r0_l);
  check (r0_l, r1_l, i, __LINE__);


  /* General tests */

  for (i = 1; i <= nooftests; i++)
    {
      setmax_l (r0_l);
      rand_l (r1_l, CLINTRNDLN);
      or_l (r0_l, r1_l, r1_l);
      check (r1_l, r0_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      or_l (r0_l, r0_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      setzero_l (r2_l);
      or_l (r0_l, r2_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  return 0;
}


static int and_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Test and_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  and_l (nul_l, r1_l, r2_l);
  check (r2_l, nul_l, i, __LINE__);

  and_l (r1_l, nul_l, r2_l);
  check (r2_l, nul_l, i, __LINE__);

  SETONE_L (r0_l);
  rand_l (r1_l, CLINTRNDLN);
  and_l (r0_l, r1_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] &= 1;
      SETDIGITS_L (r1_l, 1);
    }
  check (r2_l, r1_l, i, __LINE__);

  rand_l (r1_l, CLINTRNDLN);
  and_l (r1_l, r0_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] &= 1;
      SETDIGITS_L (r1_l, 1);
    }
  check (r2_l, r1_l, i, __LINE__);


  setmax_l (r0_l);
  setmax_l (r1_l);
  and_l (r0_l, r0_l, r0_l);
  check (r0_l, r1_l, i, __LINE__);


  /* General tests */

  for (i = 1; i <= nooftests; i++)
    {
      setmax_l (r0_l);
      rand_l (r1_l, CLINTRNDLN);
      and_l (r0_l, r1_l, r0_l);
      check (r1_l, r0_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      and_l (r0_l, r0_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      setzero_l (r2_l);
      and_l (r0_l, r2_l, r0_l);
      check (r0_l, nul_l, i, __LINE__);
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
      fprintf (stderr, "Error in test in line %d\n", line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}



