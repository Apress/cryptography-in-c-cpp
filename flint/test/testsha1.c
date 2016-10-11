/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testsha1.c       Revision: 16.05.2002                               */
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flint.h"
#include "sha1.h"


static int check (UCHAR *HashRef, UCHAR *HashRes, int Len, int error, int test);


/* Reference values for SHA-1 */

UCHAR clear_abc[] = {0xa9,0x99,0x3e,0x36,0x47,0x06,0x81,0x6a,0xba,0x3e,0x25,0x71,0x78,0x50,0xc2,0x6c,0x9c,0xd0,0xd8,0x9d};
UCHAR clear_a_q[] = {0x84,0x98,0x3e,0x44,0x1c,0x3b,0xd2,0x6e,0xba,0xae,0x4a,0xa1,0xf9,0x51,0x29,0xe5,0xe5,0x46,0x70,0xf1};
UCHAR clear_M_a[] = {0x34,0xaa,0x97,0x3c,0xd4,0xc4,0xda,0xa4,0xf6,0x1e,0xeb,0x2b,0xdb,0xad,0x27,0x31,0x65,0x34,0x01,0x6f};

int main ()
{
  int error = 0;
  unsigned i;
  SHASTAT hws;
  UCHAR HashRes[SHALEN>>3];
  UCHAR data[1024];

  printf ("Testmodule %s, compiled for sha1.c as part of FLINT/C-Library\n", __FILE__);

  printf ("Testing SHA-1\n");

  /* Test #1: String "abc" */
  sha1 (HashRes, ( UCHAR * ) "abc", 3);
  check (clear_abc, HashRes, 20, error, 1);

  /* Test #2: String "a-q */
  sha1 (HashRes, ( UCHAR * ) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
  check (clear_a_q, HashRes, 20, error, 2);

  /* Test #3: 1 million times 'a' in sha1hash() und sha1finish() */
  memset (data, 'a', 1024);
  sha1init (&hws);
  for (i = 0; i < 976; i++)
    {
      error = sha1hash (&hws, data, 1024);
    }
  sha1finish (HashRes, &hws, data, 576);
  check (clear_M_a, HashRes, 20, error, 3);

  printf ("All tests in %s passed.\n", __FILE__);

  return 0;
}


static int check (UCHAR *HashRef, UCHAR *HashRes, int Len, int error, int test)
{
  int i;
  if (error)
    {
      fprintf (stderr, "Error in SHA-1 in test %d: Error code %d\n", test, error);
    }

  if (memcmp (HashRef, HashRes, Len))
    {
      fprintf (stderr, "Error in SHA-1 in test %d: Message Digest invalid\n", test);
      for (i = 0; i < Len; i++)
        {
          fprintf (stderr, "%2x ", HashRes[i] & 0x00ff);
        }
      fprintf (stderr, "\n");
      exit (1);
    }

  fprintf (stderr, "Test #%d O.K.\n", test);

  return 0;
}


