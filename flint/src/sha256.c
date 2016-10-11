/******************************************************************************/
/*                                                                            */
/*  Functions for arithmetic and number theory with large integers in C       */
/*  Software supplement to the book "Cryptography in C and C++"               */
/*  by Michael Welschenbach                                                   */
/*                                                                            */
/*  Module sha256.c         Revision: 03.06.2004                              */
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
#include "sha256.h"


#ifndef inline
#if (_MSC_VER >= 1100) || defined __GNUC__
#define inline __inline
#else 
#define inline /**/
#endif
#endif

/* Prototypes */

static void sha1_appetize (ULONG *);
static void sha1_swallow (ULONG *, ULONG *);
static void sha1_digest (ULONG *, UCHAR *, ULONG[]);

static void sha256_appetize (ULONG *);
static void sha256_swallow (ULONG *, ULONG *);
static void sha256_digest (ULONG *, UCHAR *, ULONG[]);

#ifdef FLINT_SECURE
static inline void ZeroUlong (ULONG *);
static inline void Zero2Ulong (ULONG *, ULONG *);
static inline void Zero4Ulong (ULONG *, ULONG *, ULONG *, ULONG *);
static inline void ZeroUcharArray (void *, size_t);
#endif

/* Macros */

/* Right shift*/
#define SHR(X, N)   ((X) >> (N)) 

/* Rotation, simulating assembler instructions for barrel shifting */
#define ROL(X, N)   (((X) << (N)) | ((X) >> (32-(N))))
#define ROR(X, N)  (((X) >> (N)) | ((X) << (32-(N))))

/* SHA-1 specific expansion of input values */
#define XPND(W, i) (W[i & 15] = \
  ROL((W[ i & 15] ^ W[(i - 14) & 15] ^ W[(i - 8) & 15] ^ W[(i - 3) & 15]), 1))

/* SHA-1 specific chaining of intermediate values in rounds */
#define CHAIN(A, B, C, D, E, X) { \
  (E) = (D);\
  (D) = (C);\
  (C) = ROL ((B), 30);\
  (B) = (A);\
  (A) = (X);\
}

/* SHA-256 specific macros */
#define Ch(X, Y, Z)  ((Z)^((X)&((Y)^(Z))))

#define Maj(X, Y, Z) (((X)&(Y))|((Z)&((X)|(Y))))

#define SUM256_0(X)  (ROR((X),  2) ^ ROR((X), 13) ^ ROR((X), 22)) 

#define SUM256_1(X)  (ROR((X),  6) ^ ROR((X), 11) ^ ROR((X), 25)) 

#define SIG256_0(X)  (ROR((X),  7) ^ ROR((X), 18) ^ SHR((X),  3)) 

#define SIG256_1(X)  (ROR((X), 17) ^ ROR((X), 19) ^ SHR((X), 10)) 

/* Conversion of an array of type UCHAR into a value of type ULONG */
/* Conversion is defined such that byte-arrays are interpreted */
/* as sequences of 4-byte-words stored in Big-Endian representation */
/* Thus conversion macro UC2ULBE takes value of lowest memory address */
/* of each 4-byte-group as most significant digit of an ULONG word. */
#define UC2ULBE(ucptr) \
  (((unsigned long) *((ucptr)+3)      ) | \
   ((unsigned long) *((ucptr)+2) <<  8) | \
   ((unsigned long) *((ucptr)+1) << 16) | \
   ((unsigned long) *(ucptr)     << 24))


/* Addition of a single-digit number b to a double-digit number represented */
/* by an array (a[1],a[0]). A carry in a[0]+=b is handled by increasing a[1]. */
#define ADDC(a, b) { \
  if (((a)[0] + (b)) < (a)[0]) {(a)[1]++;} \
  (a)[0] += (b); \
}

/* SHA-256 specific constants */
static ULONG K256[]=
  {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
   0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
   0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
   0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
   0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
   0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
   0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
   0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
   0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/*****************************************************************************/
/* SHA-1 API                                                                 */
/*****************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message of length byte in a single step          */
/*  Syntax:     void sha1_l (UCHAR *HashRes, UCHAR *clear, ULONG length);     */
/*  Input:      UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block)                        */
/*  Output:     UCHAR *HashRes (Hash Value)                                   */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sha1_l (UCHAR *HashRes, UCHAR *clear, ULONG length)
{
  SHASTAT hws;

  sha1init (&hws);
  sha1finish (HashRes, &hws, clear, length);

#ifdef FLINT_SECURE
  ZeroUcharArray (&hws, sizeof (hws));
#endif
}


/******************************************************************************/
/* Functions for blockwise hashing a message in several steps                 */
/* Procedure: Initialization with sha1init_l                                  */
/*            Hashing of block1,block2,...,blockn with function sha1hash_l    */
/*            Finish operation with function sha1finish_l                     */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Initialization of SHA-1 function                              */
/*  Syntax:     void sha1init_l (SHASTAT *hws);                               */
/*  Input:      SHASTAT *hws (SHA-1 status buffer)                            */
/*  Output:     -                                                             */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sha1init_l (SHASTAT *hws)
{
  sha1_appetize (hws->stomach);
  hws->total[0] = 0;
  hws->total[1] = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message block                                    */
/*  Syntax:     int sha1hash_1 (SHASTAT *hws, UCHAR *clear, ULONG length);    */
/*  Input:      SHASTAT *hws (SHA-1 status buffer)                            */
/*              UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block i bytes = 0 mod 64)     */
/*  Output:     -                                                             */
/*  Returns:    E_CLINT_OK if everything is O.K.                              */
/*              E_CLINT_SHA if length != 0 mod 64                             */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
sha1hash_l (SHASTAT *hws, UCHAR *clear, ULONG length)
{
  ULONG ULBlock[16];    /* message block holding 16 values of type ULONG  */
  ULONG noofblocks;     /* Number of blocks of 16 ULONG-vyluse            */
  ULONG i, j;           /* Counter                                        */

  /* If incomplete 64 byte block exists... */
  if (length & 63)
    {
      return E_CLINT_SHA;  /* ...return error code  */       
    }

  /* Number of 64 byte message blocks in clear */
  noofblocks = length >> 6;

  /* Process 64 byte message blocks in clear */
  for (i = 0; i < noofblocks; i++)
    {
        for (j = 0; j < 16; j++)
          {
            ULBlock[j] = UC2ULBE (clear);
            clear += 4;
          }

        sha1_swallow (hws->stomach, ULBlock);
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
/*  Function:   Finish hash function SHA-1                                    */
/*  Syntax:     void sha1finish_l (UCHAR *HashRes, SHASTAT *hws,              */
/*                                 UCHAR *clear, ULONG length);               */
/*  Input:      SHASTAT *hws (SHA-1 status buffer)                            */
/*              UCHAR *clear (Pointer to the last message block               */
/*              ULONG length (Length of message block in bytes)               */
/*  Output:     UCHAR HashRes (Hash value)                                    */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sha1finish_l (UCHAR *HashRes, SHASTAT *hws, UCHAR *clear, ULONG length)
{
  unsigned i;

  /* Number of bytes in complete blocks */
  ULONG blength = (length >> 6) << 6;

  /* Process complete blocks in clear */
  sha1hash_l (hws, clear, blength);

  /* Add length of message in clear to hws->total */
  ADDC (hws->total,length - blength);

  /* Process last incomplete block with padding and length of message */
  sha1_digest (hws->stomach, clear + blength, hws->total);

  /* Note: Hash result is stored in Big-Endian representation. */
  for (i = 0; i < 20; i += 4)
    {
      *(HashRes+i)   = (UCHAR)(hws->stomach[i>>2] >> 24); /*lint !e661 !e662 */
      *(HashRes+i+1) = (UCHAR)(hws->stomach[i>>2] >> 16); /*lint !e661 !e662 */
      *(HashRes+i+2) = (UCHAR)(hws->stomach[i>>2] >>  8); /*lint !e661 !e662 */
      *(HashRes+i+3) = (UCHAR)(hws->stomach[i>>2]      );                      
    }
 
  return;
}


/*****************************************************************************/
/* SHA-256 API                                                               */
/*****************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message of length byte in a single step          */
/*  Syntax:     void sha1_l (UCHAR *HashRes, UCHAR *clear, ULONG length);     */
/*  Input:      UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block)                        */
/*  Output:     UCHAR *HashRes (Hash Value)                                   */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sha256_l (UCHAR *HashRes, UCHAR *clear, ULONG length)
{
  SHA256STAT hws;

  sha256init_l (&hws);
  sha256finish_l (HashRes, &hws, clear, length);

#ifdef FLINT_SECURE
  ZeroUcharArray (&hws, sizeof (hws));
#endif
}


/******************************************************************************/
/* Functions for blockwise hashing a message in several steps                 */
/* Procedure: Initialization with sha256init_l                                */
/*            Hashing of block1,block2,...,blockn with function sha256hash_l  */
/*            Finish operation with function sha256finish_l                   */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Function:   Initialization of SHA-256 function                            */
/*  Syntax:     void sha256init_l (SHASTAT *hws);                             */
/*  Input:      SHA256STAT *hws (SHA-256 status buffer)                       */
/*  Output:     -                                                             */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sha256init_l (SHA256STAT *hws)
{
  sha256_appetize (hws->stomach);
  hws->total[0] = 0;
  hws->total[1] = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Hashing of a message block                                    */
/*  Syntax:     int sha256hash_1 (SHA256STAT *hws, UCHAR *clear,              */
/*                                                   ULONG length);           */
/*  Input:      SHA256STAT *hws (SHA-256 status buffer)                       */
/*              UCHAR *clear (Pointer to message block)                       */
/*              ULONG length (Length of message block i bytes = 0 mod 64)     */
/*  Output:     -                                                             */
/*  Returns:    E_CLINT_OK if everything is O.K.                              */
/*              E_CLINT_SHA if length != 0 mod 64                             */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
sha256hash_l (SHA256STAT *hws, UCHAR *clear, ULONG length)
{
  ULONG ULBlock[16];    /* message block holding 16 values of type ULONG  */
  ULONG noofblocks;     /* Number of blocks of 16 ULONG-vyluse            */
  ULONG i, j;           /* Counter                                        */

  /* If incomplete 64 byte block exists... */
  if (length & 63)
    {
      return E_CLINT_SHA;  /* ...return error code  */
    }

  /* Number of 64 byte message blocks in clear */
  noofblocks = length >> 6;

  /* Process 64 byte message blocks in clear */
  for (i = 0; i < noofblocks; i++)
    {
        for (j = 0; j < 16; j++)
          {
            ULBlock[j] = UC2ULBE (clear);
            clear += 4;
          }

        sha256_swallow (hws->stomach, ULBlock);
    }

  /* Verarbeite 64-Byte-Bloecke von clear */
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
/*  Function:   Finish hash function SHA-256                                  */
/*  Syntax:     void sha256finish_l (UCHAR *HashRes, SHA256STAT *hws,         */
/*                                          UCHAR *clear, ULONG length);      */
/*  Input:      SHA256STAT *hws (SHA-256 status buffer)                       */
/*              UCHAR *clear (Pointer to the last message block               */
/*              ULONG length (Length of message block in bytes)               */
/*  Output:     UCHAR HashRes (Hash value)                                    */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
sha256finish_l (UCHAR *HashRes, SHA256STAT *hws, UCHAR *clear, ULONG length)
{
  unsigned i;

  /* Number of bytes in complete blocks */
  ULONG blength = (length >> 6) << 6;

  /* Process complete blocks in clear */
  sha256hash_l (hws, clear, blength);

  /* Add length of message in clear to hws->total */
  ADDC (hws->total,length - blength);

  /* Process last incomplete block with padding and length of message */
  sha256_digest (hws->stomach, clear + blength, hws->total);

  /* Note: Hash result is stored in Big-Endian representation. */
  for (i = 0; i < 32; i += 4)
    {
      *(HashRes+i)   = (UCHAR)(hws->stomach[i>>2] >> 24); /*lint !e661 !e662 */
      *(HashRes+i+1) = (UCHAR)(hws->stomach[i>>2] >> 16); /*lint !e661 !e662 */
      *(HashRes+i+2) = (UCHAR)(hws->stomach[i>>2] >>  8); /*lint !e661 !e662 */
      *(HashRes+i+3) = (UCHAR)(hws->stomach[i>>2]      );                      
    }
 
  return;
}


/******************************************************************************/
/* SHA-1 kernel functions                                                     */
/******************************************************************************/

static void
sha1_appetize (ULONG *stomach)
{
  stomach[0] = 0x67452301UL;
  stomach[1] = 0xefcdab89UL;
  stomach[2] = 0x98badcfeUL;
  stomach[3] = 0x10325476UL;
  stomach[4] = 0xc3d2e1f0UL;

  return;
}


static void
sha1_swallow (ULONG *stomach, ULONG *ULBlock)
{
  int round;
  ULONG x;

  ULONG a = stomach[0];
  ULONG b = stomach[1];
  ULONG c = stomach[2];
  ULONG d = stomach[3];
  ULONG e = stomach[4];

  /*lint -e123 Don't complain about "Macros ... defined with arguments" */

  for (round = 0; round < 16; round++)
    {
      x = ROL((a), 5) + (d^(b&(c^d))) + e + ULBlock[round] + 0x5A827999L;
      CHAIN (a, b, c, d, e, x);
    }

  for (round = 16; round < 20; round++)
    {
      x = ROL((a), 5) + (d^(b&(c^d))) + e + XPND(ULBlock, round) + 0x5A827999L;
      CHAIN (a, b, c, d, e, x);
    }

  for (round = 20; round < 40; round++)
    {
      x = ROL((a), 5) + (b^c^d) + e + XPND(ULBlock, round) + 0x6ED9EBA1L;
      CHAIN (a, b, c, d, e, x);
    }


  for (round = 40; round < 60; round++)
    {
      x = ROL((a), 5) + ((b&c)|(d&(b|c))) + e + XPND(ULBlock, round) +  0x8F1BBCDCL;
      CHAIN (a, b, c, d, e, x);
    }

  for (round = 60; round < 80; round++)
    {
      x = ROL((a), 5) + (b^c^d) + e + XPND(ULBlock, round) +  0xCA62C1D6L;
      CHAIN (a, b, c, d, e, x);
    }

  /* Result in stomach */
  stomach[0] += a;
  stomach[1] += b;
  stomach[2] += c;
  stomach[3] += d;
  stomach[4] += e;

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&a, &b, &c, &d);
  ZeroUlong (&e);
#endif  

  return;
}


static void
sha1_digest (ULONG *stomach, UCHAR *clear, ULONG total[])
{
  ULONG i,j, k, rest;
  ULONG ULBlock[16];

  memset (ULBlock, 0, sizeof (ULONG) << 4);

  /* Padding to achieve block lengthe of 512 Bit                      */
  /* by example of the message "abc" of length l = 24 bit:            */
  /* 1. One bit "1" is appended to the end of the masseage.           */
  /* 2. k "0"-bits are appended, with k the smallest non-negative     */
  /*    solution of l+1+k = 448 mod 512, in this example k = 448-1-24 */
  /*    = 423, 7 bit following the single "1" plus additional         */
  /*    13 ULONGs (i.e. 416 bit) in the positions ULBlock[1]...[13].  */
  /* 3. The length of the message is appended in 64 bit (2 ULONGs).   */
  /*    Note: Opposite to MD4, MD5 and RIPEMD MSW is stored first and */
  /*    LSW is preceding MSW.                                         */
  /*                                                                  */
  /*    01100001 01100010 01100011 1 000...00 00...011000             */
  /*       "a"      "b"      "c"   1+ 423Bit   64 bit (length)        */

  /* Message length modulo 64 ULONG-blocks (512 bit) */
  rest = total[0] & 0x3f;

  /* Insert ULONGs into ULBlock */
  for (i = 0; i < (rest >> 2); i++)
    {
      ULBlock[i] = UC2ULBE (clear);
      clear += 4;
    }

  /* Remaining UCHARs go into ULBLock. Invariant: 0 <= i <= 15 */
  for (j = i << 2, k = 3; j < rest; j++, k--)
    {
      ULBlock[i] |= (ULONG)*clear++ << (k << 3);
    }

  /* Append 0x80 to ULBlock: At least one byte is still free */
  ULBlock[i] |= (ULONG)0x80 << (k << 3);

  if (rest > 55) /* No space left for appending the message length (8 Byte), */
    {            /* therefore store length into the following block          */
      sha1_swallow (stomach, ULBlock);
      memset (ULBlock, 0, sizeof (ULONG) << 4);
    }

  /* Append message length in bit, MSW first, LSW preceding MSW */
  ULBlock[14] = (total[0] >> 29) | (total[1] << 3);
  ULBlock[15] = total[0] << 3;

  sha1_swallow (stomach, ULBlock);

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
/* SHA-256 kernel functions                                                   */
/******************************************************************************/

static void
sha256_appetize (ULONG *stomach)
{
  stomach[0] = 0x6a09e667UL;
  stomach[1] = 0xbb67ae85UL;
  stomach[2] = 0x3c6ef372UL;
  stomach[3] = 0xa54ff53aUL;
  stomach[4] = 0x510e527fUL;
  stomach[5] = 0x9b05688cUL;
  stomach[6] = 0x1f83d9abUL;
  stomach[7] = 0x5be0cd19UL;
  return;
}


static void
sha256_swallow (ULONG *stomach, ULONG *ULBlock)
{
  ULONG WW[64];
  int round;
  ULONG T1, T2;

  ULONG a = stomach[0];
  ULONG b = stomach[1];
  ULONG c = stomach[2];
  ULONG d = stomach[3];
  ULONG e = stomach[4];
  ULONG f = stomach[5];
  ULONG g = stomach[6];
  ULONG h = stomach[7];

  /*lint -e123 Don't complain about "Macros ... defined with arguments" */

  for (round = 0; round < 16; round++)
    {
      WW[round] = *(ULBlock + round);
    }
  
  for (round = 16; round < 64; round++)
    {
      WW[round] = SIG256_1(WW[round -  2]) + WW[round -  7] + 
                  SIG256_0(WW[round - 15]) + WW[round - 16];
    }
  
  for (round = 0; round < 64; round++)
    {
      T1 = h + SUM256_1(e) + Ch(e, f, g) + K256[round] + WW[round];
      T2 = SUM256_0(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
    }

  /* Result in stomach */
  stomach[0] += a;
  stomach[1] += b;
  stomach[2] += c;
  stomach[3] += d;
  stomach[4] += e;
  stomach[5] += f;
  stomach[6] += g;
  stomach[7] += h;

#ifdef FLINT_SECURE
  /* Overwrite temporary variables */
  Zero4Ulong (&a, &b, &c, &d);
  Zero4Ulong (&e, &f, &g, &h);
#endif  

  return;
}


static void
sha256_digest (ULONG *stomach, UCHAR *clear, ULONG total[])
{
  ULONG i,j, k, rest;
  ULONG ULBlock[16];

  memset (ULBlock, 0, sizeof (ULONG) << 4);

  /* Padding to achieve block lengthe of 512 Bit is exactly as in SHA-1 */

  /* Message length modulo 64 ULONG-blocks (512 bit) */
  rest = total[0] & 0x3f;

  /* Insert ULONGs into ULBlock */
  for (i = 0; i < (rest >> 2); i++)
    {
      ULBlock[i] = UC2ULBE (clear);
      clear += 4;
    }

  /* Remaining UCHARs go into ULBLock. Invariant: 0 <= i <= 15 */
  for (j = i << 2, k = 3; j < rest; j++, k--)
    {
      ULBlock[i] |= (ULONG)*clear++ << (k << 3);
    }

  /* Append 10000000 = 0x80 to ULBlock: At least one byte is still free */
  ULBlock[i] |= (ULONG)0x80 << (k << 3);

  if (rest > 55) /* No space left for appending the message length (8 Byte), */
    {            /* therefore store length into the following block          */
      sha256_swallow (stomach, ULBlock);
      memset (ULBlock, 0, sizeof (ULONG) << 4);
    }

  /* Append message length in bit, MSW first, LSW preceding MSW */
  ULBlock[14] = (total[0] >> 29) | (total[1] << 3);
  ULBlock[15] = total[0] << 3;

  sha256_swallow (stomach, ULBlock);

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

