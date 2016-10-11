/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testrand.c       Revision: 26.08.2002                               */
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


/*lint -esym(666,disperr_l)*/

static int chi_test (void);
static int rand_test (unsigned int nooftests);

float chisquare (ULONG, ULONG);

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();
  chi_test ();
  rand_test (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int chi_test ()
{
  ULONG seed, i, errcount = 0;
  float X;

  printf ("Chi-Square-Test of usrand64_l()...\n");

  for (i = 0; i < 10; i++)
    {
      seed = ulrand64_l ();
      X = chisquare (0xa00000, seed);
      if (X >= 66380) /* Err. Prob. 0.01 ~ 2^16 + 2*sqrt(2^16) */
        {
          fprintf (stderr, "Warning: Chisquare > r + 2*sqrt(r) :-|\n");
          ++errcount;
        }
      else
        {
          if (X <= 64696) /* Err. Prob. 0.01 ~ 2^16 - 2*sqrt(2^16) */
            {
              fprintf (stderr, "Warning: Chisquare < r - 2*sqrt(r) :-|\n");
              ++errcount;
            }
          else
            {
              fprintf (stderr, "Chi-Square-Test no. %ld O.K. ;-)\n", i + 1);
            }
        }
    }
  if (errcount > 3)
    {
      fprintf (stderr, "Mmm, too many errors in Chi-Square-Test... :-(\n");
    }
  return errcount;
}


static int f[65536 ];

float chisquare (ULONG sample, ULONG seed)
{
  ULONG i, t;

  ulseed64_l (seed);
  for (i = 0; i < 65536; i++)
      f[i] = 0;

  for (i = 0; i < sample; i++)
      f[usrand64_l ()] ++;

  for (i = 0, t = 0; i < 65536; i++)
      t += f[i] * f[i];

  return (float) ((( float ) 65536 * ( float ) t / ( float ) sample ) - sample );
}


static int rand_test (unsigned int nooftests)
{
  unsigned i;
  unsigned r;

  printf ("Test of rand_l ...\n");

  rand_l (r0_l, 0);
  if (!eqz_l (r0_l))
    {
      fprintf (stderr, "Error in rand_l() in line %d\n", __LINE__);
      disperr_l ("r0_l = ", r0_l);
      exit (-1);
    }

  rand_l (r0_l, 1);
  if (!equ_l (r0_l, one_l))
    {
      fprintf (stderr, "Error in rand_l() in line %d\n", __LINE__);
      disperr_l ("r0_l = ", r0_l);
      exit (-1);
    }

  for (i = 0; i < nooftests; i++)
    {
      r = ulrand64_l () % CLINTMAXDIGIT;
      rand_l (r0_l, r);

      if (ld_l (r0_l) != r)
        {
          fprintf (stderr, "Error in rand_l() in line %d\n", __LINE__);
          disperr_l ("r0_l = ", r0_l);
          fprintf (stderr, "r = %d\n", r);
          exit (-1);
        }
    }
  return 0;
}


