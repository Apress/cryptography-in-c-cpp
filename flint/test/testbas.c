/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testbas.c        Revision: 19.12.2000                               */
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


static void ldzrand_l (CLINT n_l, int bits);

static int vcheck_test (unsigned int nooftests);

static int ld_test (unsigned int nooftests);

static int cpy_test (unsigned int nooftests);
static int check_cpy (CLINT a_l, CLINT b_l, int test, int line);

static int equ_test (unsigned int nooftests);
static int check_equ (CLINT a_l, CLINT b_l, int eq, int i, int line);

static int mequ_test (unsigned int nooftests);
static int check_mequ (CLINT a_l, CLINT b_l, CLINT m_l, int eq, int i, int line);

static int cmp_test (unsigned int nooftests);
static int check_cmp (CLINT a_l, CLINT b_l, int eq, int i, int line);

static int str_test (unsigned int nooftests);
static int check_str (CLINT a_l, CLINT b_l, int i, int line);

static int byte_test (unsigned int nooftests);
static int check_byte (CLINT a_l, CLINT b_l, int i, int line);

static int ul2_test (unsigned int nooftests);
static int check_ul2 (CLINT a_l, clintd n, int islong, int line);

static int setmax_test (void);


#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)
#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());

  Assert (sizeof (USHORT) == 2);
  Assert (sizeof (ULONG) == 4);
  Assert (sizeof (CLINT) == CLINTMAXBYTE);
  Assert (sizeof (CLINTD) == 2 + (CLINTMAXDIGIT << 2));
  Assert (sizeof (CLINTQ) == 2 + (CLINTMAXDIGIT << 3));

  initrand64_lt ();
  create_reg_l ();

  vcheck_test (1000);
  cpy_test (1000);
  ld_test (1000);
  equ_test (1000);
  mequ_test (1000);
  cmp_test (1000);
  ul2_test (2000);
  str_test (100);
  byte_test (1000);
  setmax_test ();

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int vcheck_test (unsigned int nooftests)
{
  unsigned int i;
  clint r_l[CLINTMAXDIGIT << 1 ];

  printf ("Test vcheck_l()... \n");

  ldzrand_l (r_l, CLINTMAXBIT);
  setdigits_l (r_l, CLINTMAXDIGIT + 1);
  r_l[digits_l (r_l)] = 1;
  if (vcheck_l (r_l) != E_VCHECK_OFL)
    {
      fprintf (stderr, "Error in vcheck_l() in line %d\n", __LINE__);
      fprintf (stderr, "vcheck_l(r_l)!= E_VCHECK_OFL, ld_l(r_l) = %d\n", ld_l (r_l));
      exit (-1);
    }

  if (vcheck_l (NULL) != E_VCHECK_MEM)
    {
      fprintf (stderr, "Error in vcheck_l() in line %d\n", __LINE__);
      fprintf (stderr, " vcheck_l(NULL) != E_VCHECK_MEM\n");
      exit (-1);
    }

  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r_l, CLINTRNDLN / 2);
      if (vcheck_l (r_l) != E_VCHECK_LDZ)
        {
          fprintf (stderr, "Error in vcheck_l() in test %d/line %d\n", i, __LINE__);
          fprintf (stderr, " vcheck_l(r_l) != E_VCHECK_LDZ\n");
          disperr_l ("r_l = ", r_l);
          exit (-1);
        }
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r_l, CLINTRNDLN);
      if (vcheck_l (r_l) != 0)
        {
          fprintf (stderr, "Error in vcheck_l() in line %d\n", __LINE__);
          fprintf (stderr, " vcheck_l(r_l) != 0\n");
          disperr_l ("r_l = ", r_l);
          exit (-1);
        }
    }
  return 0;
}


static int cpy_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test cpy_l()...\n");

  /* test with 0 */
  setzero_l (r1_l);
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* test with max_l */
  setmax_l (r1_l);
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* test with 0 and leading zeros */
  ldzrand_l (r1_l, CLINTRNDLN);
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* test with a single digit and leading zeros */
  rand_l (r1_l, ulrand64_l () % ( BITPERDGT + 1 ));
  for (j = digits_l (r1_l) + 1; j <= CLINTMAXDIGIT; j++)
    {
      r1_l[j] = 0;
    }
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* Test copying an operand to itself */
  setzero_l (r1_l);
  cpy_l (r1_l, r1_l);
  check_cpy (r1_l, nul_l, i++, __LINE__);

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);

      for (j = 0; j <= digits_l (r1_l); j++)
        {
          r2_l[j] = r1_l[j];
        }

      cpy_l (r1_l, r1_l);
      check_cpy (r1_l, r2_l, i, __LINE__);
    }


  /* General Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      check_cpy (r2_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int check_cpy (CLINT dest_l, CLINT src_l, int test, int line)
{
  unsigned int i;
  if (vcheck_l (dest_l))
    {
      fprintf (stderr, "Error in vcheck_l(dest_l) in line %d\n", line);
      fprintf (stderr, "vcheck_l(dest_l) == %d\n", vcheck_l (dest_l));
      disperr_l ("dest_l = ", dest_l);
      exit (-1);
    }


  RMLDZRS_L (src_l);
  if (vcheck_l (dest_l) > 0)
    {
      fprintf (stderr, "Error cpy_l: vcheck_l(dest_l) != 0 in line %d\n", line);
      exit (-1);
    }

  if (digits_l (src_l) != digits_l (dest_l))
    {
      fprintf (stderr, "Error cpy_l: src_l and dest_l have different length in line %d\n", line);
      exit (-1);
    }
  for (i = 1; i < digits_l (src_l); i++)
    {
      if (src_l[i] != dest_l[i])
        {
          fprintf (stderr, "Error cpy_l: src_l != dest_l in Stelle %d in line %d\n", i, line);
          exit (-1);
        }
    }
  return 0;
}


static int ld_test (unsigned int nooftests)
{
  unsigned int i = 1, k, l;

  printf ("Test ld_l()...\n");

  /* test with 0 */
  setzero_l (r1_l);
  if (ld_l (r1_l) != 0)
    {
      fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
      fprintf (stderr, " ld_l(r1_l) != 0\n");
      exit (-1);
    }

  /* test with max_l */
  setmax_l (r1_l);
  if (ld_l (r1_l) != CLINTMAXBIT)
    {
      fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
      fprintf (stderr, " ld_l(r1_l) != %d\n", CLINTMAXBIT);
      exit (-1);
    }

  /* test with 0 and leading zeros */

  for (i = 1; i < nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      l = ld_l (r1_l);
      k = (( l % BITPERDGT ) > 0 ) ? 1 : 0;

      if ((( l >> LDBITPERDGT ) + k ) != digits_l (r2_l))
        {
          fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
          fprintf (stderr, " ld_l(r1_l)/%ld + 1 != digits_l(r2_l)\n", BITPERDGT);
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          disperr_l ("r2_l = ", r2_l); /*lint !e666*/
          exit (-1);
        }
      if (( l != 0 ) && ( r1_l[ ( l >> LDBITPERDGT ) + k] & ( 1 << (( l - 1 ) % BITPERDGT ))) == 0)
        {
          fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
          fprintf (stderr, " requested bit in r1_l not set\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
      if (( k > 0 ) && (( r1_l[ ( l >> LDBITPERDGT ) + k] >> ( l % BITPERDGT )) > 0 ))
        {
          fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
          fprintf (stderr, "ld_l(r1_l) < ld(r1_l) + 1\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
    }


  /* General tests */

  for (i = 1; i < nooftests; i++)
    {
      nzrand_l (r1_l, CLINTRNDLN); /*lint !e666*/
      l = ld_l (r1_l);
      k = (( l % BITPERDGT ) > 0 ) ? 1 : 0;

      if ((( l >> LDBITPERDGT ) + k ) != digits_l (r1_l))
        {
          fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
          fprintf (stderr, " ld_l(r1_l)/%ld + 1 != digits_l(r1_l)\n", BITPERDGT);
          exit (-1);
        }
      if (( r1_l[ ( l >> LDBITPERDGT ) + k] & ( 1 << (( l - 1 ) % BITPERDGT ))) == 0)
        {
          fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
          fprintf (stderr, " requested bit in r1_l not set\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
      if (( k > 0 ) && (( r1_l[ ( l >> LDBITPERDGT ) + k] >> ( l % BITPERDGT )) > 0 ))
        {
          fprintf (stderr, "Error in ld_l() in line %d\n", __LINE__);
          fprintf (stderr, "ld_l(r1_l) < ld(r1_l) + 1\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
    }
  return 0;
}


static int equ_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test equ_l()...\n");

  /* test with 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  check_equ (r1_l, r2_l, 1, i++, __LINE__);


  /* Tests with max_l */
  setmax_l (r1_l);
  setzero_l (r2_l);
  check_equ (r1_l, r2_l, -1, i++, __LINE__);

  setzero_l (r1_l);
  setmax_l (r2_l);
  check_equ (r1_l, r2_l, -1, i++, __LINE__);

  setmax_l (r1_l);
  setmax_l (r2_l);
  check_equ (r1_l, r2_l, 1, i++, __LINE__);


  /* test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  ldzrand_l (r2_l, 0);
  check_equ (r1_l, r2_l, 1, i++, __LINE__);


  /* test with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      ZCPY_L (r2_l, r1_l); /*lint !e666 */
      for (j = digits_l (r2_l) + 1; j <= CLINTMAXDIGIT; j++)
        {
          r2_l[j] = 0;
        }
      j = digits_l (r2_l);
      setdigits_l (r2_l, j + ( ulrand64_l () % ( CLINTMAXDIGIT - j + 1 )));
      check_equ (r1_l, r2_l, 1, i, __LINE__);
    }


  /* General tests: Equality */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      check_equ (r1_l, r2_l, 1, i, __LINE__);
    }


  /* General tests: Inequality */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      setbit_l (r2_l, usrand64_l() % CLINTMAXBIT);
      xor_l (r1_l, r2_l, r2_l);
      check_equ (r1_l, r2_l, -1, i, __LINE__);
    }
  return 0;
}


static int check_equ (CLINT a_l, CLINT b_l, int eq, int test, int line)
{
  switch (eq)
    {
      case 1:
        if (!equ_l (a_l, b_l))
          {
            fprintf (stderr, "Error in equ_l(): r1_l != r2_l in line %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;
      case -1:
        if (equ_l (a_l, b_l))
          {
            fprintf (stderr, "Error in equ_l(): r1_l == r2_l in line %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;
      default:
        fprintf (stderr, "Error in test condition for equ_test() in line %d\n", line);
        exit (-1);
    }
  return 0;
}


static int mequ_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test mequ_l()...\n");

  /* test with 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  if (E_CLINT_DBZ != mequ_l (one_l, one_l, r1_l))
    {
      fprintf (stderr, "Error: Reduction with 0  not detected by mod_l() in line %d\n", __LINE__);
      exit (-1);
    }

  /* Tests with max_l */
  setmax_l (r1_l);
  setzero_l (r2_l);
  do
    {
      nzrand_l (r10_l, CLINTRNDLN);  /*lint !e666*/
      mod_l (r1_l, r10_l, r3_l);
    }
  while (EQZ_L (r3_l));

  check_mequ (r1_l, r2_l, r10_l, -1, i++, __LINE__);

  setzero_l (r1_l);
  setmax_l (r2_l);
  do
    {
      nzrand_l (r10_l, CLINTRNDLN);  /*lint !e666*/
      mod_l (r2_l, r10_l, r3_l);
    }
  while (EQZ_L (r3_l));

  check_mequ (r1_l, r2_l, r10_l, -1, i++, __LINE__);

  setmax_l (r1_l);
  setmax_l (r2_l);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  check_mequ (r1_l, r2_l, r10_l, 1, i++, __LINE__);


  /* test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  ldzrand_l (r2_l, 0);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  check_mequ (r1_l, r2_l, r10_l, 1, i++, __LINE__);


  /* test with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r2_l, r1_l);
      for (j = digits_l (r2_l) + 1; j <= CLINTMAXDIGIT; j++)
        {
          r2_l[j] = 0;
        }
      j = digits_l (r2_l);
      setdigits_l (r2_l, j + ( ulrand64_l () % ( CLINTMAXDIGIT - j + 1 )));
      check_mequ (r1_l, r2_l, r10_l, 1, i, __LINE__);
    }


  /* Test Modulus = 1 */
  nzrand_l (r1_l, CLINTRNDLN); /*lint !e666*/
  nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
  check_mequ (r1_l, r2_l, one_l, 1, i, __LINE__);


  /* General tests: Equality */
  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (r1_l, CLINTRNDLN); /*lint !e666*/
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      add_l (r1_l, r2_l, r3_l);
      check_mequ (r3_l, r2_l, r1_l, 1, i, __LINE__);
      check_mequ (r3_l, r1_l, r2_l, 1, i, __LINE__);
    }


  /* General tests: Inequality */
  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          rand_l (r1_l, CLINTRNDLN);
          rand_l (r2_l, CLINTRNDLN);
          nzrand_l (r3_l, CLINTRNDLN); /*lint !e666*/
          mod_l (r1_l, r3_l, r10_l);
          mod_l (r2_l, r3_l, r11_l);
        }
      while (equ_l (r10_l, r11_l));
      check_mequ (r1_l, r2_l, r3_l, -1, i, __LINE__);
    }
  return 0;
}


static int check_mequ (CLINT a_l, CLINT b_l, CLINT m_l, int eq, int test, int line)
{
  switch (eq)
    {
      case 1:
        if (!mequ_l (a_l, b_l, m_l))
          {
            fprintf (stderr, "Error in mequ_l(): r1_l == r2_l in line %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;

      case -1:
        if (mequ_l (a_l, b_l, m_l))
          {
            fprintf (stderr, "Error in mequ_l(): r1_l != r2_l in line %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;
      default:
        fprintf (stderr, "Error in test condition for mequ_test() in line %d\n", line);
        exit (-1);
    }
  return 0;
}


static int cmp_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test cmp_l()...\n");

  /* test with 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  check_cmp (r1_l, r2_l, 1, i++, __LINE__);


  /* Tests with max_l */
  setmax_l (r1_l);
  setmax_l (r2_l);
  check_cmp (r1_l, r2_l, 1, i++, __LINE__);

  setmax_l (r1_l);
  setzero_l (r2_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);

  setzero_l (r1_l);
  setmax_l (r2_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);


  /* test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  ldzrand_l (r2_l, 0);
  check_cmp (r1_l, r2_l, 1, i++, __LINE__);


  /* Test for Equality with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      ZCPY_L (r2_l, r1_l); /*lint !e666 */
      for (j = digits_l (r2_l) + 1; j <= CLINTMAXDIGIT; j++)
        {
          r2_l[j] = 0;
        }
      j = digits_l (r2_l);
      setdigits_l (r2_l, j + ( ulrand64_l () % ( CLINTMAXDIGIT - j + 1 )));

      check_cmp (r1_l, r2_l, 1, i, __LINE__);
    }


  /* Test for Inequality with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          ldzrand_l (r1_l, CLINTRNDLN);
          ldzrand_l (r2_l, CLINTRNDLN);
        }
      while (equ_l (r1_l, r2_l));
      check_cmp (r1_l, r2_l, -1, i, __LINE__);
    }

  setzero_l (r1_l);
  setone_l (r2_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);

  setzero_l (r2_l);
  setone_l (r1_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);


  /* General tests: Equality */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      check_cmp (r1_l, r2_l, 1, i, __LINE__);
    }


  /* General tests: Inequality */
  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          rand_l (r1_l, CLINTRNDLN);
          rand_l (r2_l, CLINTRNDLN);
        }
      while (equ_l (r1_l, r2_l));
      check_cmp (r1_l, r2_l, -1, i, __LINE__);
    }
  return 0;
}


static int check_cmp (CLINT a_l, CLINT b_l, int eq, int test, int line)
{
  unsigned int i;
  int v = 0;
  switch (eq)
    {
      case 1:
        if (cmp_l (a_l, b_l))
          {
            fprintf (stderr, "Error in cmp_l(): r1_l != r2_l in line %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;

      case -1:
        if (!cmp_l (a_l, b_l))
          {
            fprintf (stderr, "Error in cmp_l(): r1_l == r2_l in line %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        else
          {
            ZCPY_L (r3_l, a_l); /*lint !e666*/
            ZCPY_L (r4_l, b_l); /*lint !e666*/
            if (( digits_l (r3_l) < digits_l (r4_l)) && ( 1 == cmp_l (a_l, b_l)))
              {
                fprintf (stderr, "Error in cmp_l(): r1_l > r2_l in line %d\n", line);
                disperr_l ("a_l = ", a_l);
                disperr_l ("b_l = ", b_l);
                exit (-1);
              }
            else
              {
                if (( digits_l (r3_l) > digits_l (r4_l)) && ( -1 == cmp_l (a_l, b_l)))
                  {
                    fprintf (stderr, "Error in cmp_l(): r1_l < r2_l in line %d\n", line);
                    disperr_l ("a_l = ", a_l);
                    disperr_l ("b_l = ", b_l);
                    exit (-1);
                  }
                else
                  {
                    if (digits_l (r3_l) == digits_l (r4_l))
                      {
                        i = digits_l (r3_l);
                        while (a_l[i] == b_l[i] && i > 0)
                          {
                            --i;
                          }
                        if (a_l[i] > b_l[i])
                          {
                            v = 1;
                          }
                        else
                          {
                            if (a_l[i] < b_l[i])
                              {
                                v = -1;
                              }
                          }
                        if (v != cmp_l (a_l, b_l)) /*lint !e644*/
                          {
                            fprintf (stderr, "Error in cmp_l(): cmp_l(r1_l, r2_l) != %d in line %d\n", v, line);
                            disperr_l ("a_l = ", a_l);
                            disperr_l ("b_l = ", b_l);
                            exit (-1);
                          }
                      }
                  }
              }
          }
        break;

      default:
        fprintf (stderr, "Error in test condition in cmp_test() in line %d\n", line);
        exit (-1);
    }
  return 0;
}


static int ul2_test (unsigned int nooftests)
{
  unsigned long i;
  clint n1;
  clintd n2;

  printf ("Test u2clint_l(), ul2clint_l()...\n");

  for (i = 1; i <= nooftests; i++)
    {
      n1 = usrand64_l ();
      u2clint_l (r1_l, n1);
      check_ul2 (r1_l, (clintd)n1, 0, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      n2 = ulrand64_l ();
      ul2clint_l (r1_l, n2);
      check_ul2 (r1_l, n2, 1, __LINE__);
    }
  return 0;
}


static int check_ul2 (CLINT n_l, clintd n, int islong, int line)
{
  if (vcheck_l (n_l))
    {
      fprintf (stderr, "Error in vcheck_l(n_l) in line %d\n", line);
      fprintf (stderr, "vcheck_l(n_l) == %d\n", vcheck_l (n_l));
      disperr_l ("n_l = ", n_l);
      exit (-1);
    }

  switch (islong)
    {
      case 0:
        if (( digits_l (n_l) > 0 ) && ( digits_l (n_l) != 1 || n_l[1] != n ))
          {
            fprintf (stderr, "Error in u2clint_l() in line %d\nn = %lu\n", line, n);
            disperr_l ("n_l = ", n_l);
            exit (-1);
          }
        break;

      case 1:
        if ((( digits_l (n_l) == 1 ) && ( n_l[1] != ( n & BASEMINONE ))) ||
             (( digits_l (n_l) == 2 ) && (( n_l[1] != ( n & BASEMINONE )) || ( n_l[2] != ( n >> BITPERDGT ) ))))
          {
            fprintf (stderr, "Error in ul2clint_l() in line %d\nn = %lu\n", line, n);
            disperr_l ("n_l = ", n_l);
            exit (-1);
          }
        break;

      default:
        break;
    }
  return 0;
}


static int str_test (unsigned int nooftests)
{
  unsigned int i = 1;
  char *ofl;

  printf ("Test str2clint_l(), xclint2str_l()...\n");

  /* test with integer > max_l */
  if ((ofl = (char*) malloc (sizeof (char) * (CLINTMAXDIGIT * 4 + 2))) == NULL)
    {
      fprintf (stderr, "Error in malloc");
      exit (-1);
    }

/* Generate string "1000...0" with CLINTMAXDIGIT*BITPERDGT/4 zeros */
  ofl[0] = '1';
  ofl[CLINTMAXDIGIT * BITPERDGT/4 + 1] = '\0';
  for (i = 1; i <= CLINTMAXDIGIT * BITPERDGT/4; i++)
    {
      ofl[i] = '0';
    }

  if (E_CLINT_OFL != str2clint_l (r2_l, ofl, 16))
    {
      fprintf (stderr, "Error in str2clint_l near line %d: Overflow not detected.\n", __LINE__);
      exit (-1);
    }

  free (ofl);


  /* test with 0 */
  setzero_l (r1_l);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* test with max_l */
  setmax_l (r1_l);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* test with 1 */
  setone_l (r1_l);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* test with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }


  /* test with capitals */

  str2clint_l (r1_l, "1234567890ABCDEF", 16);
  str2clint_l (r2_l, "1234567890abcdef", 16);
  check_str (r1_l, r2_l, i++, __LINE__);


  /* test with binary digits */

  str2clint_l (r1_l, "10101010", 2);
  str2clint_l (r2_l, "aa", 16);
  check_str (r1_l, r2_l, i++, __LINE__);


  /* General tests with macro clint2str_l */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 2), 2);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 8), 8);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 10), 10);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 16), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  /* General tests with xclint2str */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 2, 0), 2);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 8, 0), 8);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 10, 0), 10);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 2, 1), 2);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 8, 1), 8);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 10, 1), 10);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 16, 1), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  return 0;
}


static int check_str (CLINT a_l, CLINT b_l, int test, int line)
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
      fprintf (stderr, "Error in str2clint_l() or xclint2str_l() in line %d\n", line);
      fprintf (stderr, "xclint2str_l(a_l, 16, 0) = %s\n", xclint2str_l (a_l, 16, 0));
      exit (-1);
    }
  return 0;
}


static int byte_test (unsigned int nooftests)
{
  unsigned int i = 1;
  UCHAR* ofl;
  UCHAR* N;
  int len;

  printf ("Test byte2clint_l(), clint2byte_l()...\n");

  /* test with value > max_l */
  if (( ofl = (UCHAR*) malloc (sizeof (char) * (CLINTMAXBYTE + 2))) == NULL)
    {
      fprintf (stderr, "Error in malloc");
      exit (-1);
    }

  ofl[0] = '1';                       /* Generate byte vector "1000...0" with */
  for (i = 1; i <= CLINTMAXBYTE; i++) /* CLINTMAXBYTE zeros                   */
    {
      ofl[i] = '0';
    }

  if (E_CLINT_OFL != byte2clint_l (r2_l, ofl, (int)CLINTMAXBYTE + 1))
    {
      fprintf (stderr, "Error in byte2clint_l near line %d: Overflow not detected.\n", __LINE__);
      exit (-1);
    }


  /* test with byte vector of length 0 */
  N = clint2byte_l (one_l, &len);
  byte2clint_l (r2_l, N, 0);
  check_byte (r2_l, nul_l, i++, __LINE__);


  /* test with 0 */
  setzero_l (r1_l);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* Test constant 10945 */
  u2clint_l (r1_l, 10945);
  N = clint2byte_l (r1_l, &len);
  if ((len != 2) || (*N != 0x2A) || (*(N+1) != 0xC1))
    {
      fprintf (stderr, "Error in byte2clint_l near line %d: Incorrect handling of Constant.\n", __LINE__);
      exit (-1);
    }

  ofl[0] = 0x00;
  ofl[1] = 0x2A;
  ofl[2] = 0xC1;
  byte2clint_l (r2_l, ofl, 3);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* test with max_l */
  setmax_l (r1_l);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* test with 0 and leading zeros */
  ldzrand_l (r1_l, 0);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* test with 1 */
  setone_l (r1_l);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);

  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* test with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      N = clint2byte_l (r1_l, &len);
      byte2clint_l (r2_l, N, len);
      check_byte (r2_l, r1_l, i++, __LINE__);
    }


  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      N = clint2byte_l (r1_l, &len);
      byte2clint_l (r2_l, N, len);
      check_byte (r2_l, r1_l, i++, __LINE__);
    }

  free (ofl);
  return 0;
}


static int check_byte (CLINT a_l, CLINT b_l, int test, int line)
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
      fprintf (stderr, "Error in byte2clint_l() or clint2byte_l() in line %d\n", line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}


static int setmax_test (void)
{
  unsigned int i;

  printf ("Test setmax_l()...\n");

  /* Set r0_l to the maximum value */
  for (i = 1; i <= CLINTMAXDIGIT; i++)
    {
      r0_l[i] = BASEMINONE;
    }
  setdigits_l (r0_l, CLINTMAXDIGIT);

  setmax_l (r1_l);

  if (!equ_l (r0_l, r1_l))
    {
      fprintf (stderr, "Error in setmax_l: r0_l != r1_l in line %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l); /*lint !e666*/
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

