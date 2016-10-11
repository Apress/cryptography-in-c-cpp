/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testkar.c        Revision: 26.01.2002                               */
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
/*  26.01.2002:                                                               */
/*    Added test with product and factors of Bonn-University-factorization    */
/*    from January 2002                                                       */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "flint.h"
#include "kmul.h"

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

#define oddrand_l(n_l,bits) do { rand_l((n_l),(bits)); n_l[1] |= 1; } while (eqz_l(n_l))
#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))

static int rsaXXX_test (void);
static int simple_test (unsigned int nooftests);
static int overflow_test (void);
static int testkmul (int);
static int testksqr (int);
static int check (CLINT a_l, CLINT b_l, int test, int line);
static void ldzrand_l (CLINT n_l, int bits);


int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  rsaXXX_test ();
  simple_test (1000);
  overflow_test ();
  testkmul (1000);
  testksqr (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int rsaXXX_test (void)
{
  CLINT rsaXXX_l;

  printf ("Test factors of RSA-130 ...\n");

  str2clint_l (rsaXXX_l, "18070820886874048059516561644059055662781025167694013"
                         "49170127021450056662540244048387341127590812303371781"
                         "887966563182013214880557", 10);
  str2clint_l (r2_l, "39685999459597454290161126162883786067576449112810064"
                     "832555157243", 10);
  str2clint_l (r3_l, "45534498646735972188403686897274408864356301263205069"
                     "600999044599", 10);

  kmul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);

  printf ("Test factors of RSA-140 ...\n");

  str2clint_l (rsaXXX_l, "21290246318258757547497882016271517497806703963277216"
                         "27823338321538194998405649591136657385302191831678310"
                         "7387995317230889569230873441936471", 10);
  str2clint_l (r2_l, "33987174230284385545301236276138758356339864959695974"
                     "23490929302771479", 10);
  str2clint_l (r3_l, "62642001874012850961516549482644422193020371786235090"
                     "19111660653946049", 10);

  kmul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);

  printf ("Test factors of RSA-155 ...\n");

  str2clint_l (rsaXXX_l, "10941738641570527421809707322040357612003732945449205"
                         "99091384213147634998428893478471799725789126733249762"
                         "5752899781833797076537244027146743531593354333897", 10);
  str2clint_l (r2_l, "102639592829741105772054196573991675900716567808038066803"
                     "341933521790711307779", 10);
  str2clint_l (r3_l, "106603488380168454820927220360012878679207958575989291522"
                     "270608237193062808643", 10);

  kmul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);

  printf ("Test factors of Singh-Challenge ...\n");

  str2clint_l (rsaXXX_l, "10742788291266565907178411279942116612663921794753294"
                         "58887781721035546415098012187903383292623528109075067"
                         "2083504941996433143425558334401855808989426892463", 10);
  str2clint_l (r2_l, "128442051653810314916622590289775531989649843239158643682"
                     "16177647043137765477", 10);
  str2clint_l (r3_l, "836391832187606937820650856449710761904520026199724985596"
                     "729108812301394489219", 10);

  kmul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);

  printf ("Test with the Bonn University factorization 1/2002 ...\n");

  str2clint_l (rsaXXX_l, "39505874583265144526419767800614481996020776460304936"
                         "45413937605157935562652945068360972784246821953509354"
                         "4305870490251995655335710209799226484977949442955603", 10);
  str2clint_l (r2_l, "338849583746672139436839320467218152281583036860499304808"
                     "4925840555281177", 10);

  str2clint_l (r3_l, "116588234066712599031483765583832708181310122581463926004"
                     "39520994131344334162924536139", 10);

  kmul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);

  return 0;
}


static int simple_test (unsigned int nooftests)
{
  unsigned long i = 1;

  printf ("Special cases in kmul()...\n");

  /* Test with 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  kmul_l (r1_l, r2_l, r3_l);
  check (r3_l, nul_l, i++, __LINE__);

  setzero_l (r1_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r2_l, CLINTRNDLN);
      kmul_l (r1_l, r2_l, r3_l);
      kmul_l (r2_l, r1_l, r4_l);
      check (r3_l, nul_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r2_l, CLINTRNDLN);
      kmul_l (r1_l, r2_l, r3_l);
      kmul_l (r2_l, r1_l, r4_l);
      check (r3_l, nul_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test with 1 as neutral element */
  setone_l (r1_l);
  setone_l (r2_l);
  kmul_l (r1_l, r2_l, r3_l);
  check (r3_l, one_l, i++, __LINE__);

  setone_l (r1_l);
  setmax_l (r2_l);
  kmul_l (r1_l, r2_l, r3_l);
  kmul_l (r2_l, r1_l, r4_l);
  check (r3_l, r2_l, i++, __LINE__);
  check (r4_l, r2_l, i++, __LINE__);


  setone_l (r1_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r2_l, CLINTRNDLN);
      kmul_l (r1_l, r2_l, r3_l);
      kmul_l (r2_l, r1_l, r4_l);
      check (r3_l, r2_l, i, __LINE__);
      check (r4_l, r2_l, i, __LINE__);
    }


  /* Test with 1 and leading zeros in the second argument*/
  ldzrand_l (r1_l, 1);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r2_l, CLINTRNDLN);
      kmul_l (r1_l, r2_l, r3_l);
      kmul_l (r2_l, r1_l, r4_l);
      check (r3_l, r2_l, i, __LINE__);
      check (r4_l, r2_l, i, __LINE__);
    }

  /* Test accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      kmul_l (r1_l, r1_l, r1_l);
      kmul_l (r2_l, r2_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r10_l, r1_l);
      kmul_l (r1_l, r2_l, r1_l);
      kmul_l (r2_l, r10_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r10_l, r2_l);
      kmul_l (r1_l, r2_l, r2_l);
      kmul_l (r1_l, r10_l, r3_l);
      check (r2_l, r3_l, i, __LINE__);
    }


  /* Test ksqr_l() */

  printf ("Special cases in ksqr()...\n");

  /* Test with 0 */
  setzero_l (r1_l);
  ksqr_l (r1_l, r3_l);
  check (r3_l, nul_l, i++, __LINE__);


  /* Test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  ksqr_l (r1_l, r3_l);
  check (r3_l, nul_l, i, __LINE__);


  /* Test with 1 as neutral element */
  setone_l (r1_l);
  ksqr_l (r1_l, r3_l);
  check (r3_l, one_l, i++, __LINE__);

  setmax_l (r1_l);
  ksqr_l (r1_l, r2_l);
  sqr_l (r1_l, r3_l);
  check (r2_l, r3_l, i++, __LINE__);


  /* Test accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      ksqr_l (r1_l, r1_l);
      sqr_l (r2_l,  r3_l);
      check (r1_l, r3_l, i, __LINE__);
    }

  return 0;
}


static int overflow_test (void)
{
  printf ("Test Overflow in kmul_l() and ksqr_l() ...\n");

  setmax_l (r0_l);
  sub_l (r0_l, one_l, r1_l);

  if (E_CLINT_OFL != kmul_l (r0_l, two_l, r2_l)) /* max_l * 2 = max_l - 1 */
    {
      fprintf (stderr, "Error in kmul_l: Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r1_l, r2_l))
    {
      fprintf (stderr, "Error in reduction mod 2^CLINTMAXBIT in line %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  if (E_CLINT_OFL != kmul_l (r0_l, r0_l, r2_l)) /* max_l * max_l = 1 */
    {
      fprintf (stderr, "Error in kmul_l: Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (one_l, r2_l))
    {
      fprintf (stderr, "Error in reduction mod 2^CLINTMAXBIT in line %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }

  if (E_CLINT_OFL != ksqr_l (r0_l, r2_l)) /* max_l * max_l = 1 */
    {
      fprintf (stderr, "Error in ksqr_l: Overflow not detected in line %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (one_l, r2_l))
    {
      fprintf (stderr, "Error in reduction mod 2^CLINTMAXBIT in ksqr_l() in line %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }

  return 0;
}


/* General tests against mul_l() */
static int testkmul (int nooftests)
{
  int i;

  printf ("Test kmul_l() against mul_l()...\n");

  for (i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, usrand64_l () % CLINTMAXBIT/2);
      rand_l (r2_l, DIGITS_L (r0_l)*BITPERDGT);

      Assert (DIGITS_L (r0_l) == DIGITS_L (r2_l));

      kmul_l (r0_l, r2_l, r10_l);
      mul_l (r0_l, r2_l, r11_l);
      check (r10_l, r11_l, i, __LINE__);
    }

  return (0);
}


/* General tests against sqr_l() */
static int testksqr (int nooftests)
{
  int i;

  printf ("Test ksqr_l() against sqr_l()...\n");

  for (i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, usrand64_l () % CLINTMAXBIT/2);
      ksqr_l (r0_l, r10_l);
      sqr_l (r0_l, r11_l);
      check (r10_l, r11_l, i, __LINE__);
    }

  return (0);
}


/* Auxiliary functions */
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
      fprintf (stderr, "Error in test %d near line %d\n", test, line);
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

