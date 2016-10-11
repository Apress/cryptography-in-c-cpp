/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach, published by Apress Berkeley CA, 2005             */
/*                                                                            */
/* Module sha1.c           Revision: 01.12.2002                               */
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
#include "sha1.h"


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

#ifdef FLINT_SECURE
static inline void ZeroUlong (ULONG *);
static inline void Zero2Ulong (ULONG *, ULONG *);
static inline void Zero4Ulong (ULONG *, ULONG *, ULONG *, ULONG *);
static inline void ZeroUcharArray (void *, size_t);
#endif

/* Macros */

/* Left rotation, simulation of assembler instruction for barrel shifting */
#define ROL(X, N)   (((X) << (N)) | ((X) >> (32-(N))))

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
/* Procedure: Initialization with ripeinit                                    */
/*            Hashing of block1, block2,... ,blockn with function sha1hash    */
/*            Finish operation with function sha1finish                       */
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



#ifdef SHA1_TEST

unsigned char digest[20];
unsigned char message[3] = {'a', 'b', 'c' };
unsigned char *mess56 = 
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

/* Correct solutions from FIPS PUB 180-1 */
char *dig1 = "A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D";
char *dig2 = "84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1";
char *dig3 = "34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F";

/* Output should look like:-
 a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d
 A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D <= correct
 84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1
 84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1 <= correct
 34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f
 34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F <= correct
*/

main()
{
  SHASTAT sha;
  int i;
  UCHAR big[1000];

  sha1init (&sha);
  sha1finish (digest, &sha, message, 3);

  for (i = 0; i < 20; i++)
    {
      if ((i % 4) == 0) printf(" ");
        printf("%02x", digest[i]);
    }
  printf ("\n");
  printf (" %s <= correct\n", dig1);

  sha1init (&sha);
  sha1finish (digest, &sha, mess56, 56);

  for (i = 0; i < 20; i++)
    {
      if ((i % 4) == 0) printf(" ");
        printf ("%02x", digest[i]);
    }
  printf ("\n");
  printf (" %s <= correct\n", dig2);

  /* Fill up big array */
  for (i = 0; i < 1000; i++)
    big[i] = 'a';

  sha1init (&sha);
  /* Digest 1 million x 'a' */
  for (i = 0; i < 1041; i++)
    sha1hash (&sha, big, 960);
  sha1finish (digest, &sha, big, 640);

  for (i = 0; i < 20; i++)
    {
      if ((i % 4) == 0) printf(" ");
      printf("%02x", digest[i]);
    }
  printf("\n");
  printf(" %s <= correct\n", dig3);

  return 0;
}

#endif /* SHA1_TEST? */
