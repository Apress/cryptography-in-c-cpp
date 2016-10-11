/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module entropy.c        Revision: 01.07.2002                               */
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

#if defined _WIN32 && defined _MSC_VER
#include <windows.h>
#include <winbase.h>
#include <wincrypt.h>
#endif /* _WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "ripemd.h"
#include "flint.h"
#include "entropy.h"

static void * local_memset (void *ptr, int val, size_t len);


/******************************************************************************/
/*                                                                            */
/*  Function:   Generation of entropy for initialization of Pseudo-           */
/*              randomnumber generators. In addition to a user defined byte   */
/*              vector (optional) entropy bytes are read from system specific */
/*              sources:                                                      */
/*               - For Win32: Value from QueryPerformanceCounter (64 bytes),  */
/*                 Values from CryptGenRandom                                 */
/*               - For Unix/Linux systems:                                    */
/*                 Entropy is read from virtual device /dev/urandom           */
/*                 if this source is available.                               */
/*              Altogether a number of LenRndStr+AddEntropy bytes are fed     */
/*              into the result, which is returned as CLINT-value and in      */
/*              addition as Ripemd-160 hashvalue.                             */
/*                                                                            */
/*              Usage with Windows 9x/ME/NT/2K/XP:                            */
/*              -----------------------------------------                     */
/*              Usage of Win32-function GenRandom() requires linking with     */
/*              advapi32.lib and accessibility of DLL advapi32.dll at runtime */
/*              (available under Windows 95 OSR2 and higher, Windows NT 4.x   */
/*              Windows 2000, XP).                                            */
/*                                                                            */
/*  Syntax:     GetEntropy_l (CLINT Seed_l, char *Hashres, int AddEntropy,    */
/*                                              char *RndStr, int LenRndStr); */
/*  Input:      AddEntropy (Number of entropy btes to be generated)           */
/*              RndStr (User defined byte vector, NULL-pointer accepted)      */
/*              LenRndStr (Laenge von RndStr in Bytes)                        */
/*  Output:     Seed_l (Entropie as CLINT-value. If Seed_l == NULL-pointer    */
/*                      no output is given)                                   */
/*              Hashres (Entropy as Ripemd-160-hashvalue, length of 20 bytes  */
/*                       If Hashres == NULL-pointer no Output is given)       */
/*  Returns:    0 if everything is OK                                         */
/*              n > 0 if n bytes less than the requested number of entropy    */
/*                    bytes could be generated                                */
/*              E_CLINT_MAL error in allocation of memory                     */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
GetEntropy_l (CLINT Seed_l, UCHAR *Hashres, int AddEntropy, char *RndStr, int LenRndStr)
{
  unsigned nextfree = 0, MissingEntropy = MAX(AddEntropy, sizeof (time_t));
  unsigned i, j;
  UCHAR *Seedbytes;
  int BytesRead;
  int LenSeedbytes = LenRndStr + MissingEntropy + 
                     sizeof (time_t) + 2*sizeof (ULONG);
  RMDSTAT hws;
  time_t SeedTime;
  FILE *fp;

#if defined _WIN32 && defined _MSC_VER
  LARGE_INTEGER PCountBuff;
  HCRYPTPROV hProvider = 0;
#endif /*defined _WIN32 && defined _MSC_VER? */

  if ((Seedbytes = (UCHAR*)malloc(LenSeedbytes)) == NULL)
    {
      return E_CLINT_MAL;
    }

  if (RndStr != NULL && LenRndStr > 0)
    {
      memcpy (Seedbytes, RndStr, LenRndStr);

#ifdef FLINT_TEST
      printf("\nRndStr in Seedbytes = ");
        for (i = 0 ; i < LenRndStr; i++) 
      printf("%.2x ",(unsigned char)Seedbytes[nextfree+i]);
#endif /* FLINT_TEST */

      nextfree = LenRndStr;
    }
       
  SeedTime = (time_t)time(NULL);

  /* System time in Seed */
  for (i = 0; i < sizeof(time_t); i++) 
    {
      j = i << 3;
      /* right shift of signed SeedTime doesn't matter, turn lint off */
      Seedbytes[nextfree+i] = (UCHAR)((SeedTime >> j) & (time_t)0xff); /*lint !e704, !e713 */
    }

#ifdef FLINT_TEST
  printf("\nWert Time in Seedbytes = ");
  for (i = 0 ; i < sizeof (time_t); i++) 
    printf("%.2x ",(unsigned char)Seedbytes[nextfree+i]);
#endif /* FLINT_TEST */

  nextfree += sizeof (time_t);
  MissingEntropy -= sizeof (time_t);

#if defined _WIN32 && defined _MSC_VER   /* Get Win32-generated Entropy */
  if (MissingEntropy)
    {
  /* Chaining with 64-bit-value QueryPerformanceCounter() (Win32-Function) */
      QueryPerformanceCounter (&PCountBuff);
      for (i = 0; i < sizeof (DWORD); i++)
        {
          j = i << 3;
          Seedbytes[nextfree + i] = 
            (char)((PCountBuff.HighPart >> j) & (DWORD)0xff); 
          Seedbytes[nextfree + sizeof (DWORD) + i] = 
            (char)((PCountBuff.LowPart >> j) & (DWORD)0xff); 
        }

#ifdef FLINT_TEST
      printf("\nWert PerformanceCounter in Seedbytes = ");
      for (i = 0; i < 2*sizeof (DWORD); i++) 
        printf("%.2x ",(unsigned char)Seedbytes[nextfree+i]);
#endif

      nextfree += 2*sizeof (DWORD);
      MissingEntropy -= 2*sizeof (DWORD);
    }

  /* Chaining with output from CryptGenRandom() (Win32-Function) */
  if (CryptAcquireContext(&hProvider, 
      NULL, 
      NULL, 
      PROV_RSA_FULL, 
      CRYPT_VERIFYCONTEXT))
    {
      if (CryptGenRandom (hProvider, 
                          MissingEntropy, 
                          &Seedbytes[nextfree]))
        {
#ifdef FLINT_TEST
          printf("\nWert CryptGenRandom in Seedbytes = ");
          for (i = 0 ; i < MissingEntropy; i++) 
            printf("%.2x ",(unsigned char)Seedbytes[nextfree+i]);
          printf("\n");
#endif /* FLINT_TEST */

          nextfree += MissingEntropy;
          MissingEntropy = 0;
        }
    }

  if (hProvider)
    {
      CryptReleaseContext (hProvider, 0);
    }
#endif  /* defined _WIN32 && _MSC_VER */

/* Get entropy from /dev/urandom if available */
  if ((fp = fopen("/dev/urandom", "r")) != NULL) 
    {
      BytesRead = fread(&Seedbytes[nextfree], sizeof (UCHAR), MissingEntropy, fp);

#ifdef FLINT_TEST
      printf("\nWert aus /dev/urandom in Seedbytes = ");
      for (i = 0; i < BytesRead; i++) 
        printf("%.2x ",(unsigned char)Seedbytes[nextfree+i]);
      printf("\n");
#endif /* FLINT_TEST */

      nextfree += BytesRead;
      MissingEntropy -= BytesRead;
      fclose (fp);
    }

  /* Hashing of chained entropy-values */
  if (Hashres != NULL)
    {
      ripeinit (&hws);
      ripefinish (Hashres, &hws, Seedbytes, nextfree);

#ifdef FLINT_TEST
      printf("\nHashwert in Hashres = ");
      for (i = 0; i < 20; i++) printf("%.2x ", (unsigned char)Hashres[i]);
      printf("\n");
#endif /* FLINT_TEST */
    }

  /* Seed as CLINT-values */
  if (Seed_l != NULL)
    {
      byte2clint_l (Seed_l, Seedbytes, nextfree);
#ifdef FLINT_TEST
      disp_l ("Random Seed in Seed_l = ", Seed_l);
#endif /* FLINT_TEST */
    }

  /* Security: Purge Seed by overwriting with 0 */
  SeedTime = 0;
  local_memset (Seedbytes, 0, LenSeedbytes);
  local_memset (&hws, 0, sizeof (hws));

  free (Seedbytes);

  return MissingEntropy;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Calculation of the entropy of a password from the             */
/*              number of characters of the underlying alphabet and number    */
/*              of characters of the password itself, using the formular      */
/*                        Entropie = log2 (n^m)                               */
/*              with n the size of the alphabet and m the size of the         */
/*              password in characters.                                       */
/*              This value is important for the calculation of a              */
/*              cryptographic key from a password. If all printable ASCII-    */
/*              characters can be used for a password (93 characters), then   */
/*              the password should have                                      */
/*               - 20 characters for generating a key of 128-bit length       */
/*               - 30 characters for generating a key of 192-bit length       */
/*               - 40 characters for generating a key of 256-bit length.      */
/*  Syntax:     int PWDQual_l (char *pwd, int len_pwd, int len_alpha);        */
/*  Input:      char *PWD (Password, RFU, not beeing evaluated here)          */
/*              int LenPWD (Length of the password in characters)             */
/*              int LenAlpha (Size of the alphabet in characters)             */
/*  Output:     -                                                             */
/*  Returns:    Entropy of password                                           */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
PWDQual_l (char *PWD, int LenPWD, int LenAlpha)
{
  return (int)((double)LenAlpha * log10 (LenPWD) / log10 (2));
}


/******************************************************************************/
/*                                                                            */
/* Local memset-Function, does the same as memset()                           */
/* Needed to make compiler purge variables in spite of optimization.          */
/*                                                                            */
/******************************************************************************/
static void *
local_memset (void *ptr, int val, size_t len)
{
    return memset (ptr, val, len);
}



