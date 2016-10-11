/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testiroo.c       Revision: 19.12.2000                               */
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
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

static int test_squares (unsigned int nooftests);
static int test_nonsquares (void);
static int test_iroot (unsigned int nooftests);

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  test_squares (1000);
  test_nonsquares ();
  test_iroot (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int
test_squares (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test function issqr_l()...\n");
  printf ("Trivial tests with small constants\n");

  u2clint_l (r0_l, 0);
  u2clint_l (r1_l, 0);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square not detected near line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square root wrong near line %d\n", __LINE__);
      disperr_l ("Root = ", r0_l); /*lint !e666*/
      disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 1);
  u2clint_l (r1_l, 1);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square not detected near line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square root wrong near line %d\n", __LINE__);
      disperr_l ("Root = ", r0_l); /*lint !e666*/
      disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 2);
  u2clint_l (r1_l, 4);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square not detected near line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square root wrong near line %d\n", __LINE__);
      disperr_l ("Root = ", r0_l); /*lint !e666*/
      disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 4);
  u2clint_l (r1_l, 16);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square not detected near line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square root falsch near line %d\n", __LINE__);
      disperr_l ("Root = ", r0_l); /*lint !e666*/
      disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 256);
  ul2clint_l (r1_l, 65536);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square not detected near line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Error in issqr_l():\n");
      fprintf (stderr, "Square root falsch near line %d\n", __LINE__);
      disperr_l ("Root = ", r0_l); /*lint !e666*/
      disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  printf ("Tests with squares...\n");

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (r0_l, CLINTRNDLN/2);
      sqr_l (r0_l, r1_l);

      if (!issqr_l (r1_l, r2_l))
        {
          fprintf (stderr, "Error in issqr_l():\n");
          fprintf (stderr, "Square not detected in test %d near line %d\n", i, __LINE__);
          exit (-1);
        }

      if (!equ_l (r0_l, r2_l))
        {
          fprintf (stderr, "Error in issqr_l():\n");
          fprintf (stderr, "Square root falsch in test %d near line %d\n", i, __LINE__);
          disperr_l ("Root = ", r0_l); /*lint !e666*/
          disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
          exit (-1);
        }
    }

  return 0;
}

static int
test_nonsquares (void)
{
  int i;
  unsigned short p;

  printf ("Tests of issqr_l() with non-squares...\n");

  i = 1;
  p = 2;
  do
    {
      ul2clint_l (r0_l, p);

      if (issqr_l (r0_l, r1_l))
        {
          fprintf (stderr, "Error in issqr_l():\n");
          fprintf (stderr, "Square detected in test %d near line %d\n", i, __LINE__);
          exit (-1);
        }

      p += smallprimes[i];
    }
  while (++i <= NOOFSMALLPRIMES);

  for (i = 1; i <= 5; i++)
    {
      nzrand_l (r0_l, 256);

      if (ISEVEN_L (r0_l))
        {
          inc_l (r0_l);
        }

      while (!isprime_l (r0_l))
        {
          inc_l (r0_l);
          inc_l (r0_l);
        }

      if (issqr_l (r1_l, r2_l))
        {
          fprintf (stderr, "Error in issqr_l():\n");
          fprintf (stderr, "Square detected in test %d near line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int
test_iroot (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test function iroot_l()...\n");
  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (r0_l, CLINTRNDLN/2);
      sqr_l (r0_l, r1_l);

      iroot_l (r1_l, r2_l);

      if (!equ_l (r0_l, r2_l))
        {
          fprintf (stderr, "Error in issqr_l():\n");
          fprintf (stderr, "Square root wrong in line %d\n", __LINE__);
          disperr_l ("Root = ", r0_l); /*lint !e666*/
          disperr_l ("Calculated value = ", r2_l); /*lint !e666*/
          exit (-1);
        }
    }

  return 0;
}
