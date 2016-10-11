/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testgcd.c        Revision: 19.12.2000                               */
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
#include <time.h>
#include <stdlib.h>

#include "flint.h"

#define MAXTESTLEN (CLINTMAXBIT)
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

static int trivial_tests (void);
static int three_integers_tests (int);
static int distributive_tests (int);
static int general_tests (int);
static int check (CLINT, CLINT, CLINT, CLINT, CLINT, int, CLINT, int, int, int);
static int check_inv (CLINT, CLINT, CLINT, CLINT, CLINT, int, int);


int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();

  trivial_tests ();
  three_integers_tests (1000);
  distributive_tests (1000);
  general_tests (1000);

  printf ("All tests in %s passed.\n", __FILE__);

  return 0;
}


static int
trivial_tests (void)
{
  CLINTD a_l, b_l, c1_l, c2_l, u_l, v_l;
  int i = 1, su, sv;

  printf ("Trivial tests with small constants ...\n");

  setzero_l (a_l);
  setone_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  setone_l (a_l);
  setone_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  setone_l (a_l);
  setzero_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  settwo_l (a_l);
  settwo_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  return 0;
}


static int
three_integers_tests (int nooftests)
{
  CLINT a_l, b_l, c_l, t1_l, t2_l, abc_l;
  int i;

  printf ("Test gcd against lcm with three arguments...\n");

  /* It is true that gcd (a, b, c) * lcm (ab, ac, bc) = a*b*c */

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN/6);     /* a */
      nzrand_l (b_l, CLINTRNDLN/6);     /* b */
      nzrand_l (c_l, CLINTRNDLN/6);     /* c */

      mul_l (a_l, b_l, t1_l);           /* a*b   */

      mul_l (t1_l, c_l, abc_l);         /* a*b*c */

      mul_l (a_l, c_l, t2_l);
      lcm_l (t1_l, t2_l, t1_l);         /* lcm (ab, ac)                */

      mul_l (b_l, c_l, t2_l);           /* lcm (lcm (ab, ac), bc)) =   */
      lcm_l (t1_l, t2_l, t1_l);         /* lcm (ab, ac, bc)            */

      gcd_l (a_l, b_l, t2_l);           /* gcd (a, b)                  */
      gcd_l (t2_l, c_l, t2_l);          /* gcd (gcd (a, b), c)         */

      mul_l (t1_l, t2_l, t2_l);         /* lcm (ab, ac, bc)*gcd (a, b, c) */

      if (!equ_l (abc_l, t2_l))         /* = a*b*c                     */
        {
          printf ("Error in test no. %d near line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int
distributive_tests (int nooftests)
{
  CLINT a_l, b_l, c_l, t1_l, t2_l, t3_l;
  int i;

  printf ("Test gcd against lcm with \"distributive laws\" ...\n");

  /* distributive law: lcm(c, gcd(a, b)) = gcd(lcm(a, c), lcm(b, c)) */

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN/3);     /* a */
      nzrand_l (b_l, CLINTRNDLN/3);     /* b */
      nzrand_l (c_l, CLINTRNDLN/3);     /* c */

      gcd_l (a_l, b_l, t1_l);           /* gcd(a, b)                  */
      lcm_l (c_l, t1_l, t1_l);          /* lcm(c, gcd(a, b))          */

      lcm_l (a_l, c_l, t2_l);
      lcm_l (b_l, c_l, t3_l);

      gcd_l (t2_l, t3_l, t2_l);         /* gcd(lcm(a, c), lcm(b, c))  */

      if (!equ_l (t1_l, t2_l))
        {
          printf ("Error in Test #%d near line %d\n", i, __LINE__);
          exit (-1);
        }
    }


  /* distributive law: gcd(c, lcm(a, b)) = lcm(gcd(a, c), gcd(b, c)) */

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN/3);     /* a */
      nzrand_l (b_l, CLINTRNDLN/3);     /* b */
      nzrand_l (c_l, CLINTRNDLN/3);     /* c */

      lcm_l (a_l, b_l, t1_l);           /* lcm (a, b)                  */
      gcd_l (c_l, t1_l, t1_l);          /* gcd(c, lcm(a, b))           */

      gcd_l (a_l, c_l, t2_l);
      gcd_l (b_l, c_l, t3_l);

      lcm_l (t2_l, t3_l, t2_l);         /* lcm(gcd(a, c), gcd(b, c))  */

      if (!equ_l (t1_l, t2_l))
        {
          printf ("Error in test no. %d near line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int
general_tests (int nooftests)
{
  CLINTD a_l, b_l, c1_l, c2_l, u_l, v_l;
  int i, su, sv;

  printf ("General tests with random parameters ...\n");

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN);
      nzrand_l (b_l, CLINTRNDLN);
      gcd_l (a_l, b_l, c1_l);
      xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
      check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i);
      inv_l (a_l, b_l, u_l, v_l);
      check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);
    }

  return 0;
}


static int
check (CLINT a_l,
       CLINT b_l,
       CLINT c1_l,
       CLINT c2_l,
       CLINT u_l, int su,
       CLINT v_l, int sv,
       int line,
       int test)
{
  int error = 0;
  CLINT uu_l, vv_l;

  if (!equ_l (c1_l, c2_l))
    {
      error = 1;
      printf ("Error in calculation of gcd in test no. %d near line %d\n", test, line);
      disp_l ("gcd:", c1_l);
      disp_l ("\nxgcd:", c2_l);
    }

  mul_l (u_l, a_l, uu_l);
  mul_l (v_l, b_l, vv_l);
  su = sadd (uu_l, su, vv_l, sv, uu_l);
  if (( su != 1 ) || ( !equ_l (uu_l, c2_l)))
    {
      error = 1;
      printf ("Error in calculation of factor u and v in test no. %d near line %d\n", test, line);
      printf ("sign_u = %d, sign_v = %d\n", su, sv);
      disp_l ("u = \n", u_l);
      disp_l ("v = \n", v_l);
      disp_l ("Linear combined gcd  = \n", uu_l);
      disp_l ("Reference-gcd  = \n", c1_l);
    }

  if (error)
      exit (-1);
  else
      return 0;
}


static int
check_inv (CLINT a_l,
           CLINT b_l,
           CLINT c1_l,
           CLINT g_l,
           CLINT i_l,
           int line,
           int test)
{
  int error = 0;
  CLINT n_l;

  if (!equ_l (c1_l, g_l))
    {
      error = 1;
      printf ("Error in calculation of gcd with inv_l() in test #%d near line %d\n", test, line);
      disp_l ("a:", a_l);
      disp_l ("\nb:", b_l);
      disp_l ("\nggT:", c1_l);
      disp_l ("\ninv_l:", g_l);
    }

  if (EQONE_L (g_l) && GT_L (b_l, one_l) )
    {
      mmul_l (a_l, i_l, n_l, b_l);
      if (!EQONE_L (n_l))
        {
          error = 1;
          printf ("Error in calculation of multiplicative inverse with inv_l() in test #%d near line %d\n", test, line);
          disp_l ("a:", a_l);
          disp_l ("\nb:", b_l);
          disp_l ("\ni:", i_l);
          disp_l ("\na * i mod b:", n_l);
        }
    }

  if (error)
      exit (-1);
  else
      return 0;
}
