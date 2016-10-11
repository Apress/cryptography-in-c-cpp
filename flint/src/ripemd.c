/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module ripemd.c         Revision: 01.12.2002                               */
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
#include <string.h>
#include "flint.h"
#include "ripemd.h"

#ifndef inline
#if (_MSC_VER >= 1100) || defined __GNUC__
#define inline __inline
#else 
#define inline /**/
#endif
#endif

/* Prototypes of local functions */

static void appetize (ULONG *);
static void swallow (ULONG *, ULONG *);
static void digest (ULONG *, UCHAR *, ULONG[]);

static void appetize128 (ULONG *);
static void swallow128 (ULONG *, ULONG *);
static void digest128 (ULONG *, UCHAR *, ULONG[]);

#ifdef FLINT_SECURE
static inline void ZeroUlong (ULONG *);
static inline void Zero2Ulong (ULONG *, ULONG *);
static inline void Zero4Ulong (ULONG *, ULONG *, ULONG *, ULONG *);
static inline void ZeroUcharArray (void *, size_t);
#endif

/* Macros */

/* Rotation, simulation of assembler instruction for barrel shifting */
#define ROL(X, N)   (((X) << (N)) | ((X) >> (32-(N))))

/* RIPEMD specific chaining of intermediate values in rounds */
#define CHAIN128(A, B, C, D, X, S) { \
          (A) = (D); (D) = (C); (C) = (B); \
          (B) = ROL((X), (S)); \
        }

#define CHAIN160(A, B, C, D, E, X, S) { \
          (A) = (E); (E) = (D); (D) = ROL((C), 10); (C) = (B); \
          (B) = ROL((X), (S)) + (A); \
        }

/* Conversion of an array of type UCHAR into a value of type ULONG */
#if defined LITTLE_ENDIAN
#define UC2UL(ucptr)  *(unsigned long *)((ucptr))
#else
#define UC2UL(ucptr) \
          (((unsigned long) *((ucptr)+3) << 24) | \
           ((unsigned long) *((ucptr)+2) << 16) | \
           ((unsigned long) *((ucptr)+1) <<  8) | \
           ((unsigned long) *(ucptr)))
#endif


/* Addition of a single-digit number b to a double-digit number represented */
/* by an array (a[1],a[0]). A carry in a[0]+=b is handled by increasing a[1]. */
#define ADDC(a, b) { \
          if (((a)[0] + (b)) < (a)[0]) {(a)[1]++;} \
          (a)[0] += (b); \
        }

/* Constants */
/* Choice of message words... */
static const int
r1[] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
         7,  4, 13,  1, 10,  6, 15,  3, 12,  0,  9,  5,  2, 14, 11,  8,
         3, 10, 14,  4,  9, 15,  8,  1,  2,  7,  0,  6, 13, 11,  5, 12,
         1,  9, 11, 10,  0,  8, 12,  4, 13,  3,  7, 15, 14,  5,  6,  2,
         4,  0,  5,  9,  7, 12,  2, 10, 14,  1,  3,  8, 11,  6, 15, 13};

/* ... for parallel rounds */
static const int
r2[] = { 5, 14,  7,  0,  9,  2, 11,  4, 13,  6, 15,  8,  1, 10,  3, 12,
         6, 11,  3,  7,  0, 13,  5, 10, 14, 15,  8, 12,  4,  9,  1,  2,
        15,  5,  1,  3,  7, 14,  6,  9, 11,  8, 12,  2, 10,  0,  4, 13,
         8,  6,  4,  1,  3, 11, 15,  0,  5, 12,  2, 13,  9,  7, 10, 14,
        12, 15, 10,  4,  1,  5,  8,  7,  6,  2, 13, 14,  0,  3,  9, 11};

/* Length of rotations... */
static const int
s1[] = {11, 14, 15, 12,  5,  8,  7,  9, 11, 13, 14, 15,  6,  7,  9,  8,
         7,  6,  8, 13, 11,  9,  7, 15,  7, 12, 15,  9, 11,  7, 13, 12,
        11, 13,  6,  7, 14,  9, 13, 15, 14,  8, 13,  6,  5, 12,  7,  5,
        11, 12, 14, 15, 14, 15,  9,  8,  9, 14,  5,  6,  8,  6,  5, 12,
         9, 15,  5, 11,  6,  8, 13, 12,  5, 12, 13, 14, 11,  8,  5,  6};

/* ... for parallel rounds */
static const int
s2[] = { 8,  9,  9, 11, 13, 15, 15,  5,  7,  7,  8, 11, 14, 14, 12,  6,
         9, 13, 15,  7, 12,  8,  9, 11,  7,  7, 12,  7,  6, 15, 13, 11,
         9,  7, 15, 11,  8,  6,  6, 14, 12, 13,  5, 14, 13, 13,  7,  5,
        15,  5,  8, 11, 14, 14,  6, 14,  6,  9, 12,  9, 12,  5, 15,  8,
         8,  5, 12,  9, 12,  5, 14,  6,  8, 13,  6,  5, 15, 13, 11, 11};


/*****************************************************************************/
/* RIPEMD-160 API                                                            */
/*****************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message of length byte in a single step          */
/*  Syntax:     void                                                          */
/*              ripemd160_l (UCHAR *HashRes, UCHAR *clear, ULONG length);     */
/*  Input:      UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block)                        */
/*  Output:     UCHAR *HashRes (Hash Value)                                   */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripemd160_l (UCHAR *HashRes, UCHAR *clear, ULONG length)
{
  RMDSTAT hws;

  ripeinit_l (&hws);
  ripefinish_l (HashRes, &hws, clear, length);

#ifdef FLINT_SECURE
  ZeroUcharArray (&hws, sizeof (hws));
#endif /* FLINT_SECURE */

  return;
}


/******************************************************************************/
/* Functions for blockwise hashing a message in several steps                 */
/* Procedure: Initialization with ripeinit                                    */
/*            Hashing of block1, block2,... ,blockn with function ripehash    */
/*            Finish operation with function ripefinish                       */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Initialization of RIPEMD-160 function                         */
/*  Syntax:     void ripeinit_l (RMDSTAT *hws);                               */
/*  Input:      RMDSTAT *hws (RIPEMD status buffer)                           */
/*  Output:     -                                                             */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripeinit_l (RMDSTAT *hws)
{
  appetize (hws->stomach);
  hws->total[0] = 0;
  hws->total[1] = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message block                                    */
/*  Syntax:     int ripehash_l (RMDSTAT *hws, UCHAR *clear, ULONG length);    */
/*  Input:      RMDSTAT *hws (RIPEMD status buffer)                           */
/*              UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block i bytes = 0 mod 64)     */
/*  Output:     -                                                             */
/*  Returns:    E_CLINT_OK if everything is O.K.                              */
/*              E_CLINT_RMD if length != 0 mod 64                             */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ripehash_l (RMDSTAT *hws, UCHAR *clear, ULONG length)
{
  ULONG ULBlock[16];    /* message block holding 16 values of type ULONG  */
  ULONG noofblocks;     /* Number of blocks of 16 ULONG-vyluse            */
  ULONG i, j;           /* Counter                                        */

  /* If incomplete 64 byte block exists... */
  if (length & 63)
    {
      return E_CLINT_RMD; /* ...return error code */
    }

  /* Number of 64 byte message blocks in clear */
  noofblocks = length >> 6;

  /* Process 64 byte message blocks in clear */
  for (i = 0; i < noofblocks; i++)
    {
        for (j = 0; j < 16; j++)
          {
            ULBlock[j] = UC2UL (clear);
            clear += 4;
          }

        swallow (hws->stomach, ULBlock);
    }

  /* Add length of message in clear to hws->total */
  ADDC (hws->total, length);

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&ULBlock[0], &ULBlock[1], &ULBlock[2], &ULBlock[3]);
  Zero4Ulong (&ULBlock[4], &ULBlock[5], &ULBlock[6], &ULBlock[7]);
  Zero4Ulong (&ULBlock[8], &ULBlock[9], &ULBlock[10], &ULBlock[11]);
  Zero4Ulong (&ULBlock[12], &ULBlock[13], &ULBlock[14], &ULBlock[15]);
#endif

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Finish hash function RIPEMD-160                               */
/*  Syntax:     void ripefinish_l (UCHAR *HashRes, RMDSTAT *hws,              */
/*                                 UCHAR *clear, ULONG length);               */
/*  Input:      RMDSTAT *hws (RIPEMD status buffer)                           */
/*              UCHAR *clear (Pointer to the last message block               */
/*              ULONG length (Length of message block in bytes)               */
/*  Output:     UCHAR HashRes (20 byte hash value)                            */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripefinish_l (UCHAR *HashRes, RMDSTAT *hws, UCHAR *clear, ULONG length)
{
  unsigned i;

  /* Number of bytes in complete blocks */
  ULONG blength = (length >> 6) << 6;

  /* Process complete blocks in clear */
  ripehash_l (hws, clear, blength);

  /* Add length of message in clear to hws->total */
  ADDC (hws->total,length - blength);

  /* Process last incomplete block with padding and length of message */
  digest (hws->stomach, clear + blength, hws->total);

  for (i = 0; i < 20; i += 4)
    {
      *(HashRes+i)   = (UCHAR)hws->stomach[i>>2];
      *(HashRes+i+1) = (UCHAR)(hws->stomach[i>>2] >>  8); /*lint !e661 !e662 */
      *(HashRes+i+2) = (UCHAR)(hws->stomach[i>>2] >> 16); /*lint !e661 !e662 */
      *(HashRes+i+3) = (UCHAR)(hws->stomach[i>>2] >> 24); /*lint !e661 !e662 */
    }

  return;
}


/*****************************************************************************/
/* RIPEMD-128 API                                                            */
/*****************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message of length byte in a single step          */
/*  Syntax:     void                                                          */
/*              ripemd128_l (UCHAR *HashRes, UCHAR *clear, ULONG length);     */
/*  Input:      UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block)                        */
/*  Output:     UCHAR *HashRes (Hash Value)                                   */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripemd128_l (UCHAR *HashRes, UCHAR *clear, ULONG length)
{
  RMDSTAT hws;

  ripeinit128_l (&hws);
  ripefinish128_l (HashRes, &hws, clear, length);

#ifdef FLINT_SECURE
  ZeroUcharArray (&hws, sizeof (hws));
#endif
}


/******************************************************************************/
/* Functions for blockwise hashing a message in several steps                 */
/* Procedure: Initialization with ripeinit                                    */
/*            Hashing of block1, block2,... ,blockn with function ripehash    */
/*            Finish operation with function ripefinish                       */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Initialization of RIPEMD-128 function                         */
/*  Syntax:     void ripeinit128_l (RMDSTAT *hws);                            */
/*  Input:      RMDSTAT *hws (RIPEMD status buffer)                           */
/*  Output:     -                                                             */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripeinit128_l (RMDSTAT *hws)
{
  appetize128 (hws->stomach);
  hws->total[0] = 0;
  hws->total[1] = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message block                                    */
/*  Syntax:     int ripehash128_l (RMDSTAT *hws, UCHAR *clear, ULONG length); */
/*  Input:      RMDSTAT *hws (RIPEMD status buffer)                           */
/*              UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block i bytes = 0 mod 64)     */
/*  Output:     -                                                             */
/*  Returns:    E_CLINT_OK if everything is O.K.                              */
/*              E_CLINT_RMD if length != 0 mod 64                             */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ripehash128_l (RMDSTAT *hws, UCHAR *clear, ULONG length)
{
  ULONG ULBlock[16];    /* aktueller 16-ULONG (512 Bit) Message-Block */
  ULONG noofblocks;     /* Anzahl 16-ULONG- (512 Bit-) Bloecke        */
  ULONG i, j;           /* Zaehler                                    */

  /* If incomplete 64 byte block exists... */
  if (length & 63)
    {
      return E_CLINT_RMD;          /* ...return error code */
    }

  /* Number of 64 byte message blocks in clear */
  noofblocks = length >> 6;

  /* Process 64 byte message blocks in clear */
  for (i = 0; i < noofblocks; i++)
    {
        for (j = 0; j < 16; j++)
          {
            ULBlock[j] = UC2UL (clear);
            clear += 4;
          }

        swallow128 (hws->stomach, ULBlock);
    }

  /* Add length of message in clear to hws->total */
  ADDC (hws->total, length);

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&ULBlock[0], &ULBlock[1], &ULBlock[2], &ULBlock[3]);
  Zero4Ulong (&ULBlock[4], &ULBlock[5], &ULBlock[6], &ULBlock[7]);
  Zero4Ulong (&ULBlock[8], &ULBlock[9], &ULBlock[10], &ULBlock[11]);
  Zero4Ulong (&ULBlock[12], &ULBlock[13], &ULBlock[14], &ULBlock[15]);
#endif

  return 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Finish hash function RIPEMD-128                               */
/*  Syntax:     void ripefinish128_l (UCHAR *HashRes, RMDSTAT *hws,           */
/*                                    UCHAR *clear, ULONG length);            */
/*  Input:      RMDSTAT *hws (RIPEMD status buffer)                           */
/*              UCHAR *clear (Pointer to the last message block               */
/*              ULONG length (Length of message block in bytes)               */
/*  Output:     UCHAR HashRes (16 byte hash value)                            */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripefinish128_l (UCHAR *HashRes, RMDSTAT *hws, UCHAR *clear, ULONG length)
{
  unsigned i;

  /* Number of bytes in complete blocks */
  ULONG blength = (length >> 6) << 6;

  /* Process complete blocks in clear */
  ripehash128_l (hws, clear, blength);

  /* Add length of message in clear to hws->total */
  ADDC (hws->total, length - blength);

  /* Process last incomplete block with padding and length of message */
  digest128 (hws->stomach, clear + blength, hws->total);

  for (i = 0; i < 16; i += 4)
    {
      *(HashRes+i)   = (UCHAR)hws->stomach[i>>2];
      *(HashRes+i+1) = (UCHAR)(hws->stomach[i>>2] >>  8); /*lint !e661 !e662 */
      *(HashRes+i+2) = (UCHAR)(hws->stomach[i>>2] >> 16); /*lint !e661 !e662 */
      *(HashRes+i+3) = (UCHAR)(hws->stomach[i>>2] >> 24); /*lint !e661 !e662 */
    }

  return;
}


/******************************************************************************/
/* RIPEMD-160 kernel functions                                                */
/******************************************************************************/

static void
appetize (ULONG *stomach)
{
  stomach[0] = 0x67452301UL;
  stomach[1] = 0xefcdab89UL;
  stomach[2] = 0x98badcfeUL;
  stomach[3] = 0x10325476UL;
  stomach[4] = 0xc3d2e1f0UL;

  return;
}


static void
swallow (ULONG *stomach, ULONG *ULBlock)
{
  int round, rol;
  ULONG x;

  ULONG a1 = stomach[0];
  ULONG b1 = stomach[1];
  ULONG c1 = stomach[2];
  ULONG d1 = stomach[3];
  ULONG e1 = stomach[4];

  ULONG a2 = stomach[0];
  ULONG b2 = stomach[1];
  ULONG c2 = stomach[2];
  ULONG d2 = stomach[3];
  ULONG e2 = stomach[4];

  /*lint -e123 Don't complain about "Macros ... defined with arguments" */

  /* Rounds and parallel rounds 0-15 */
  for (round = 0; round < 16; round++)
    {
      rol = s1[round];
      x = a1 + (b1 ^ c1 ^ d1) + ULBlock[round];
      CHAIN160 (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (b2 ^ (c2 | ~d2)) + ULBlock[r2[round]] + 0x50a28BE6UL;
      CHAIN160 (a2, b2, c2, d2, e2, x, rol);
    }

  /* Rounds and parallel rounds 16-31 */
  for (round = 16; round < 32; round++)
    {
      rol = s1[round];
      x = a1 + (d1 ^ (b1 & (c1 ^ d1))) + ULBlock[r1[round]] + 0x5A827999UL;
      CHAIN160 (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (c2 ^ (d2 & (b2 ^ c2))) + ULBlock[r2[round]] + 0x5C4Dd124UL;
      CHAIN160 (a2, b2, c2, d2, e2, x, rol);
    }

  /* Rounds and parallel rounds 32-47 */
  for (round = 32; round < 48; round++)
    {
      rol = s1[round];
      x = a1 + ((b1 | ~c1) ^ d1) + ULBlock[r1[round]] + 0x6ED9EBA1UL;
      CHAIN160 (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + ((b2 | ~c2) ^ d2) + ULBlock[r2[round]] + 0x6D703EF3UL;
      CHAIN160 (a2, b2, c2, d2, e2, x, rol);
    }

  /* Rounds and parallel rounds 48-63 */
  for (round = 48; round < 64; round++)
    {
      rol = s1[round];
      x = a1 + (c1 ^ (d1 & (b1 ^ c1))) + ULBlock[r1[round]] + 0x8F1BBCDCUL;
      CHAIN160 (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (d2 ^ (b2 & (c2 ^ d2))) + ULBlock[r2[round]] + 0x7A6D76E9UL;
      CHAIN160 (a2, b2, c2, d2, e2, x, rol);
    }

  /* Rounds and parallel rounds 64-79 */
  for (round = 64; round < 80; round++)
    {
      rol = s1[round];
      x = a1 + (b1 ^ (c1 | ~d1)) + ULBlock[r1[round]] + 0xA953FD4EUL;
      CHAIN160 (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (b2 ^ c2 ^ d2) + ULBlock[r2[round]];
      CHAIN160 (a2, b2, c2, d2, e2, x, rol);
    }

  /* Result in stomach */
  d2 += c1 + stomach[1];
  stomach[1] = stomach[2] + d1 + e2;
  stomach[2] = stomach[3] + e1 + a2;
  stomach[3] = stomach[4] + a1 + b2;
  stomach[4] = stomach[0] + b1 + c2;
  stomach[0] = d2;

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&a1, &b1, &c1, &d1);
  Zero4Ulong (&e1, &a2, &b2, &c2);
  Zero4Ulong (&d2, &e2, &x, &a1);
#endif
  
  return;
}


static void
digest (ULONG *stomach, UCHAR *clear, ULONG total[])
{
  ULONG i,j, rest;
  ULONG ULBlock[16];

  memset (ULBlock, 0, sizeof (ULONG) << 4);

  /* Padding to achieve block length of 512 Bit                       */
  /* by example of the message "abc" of length l = 24 bit:            */
  /* 1. One bit "1" is appended to the end of the masseage.           */
  /* 2. k "0"-bits are appended, with k the smallest non-negative     */
  /*    solution of l+1+k = 448 mod 512, in this example k = 448-1-24 */
  /*    = 423, 7 bit following the single "1" plus additional         */
  /*    13 ULONGs (i.e. 416 bit) in the positions ULBlock[1]...[13].  */
  /* 3. The length of the message is appended in 64 bit (2 ULONGs).   */
  /*                                                                  */
  /*    01100001 01100010 01100011 1 000...00 00...011000             */
  /*       "a"      "b"      "c"   1+ 423Bit   64 bit (length)        */

  /* Message length modulo 64 ULONG-blocks (512 bit) */
  rest = total[0] & 0x3f;

  /* Insert ULONGs into ULBlock */
  for (i = 0; i < (rest >> 2); i++)
    {
      ULBlock[i] = UC2UL (clear);
      clear += 4;
    }

  /* Remaining UCHARs go into ULBLock. Invariant: 0 <= i <= 15 */
  for (j = i << 2; j < rest; j++)
    {
      ULBlock[i] |= (ULONG)*clear++ << ((j & 3) << 3);
    }

  /* Append 0x80 to ULBlock: At least one byte is still free */
  ULBlock[i] |= (ULONG)0x80 << ((j & 3) << 3);

  if (rest > 55) /* No space left for appending the message length (8 Byte), */
    {            /* therefore store length into the following block          */
      swallow (stomach, ULBlock);
      memset (ULBlock, 0, sizeof (ULONG) << 4);
    }

  /* Append message length in bit */
  ULBlock[14] = total[0] << 3;
  ULBlock[15] = (total[0] >> 29) | (total[1] << 3);
  swallow (stomach, ULBlock);

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&ULBlock[0], &ULBlock[1], &ULBlock[2], &ULBlock[3]);
  Zero4Ulong (&ULBlock[4], &ULBlock[5], &ULBlock[6], &ULBlock[7]);
  Zero4Ulong (&ULBlock[8], &ULBlock[9], &ULBlock[10], &ULBlock[11]);
  Zero4Ulong (&ULBlock[12], &ULBlock[13], &ULBlock[14], &ULBlock[15]);
#endif

  return;
}


/******************************************************************************/
/* RIPEMD-128 kernel functions                                                */
/******************************************************************************/


static void
appetize128 (ULONG *stomach)
{
  stomach[0] = 0x67452301UL;
  stomach[1] = 0xefcdab89UL;
  stomach[2] = 0x98badcfeUL;
  stomach[3] = 0x10325476UL;

  return;
}


static void
swallow128 (ULONG *stomach, ULONG *ULBlock)
{
  int round, rol;
  ULONG x;

  ULONG a1 = stomach[0];
  ULONG b1 = stomach[1];
  ULONG c1 = stomach[2];
  ULONG d1 = stomach[3];

  ULONG a2 = stomach[0];
  ULONG b2 = stomach[1];
  ULONG c2 = stomach[2];
  ULONG d2 = stomach[3];

  /*lint -e123 Don't complain about "Macros ... defined with arguments" */

  /* Rounds and parallel rounds 0-15 */
  for (round = 0; round < 16; round++)
    {
      rol = s1[round];
      x = a1 + (b1 ^ c1 ^ d1) + ULBlock[round];
      CHAIN128 (a1, b1, c1, d1, x, rol);

      rol = s2[round];
      x = a2 + (c2 ^ (d2 & (b2 ^ c2))) + ULBlock[r2[round]] + 0x50a28be6UL;
      CHAIN128 (a2, b2, c2, d2, x, rol);
    }

  /* Rounds and parallel rounds 16-31 */
  for (round = 16; round < 32; round++)
    {
      rol = s1[round];
      x = a1 + (d1 ^ (b1 & (c1 ^ d1))) + ULBlock[r1[round]] + 0x5A827999UL;
      CHAIN128 (a1, b1, c1, d1, x, rol);

      rol = s2[round];
      x = a2 + ((b2 | ~c2) ^ d2) + ULBlock[r2[round]] + 0x5c4dd124UL;
      CHAIN128 (a2, b2, c2, d2, x, rol);
    }

  /* Rounds and parallel rounds 32-47 */
  for (round = 32; round < 48; round++)
    {
      rol = s1[round];
      x = a1 + ((b1 | ~c1) ^ d1) + ULBlock[r1[round]] + 0x6ed9eba1UL;
      CHAIN128 (a1, b1, c1, d1, x, rol);

      rol = s2[round];
      x = a2 + (d2 ^ (b2 & (c2 ^ d2))) + ULBlock[r2[round]] + 0x6d703ef3UL;
      CHAIN128 (a2, b2, c2, d2, x, rol);
    }

  /* Rounds and parallel rounds 48-63 */
  for (round = 48; round < 64; round++)
    {
      rol = s1[round];
      x = a1 + (c1 ^ (d1 & (b1 ^ c1))) + ULBlock[r1[round]] + 0x8f1bbcdcUL;
      CHAIN128 (a1, b1, c1, d1, x, rol);

      rol = s2[round];
      x = a2 + (b2 ^ c2 ^ d2) + ULBlock[r2[round]];
      CHAIN128 (a2, b2, c2, d2, x, rol);
    }


  /* Result in stomach */
  d2 += c1 + stomach[1];
  stomach[1] = stomach[2] + d1 + a2;
  stomach[2] = stomach[3] + a1 + b2;
  stomach[3] = stomach[0] + b1 + c2;
  stomach[0] = d2;

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&a1, &b1, &c1, &d1);
  Zero4Ulong (&a2, &b2, &c2, &d2);
  ZeroUlong (&x);
#endif  

  return;
}


static void
digest128 (ULONG *stomach, UCHAR *clear, ULONG total[])
{
  ULONG i,j, rest;
  ULONG ULBlock[16];

  memset (ULBlock, 0, sizeof (ULONG) << 4);

  /* Padding as for RIPEMD-160 */

  /* Message length modulo 64 ULONG-blocks (512 bit) */
  rest = total[0] & 0x3f;

  /* Insert ULONGs into ULBlock */
  for (i = 0; i < (rest >> 2); i++)
    {
      ULBlock[i] = UC2UL (clear);
      clear += 4;
    }

  /* Remaining UCHARs go into ULBLock. Invariant: 0 <= i <= 15 */
  for (j = i << 2; j < rest; j++)
    {
      ULBlock[i] |= (ULONG)*clear++ << ((j & 3) << 3);
    }

  /* Append 0x80 to ULBlock: At least one byte is still free */
  ULBlock[i] |= (ULONG)0x80 << ((j & 3) << 3);

  if (rest > 55) /* No space left for appending the message length (8 Byte), */
    {            /* therefore store length into the following block          */
      swallow128 (stomach, ULBlock);
      memset (ULBlock, 0, sizeof (ULONG) << 4);
    }

  /* Append message length in bit */
  ULBlock[14] = total[0] << 3;
  ULBlock[15] = (total[0] >> 29) | (total[1] << 3);
  swallow128 (stomach, ULBlock);

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&ULBlock[0], &ULBlock[1], &ULBlock[2], &ULBlock[3]);
  Zero4Ulong (&ULBlock[4], &ULBlock[5], &ULBlock[6], &ULBlock[7]);
  Zero4Ulong (&ULBlock[8], &ULBlock[9], &ULBlock[10], &ULBlock[11]);
  Zero4Ulong (&ULBlock[12], &ULBlock[13], &ULBlock[14], &ULBlock[15]);
#endif

  return;
}


#ifdef FLINT_SECURE

/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of variables                                          */
/*  Syntax:     ZeroUlong (ULONG *a);                                         */
/*  Input:      ULONG *a (Pointer to ULONG variable to be purged)             */
/*  Output:     *a overwritten by 0                                           */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
static inline void 
ZeroUlong (ULONG *a)
{
  *a = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of variables                                          */
/*  Syntax:     Zero2Ulong (ULONG *a, ULONG *b);                              */
/*  Input:      ULONG *a (Pointer to ULONG variable to be purged)             */
/*              ULONG *b (Pointer to ULONG variable to be purged)             */
/*  Output:     *a, *b overwritten by 0                                       */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
static inline void 
Zero2Ulong (ULONG *a, ULONG *b)
{
  *a = *b = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of variables                                          */
/*  Syntax:     Zero4Ulong (ULONG *a, ULONG *b, ULONG *c, ULONG *d);          */
/*  Input:      ULONG *a (Pointer to ULONG variable to be purged)             */
/*              ULONG *b (Pointer to ULONG variable to be purged)             */
/*              ULONG *c (Pointer to ULONG variable to be purged)             */
/*              ULONG *d (Pointer to ULONG variable to be purged)             */
/*  Output:     *a, *b, *c and *d overwritten by 0                            */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
static inline void 
Zero4Ulong (ULONG *a, ULONG *b, ULONG *c, ULONG *d)
{
  *a = *b = *c = *d = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of Array                                              */
/*  Syntax:     ZeroUcharArray (UCHAR *a, int Len);                           */
/*  Input:      UCHAR *a (Pointer to array of UCHARs)                         */
/*              int Len (Length of array in byte)                             */
/*  Output:     Array overwritten by 0                                        */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
static inline void 
ZeroUcharArray (void *a, size_t Len)
{
  memset ((UCHAR*)a, 0, Len);
}

#endif












