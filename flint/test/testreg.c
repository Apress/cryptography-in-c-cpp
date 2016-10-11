/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testreg.c        Revision: 19.12.2000                               */
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

static int reg_test (void);
static int regs_test (void);

#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();

  reg_test ();
  regs_test ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}



static int reg_test ()
{
  unsigned int i;
  clint *r_l;
  if (NULL == ( r_l = create_l ()))
    {
      fprintf (stderr, "Error in create_l(): NULL zurueck in line %d\n", __LINE__);
      exit (-1);
    }
  rand_l (r_l, CLINTMAXDIGIT);
  purge_l (r_l);
  for (i = 0; i <= CLINTMAXDIGIT; i++)
    {
      if (r_l[i] != 0)
        {
          fprintf (stderr, "Error in purge_l(): r_l[%d] != 0 in line %d\n", i, __LINE__);
          exit (-1);
        }
    }
  free_l (r_l);
  return 0;
}


static int regs_test ()
{
  unsigned int i;
  CLINT a_l;

#ifndef FLINT_USEDLL
  create_reg_l ();
#endif

  for (i = 0; i < NOOFREGS; i++)
    {
      if (NULL == get_reg_l (i))
        {
          fprintf (stderr, "Error in create_reg_l(): get_reg_l(%d) == NULL in line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  rand_l (r0_l, CLINTMAXDIGIT);
  cpy_l (a_l, r0_l);
  create_reg_l ();
  free_reg_l ();
  if (NULL == r0_l)
    {
      fprintf (stderr, "Error in create_reg_l()/free_reg_l(): get_reg_l(0) == NULL in line %d\n", __LINE__);
      exit (-1);
    }
  if (!equ_l (r0_l, a_l))
    {
      fprintf (stderr, "Error in create_reg_l()/free_reg_l(): r_l != a_l in line %d\n", __LINE__);
      disperr_l ("r0_l =", r0_l); /*lint !e666 */
      exit (-1);
    }

  purge_reg_l (0);
  for (i = 0; i <= CLINTMAXDIGIT; i++)
    {
      if (r0_l[i] != 0)
        {
          fprintf (stderr, "Error in purge_reg_l(): r0_l[%d] != 0 in line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  set_noofregs_l (100);

  purgeall_reg_l ();
  free_reg_l ();
  if (NULL != r0_l)
    {
      fprintf (stderr, "Error in create_reg_l()/free_reg_l(): get_reg_l(0) != NULL in line %d\n", __LINE__);
      exit (-1);
    }

  if (0 != create_reg_l ())
    {
      fprintf (stderr, "Error: create_reg_l() != 0 in line %d\n", __LINE__);
      exit (-1);
    }

  for (i = 0; i < 100; i++)
    {
      if (NULL == get_reg_l (i))
        {
          fprintf (stderr, "Error in create_reg_l(): get_reg_l(%d) == NULL in line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  if (get_reg_l (100) != NULL)
    {
      fprintf (stderr, "Error in get_reg_l() get_reg_l(100) != NULL in line %d\n", __LINE__);
      disperr_l ("r0_l =", r0_l); /*lint !e666 */
      exit (-1);
    }

  purgeall_reg_l ();

#ifndef FLINT_USEDLL
  free_reg_l ();
#endif

  return 0;
}

