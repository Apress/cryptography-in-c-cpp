/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testripe.c       Revision: 16.05.2002                               */
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
#include "ripemd.h"

#define TESTFILE1 "testvals/test1.bin"
#define TESTFILE2 "testvals/test2.bin"
#define TESTFILE3 "testvals/test3.bin"

static int check (UCHAR *hashref, UCHAR *HashRes, int Len, int error, int test);


/* Reference values for RIPEMD-128 */

UCHAR clear128_[] = {0xcd,0xf2,0x62,0x13,0xa1,0x50,0xdc,0x3e,0xcb,0x61,0x0f,0x18,0xf6,0xb3,0x8b,0x46};
UCHAR clear128_A_9[] = {0xd1,0xe9,0x59,0xeb,0x17,0x9c,0x91,0x1f,0xae,0xa4,0x62,0x4c,0x60,0xc5,0xc7,0x02};
UCHAR clear128_a[] = {0x86,0xbe,0x7a,0xfa,0x33,0x9d,0x0f,0xc7,0xcf,0xc7,0x85,0xe7,0x2f,0x57,0x8d,0x33};
UCHAR clear128_abc[] = {0xc1,0x4a,0x12,0x19,0x9c,0x66,0xe4,0xba,0x84,0x63,0x6b,0x0f,0x69,0x14,0x4c,0x77};
UCHAR clear128_a_q[] = {0xa1,0xaa,0x06,0x89,0xd0,0xfa,0xfa,0x2d,0xdc,0x22,0xe8,0x8b,0x49,0x13,0x3a,0x06};
UCHAR clear128_a_z[] = {0xfd,0x2a,0xa6,0x07,0xf7,0x1d,0xc8,0xf5,0x10,0x71,0x49,0x22,0xb3,0x71,0x83,0x4e};
UCHAR clear128_1_0[] = {0x3f,0x45,0xef,0x19,0x47,0x32,0xc2,0xdb,0xb2,0xc4,0xa2,0xc7,0x69,0x79,0x5f,0xa3};
UCHAR clear128_M_a[] = {0x4a,0x7f,0x57,0x23,0xf9,0x54,0xeb,0xa1,0x21,0x6c,0x9d,0x8f,0x63,0x20,0x43,0x1f};

/* Reference values for RIPEMD-160 */

UCHAR clear_[] = {0x9c,0x11,0x85,0xa5,0xc5,0xe9,0xfc,0x54,0x61,0x28,0x08,0x97,0x7e,0xe8,0xf5,0x48,0xb2,0x25,0x8d,0x31};
UCHAR clear_A_9[] = {0xb0,0xe2,0x0b,0x6e,0x31,0x16,0x64,0x02,0x86,0xed,0x3a,0x87,0xa5,0x71,0x30,0x79,0xb2,0x1f,0x51,0x89};
UCHAR clear_a[] = {0x0b,0xdc,0x9d,0x2d,0x25,0x6b,0x3e,0xe9,0xda,0xae,0x34,0x7b,0xe6,0xf4,0xdc,0x83,0x5a,0x46,0x7f,0xfe};
UCHAR clear_abc[] = {0x8e,0xb2,0x08,0xf7,0xe0,0x5d,0x98,0x7a,0x9b,0x04,0x4a,0x8e,0x98,0xc6,0xb0,0x87,0xf1,0x5a,0x0b,0xfc};
UCHAR clear160_a_q[] = {0x12,0xa0,0x53,0x38,0x4a,0x9c,0x0c,0x88,0xe4,0x05,0xa0,0x6c,0x27,0xdc,0xf4,0x9a,0xda,0x62,0xeb,0x2b};
UCHAR clear_a_z[] = {0xf7,0x1c,0x27,0x10,0x9c,0x69,0x2c,0x1b,0x56,0xbb,0xdc,0xeb,0x5b,0x9d,0x28,0x65,0xb3,0x70,0x8d,0xbc};
UCHAR clear_1_0[] = {0x9b,0x75,0x2e,0x45,0x57,0x3d,0x4b,0x39,0xf4,0xdb,0xd3,0x32,0x3c,0xab,0x82,0xbf,0x63,0x32,0x6b,0xfb};
UCHAR clear_M_a[] = {0x52,0x78,0x32,0x43,0xc1,0x69,0x7b,0xdb,0xe1,0x6d,0x37,0xf9,0x7f,0x68,0xf0,0x83,0x25,0xdc,0x15,0x28};
UCHAR clear_bin1[] = {0xea,0xe5,0xe8,0x49,0x85,0x03,0xe8,0x25,0xb2,0xa0,0xa7,0x4f,0x54,0xf2,0xad,0x6f,0x5f,0xb3,0xb1,0xd8};
UCHAR clear_bin2[] = {0xe1,0x1f,0x59,0xa6,0x96,0x1d,0xde,0xa9,0xec,0x63,0xe7,0x65,0xbb,0x70,0xd3,0x7b,0x44,0x46,0x01,0x6f};
UCHAR clear_bin3[] = {0x4c,0xb3,0x3d,0xb6,0x14,0x35,0x42,0xd9,0x2c,0x9b,0x6d,0xfc,0xfd,0xa8,0x1b,0x63,0xa9,0x23,0xb2,0x04};

int main ()
{
  int error = 0;
  unsigned i, nbytes;
  RMDSTAT hws;
  UCHAR HashRes[RMDVER>>3];
  UCHAR data[1024];
  FILE *mf;

  printf ("Testmodule %s, compiled for ripemd.c as part of FLINT/C-Library\n", __FILE__);

  /* Tests RIPEMD-128 */
  printf ("Testing RIPEMD-128\n");

  /* Test #1: Empty String */
  ripemd128 (HashRes, ( UCHAR * ) "", 0);
  check (clear128_, HashRes, 16, error, 1);

  /* Test #2: String "a" */
  ripemd128 (HashRes, ( UCHAR * ) "a", 1);
  check (clear128_a, HashRes, 16, error, 2);

  /* Test #3: String "abc" */
  ripemd128 (HashRes, ( UCHAR * ) "abc", 3);
  check (clear128_abc, HashRes, 16, error, 3);

  /* Test #4: String "a-q" */
  ripemd128 (HashRes, ( UCHAR * ) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
  check (clear128_a_q, HashRes, 16, error, 4);

  /* Test #5: String "a-z" */
  ripemd128 (HashRes, ( UCHAR * ) "abcdefghijklmnopqrstuvwxyz", 26);
  check (clear128_a_z, HashRes, 16, error, 5);

  /* Test #6: String "A .. Za .. z0 .. 9" */
  ripemd128 (HashRes, ( UCHAR * ) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 62);
  check (clear128_A_9, HashRes, 16, error, 6);

  /* Test #7: 8 times "1234567890" with ripehash() and ripefinish() */
  ripeinit128 (&hws);
  ripehash128 (&hws, ( UCHAR * ) "1234567890123456789012345678901234567890123456789012345678901234", 64);
  ripefinish128 (HashRes, &hws, ( UCHAR * ) "5678901234567890", 16);
  check (clear128_1_0, HashRes, 16, error, 7);

  /* Test #8: 1 million times 'a' with ripehash() and ripefinish() */
  memset (data, 'a', 1024);
  ripeinit128 (&hws);
  for (i = 0; i < 976; i++)
    {
      error = ripehash128 (&hws, data, 1024);
    }
  ripefinish128 (HashRes, &hws, data, 576);
  check (clear128_M_a, HashRes, 16, error, 8);


  /* Tests RIPEMD-160 */
  printf ("Testing RIPEMD-160\n");

  /* Test #1: Empty String */
  ripemd160 (HashRes, ( UCHAR * ) "", 0);
  check (clear_, HashRes, 20, error, 1);

  /* Test #2: String "a" */
  ripemd160 (HashRes, ( UCHAR * ) "a", 1);
  check (clear_a, HashRes, 20, error, 2);

  /* Test #3: String "abc" */
  ripemd160 (HashRes, ( UCHAR * ) "abc", 3);
  check (clear_abc, HashRes, 20, error, 3);

  /* Test #4: String "a-q" */
  ripemd160 (HashRes, ( UCHAR * ) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
  check (clear160_a_q, HashRes, 16, error, 4);

  /* Test #5: String "a-z" */
  ripemd160 (HashRes, ( UCHAR * ) "abcdefghijklmnopqrstuvwxyz", 26);
  check (clear_a_z, HashRes, 20, error, 5);

  /* Test #6: String "A .. Za .. z0 .. 9" */
  ripemd160 (HashRes, ( UCHAR * ) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 62);
  check (clear_A_9, HashRes, 20, error, 6);

  /* Test #7: 8 times "1234567890" in ripehash() and ripefinish() */
  ripeinit (&hws);
  ripehash (&hws, ( UCHAR * ) "1234567890123456789012345678901234567890123456789012345678901234", 64);
  ripefinish (HashRes, &hws, ( UCHAR * ) "5678901234567890", 16);
  check (clear_1_0, HashRes, 20, error, 7);

  /* Test #8: 1 million times 'a' in ripehash() and ripefinish() */
  memset (data, 'a', 1024);
  ripeinit (&hws);
  for (i = 0; i < 976; i++)
    {
      error = ripehash (&hws, data, 1024);
    }
  ripefinish (HashRes, &hws, data, 576);
  check (clear_M_a, HashRes, 20, error, 8);

  /* Test #9: testfile 1 */
  printf ("Testfile %s ...\n", TESTFILE1);
  ripeinit (&hws);

  if (( mf = fopen (TESTFILE1, "rb")) == NULL)
    {
      fprintf (stderr, "Error: %s cannot open file %s.\n", __FILE__, TESTFILE1);
      exit (-1);
    }

  while ((( nbytes = fread (data, 1, 1024, mf)) != 0 ) && ( nbytes % 64 == 0 ))
    {
      if (!error)
        {
          error = ripehash (&hws, data, nbytes);
        }
      else
        {
          ripehash (&hws, data, nbytes);
        }
    }
  ripefinish (HashRes, &hws, data, nbytes);
  check (clear_bin1, HashRes, 20, error, 9);
  fclose (mf);

  /* testfile "test2.bin" */
  printf ("Testfile %s ...\n", TESTFILE2);
  ripeinit (&hws);

  if (( mf = fopen (TESTFILE2, "rb")) == NULL)
    {
      fprintf (stderr, "Error: %s cannot open file %s.\n", __FILE__, TESTFILE2);
      exit (1);
    }

  while ((( nbytes = fread (data, 1, 1024, mf)) != 0 ) && ( nbytes % 64 == 0 ))
    {
      if (!error)
        {
          error = ripehash (&hws, data, nbytes);
        }
      else
        {
          ripehash (&hws, data, nbytes);
        }
    }

  ripefinish (HashRes, &hws, data, nbytes);
  check (clear_bin2, HashRes, 20, error, 10);
  fclose (mf);

  /* testfile "test3.bin" */
  printf ("Testfile %s ...\n", TESTFILE3);
  ripeinit (&hws);

  if (( mf = fopen (TESTFILE3, "rb")) == NULL)
    {
      fprintf (stderr, "Error: %s cannot open file %s.\n", __FILE__, TESTFILE3);
      exit (1);
    }

  while ((( nbytes = fread (data, 1, 1024, mf)) != 0 ) && ( nbytes % 64 == 0 ))
    {
      if (!error)
        {
          error = ripehash (&hws, data, nbytes);
        }
      else
        {
          ripehash (&hws, data, nbytes);
        }
    }

  ripefinish (HashRes, &hws, data, nbytes);
  check (clear_bin3, HashRes, 20, error, 11);
  fclose (mf);

  printf ("All tests in %s passed.\n", __FILE__);

  return 0;
}


static int check (UCHAR *hashref, UCHAR *HashRes, int Len, int error, int test)
{
  int i;
  if (error)
    {
      fprintf (stderr, "Error in RIPEMD in test %d: Error code %d\n", test, error);
    }

  if (memcmp (hashref, HashRes, Len))
    {
      fprintf (stderr, "Error in RIPEMD in test %d: Message Digest invalid\n", test);
      for (i = 0; i < 20; i++)
        {
          fprintf (stderr, "%2x ", HashRes[i] & 0x00ff);
        }
      fprintf (stderr, "\n");
      exit (1);
    }

  fprintf (stderr, "Test #%d O.K.\n", test);

  return 0;
}


