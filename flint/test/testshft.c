/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testshft.c       Revision: 19.12.2000                               */
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

static int shl_test (unsigned int nooftests);
static int shr_test (unsigned int nooftests);
static int shift_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);


int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  shl_test (1000);
  shr_test (1000);
  shift_test (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int shl_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;

  printf ("Test shl_l()...\n");

  /* Test with 0 */
  setzero_l (r1_l);
  shl_l (r1_l);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test with max_l */
  setmax_l (r0_l);
  setmax_l (r1_l);
  if (E_CLINT_OFL != shl_l (r0_l))
    {
      fprintf (stderr, "Error in shl_l(): Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }
  umul_l (r1_l, 2, r1_l);
  check (r0_l, r1_l, i++, __LINE__);

  /* Test overriding word boundary */
  u2clint_l (r1_l, BASEMINONE);
  cpy_l (r2_l, r1_l);
  shl_l (r2_l);
  if (ld_l (r2_l) != 17)
    {
      fprintf (stderr, "Error in shl_l(): ld_l(r1_l) != 17 in line %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);        /*lint !e666*/
      disperr_l ("shl_l(r1_l) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  mul_l (r1_l, two_l, r1_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      for (j = 1; j <= k; j++)
        {
          shl_l (r1_l);
          mul_l (r2_l, two_l, r2_l);
        }
      check (r1_l, r2_l, i, __LINE__);
    }
  return 0;
}


static int shr_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;

  printf ("Test shr_l()...\n");

  /* Test with 0 */
  setzero_l (r1_l);
  if (E_CLINT_UFL != shr_l (r1_l))
    {
      fprintf (stderr, "Error in shr_l(): Underflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  /* Test with 1 */
  setone_l (r1_l);
  shr_l (r1_l);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test cross word boundary */
  setdigits_l (r1_l, 2);
  r1_l[1] = 0;
  r1_l[2] = 1;
  cpy_l (r2_l, r1_l);
  shr_l (r2_l);
  if (ld_l (r2_l) != 16 && digits_l (r2_l) != 1)
    {
      fprintf (stderr, "Error in shr_l(): ld_l(r1_l) != 16 in line %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);        /*lint !e666*/
      disperr_l ("shr_l(r1_l) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  udiv_l (r1_l, 2, r1_l, r0_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      for (j = 1; j <= k; j++)
        {
          shr_l (r1_l);
          udiv_l (r2_l, 2, r2_l, r0_l);
        }
      check (r1_l, r2_l, i, __LINE__);
    }
  return 0;

}


static int shift_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;
  int sk;

  printf ("Test shift_l to left ...\n");

  /* Test with 0 */
  setzero_l (r1_l);
  shift_l (r1_l, 20);
  check (r1_l, nul_l, i++, __LINE__);

  rand_l (r1_l, CLINTRNDLN);
  cpy_l (r2_l, r1_l);
  shift_l (r1_l, 0);
  check (r1_l, r2_l, i++, __LINE__);

  /* Test with max_l */
  setmax_l (r0_l);
  setmax_l (r1_l);
  if (E_CLINT_OFL != shift_l (r0_l, 1))
    {
      fprintf (stderr, "Error in shift_l(): Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }
  r1_l[1] &= (BASEMINONE - 1);
  check (r0_l, r1_l, i++, __LINE__);

  if (E_CLINT_OFL != shift_l (r0_l, CLINTMAXBIT - 1))
    {
      fprintf (stderr, "Error in shift_l(): Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, nul_l, i++, __LINE__);

  /* Test cross word boundary */
  u2clint_l (r1_l, BASEMINONE);
  cpy_l (r2_l, r1_l);
  shift_l (r2_l, 1);
  if (ld_l (r2_l) != 17)
    {
      fprintf (stderr, "Error in shift_l(): ld_l(r1_l) != 17 in line %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);             /*lint !e666*/
      disperr_l ("shift_l(r1_l, 1) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  mul_l (r1_l, two_l, r1_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      shift_l (r1_l, k);
      for (j = 1; j <= k; j++)
        {
          mul_l (r2_l, two_l, r2_l);
        }
      check (r1_l, r2_l, i, __LINE__);
    }

  printf ("Test shift_l to right...\n");

  /* Test with 1 */
  setone_l (r1_l);
  shift_l (r1_l, -1);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test with max_l */
  setmax_l (r0_l);
  setmax_l (r1_l);
  shift_l (r0_l, -1);
  r1_l[CLINTMAXDIGIT] &= (BASEMINONE - BASEDIV2);
  check (r0_l, r1_l, i++, __LINE__);

  shift_l (r0_l, -((int)( CLINTMAXBIT - 1 )));
  r1_l[CLINTMAXDIGIT] &= (BASEMINONE - BASEDIV2);
  check (r0_l, nul_l, i++, __LINE__);

  /* Test cross word boundary */
  setdigits_l (r1_l, 2);
  r1_l[1] = 0;
  r1_l[2] = 1;
  cpy_l (r2_l, r1_l);
  shift_l (r2_l, -1);
  if (ld_l (r2_l) != 16 && digits_l (r2_l) != 1)
    {
      fprintf (stderr, "Error in shift_l(): ld_l(r1_l) != 16 in line %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);              /*lint !e666*/
      disperr_l ("shift_l(r1_l, -1) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  udiv_l (r1_l, 2, r1_l, r0_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      sk = ( int ) ( 0 - k );
      shift_l (r1_l, sk);
      for (j = 1; j <= k; j++)
        {
          div_l (r2_l, two_l, r2_l, r0_l);
        }
      check (r1_l, r2_l, i, __LINE__);
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
      fprintf (stderr, "Error in sh?_l() in line %d\n", line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}

