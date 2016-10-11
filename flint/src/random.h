/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module random.h         Revision: 10.07.2002                               */
/*                                                                            */
/*  Copyright (C) 1998-2005 by Michael Welschenbach                           */
/*  Copyright (C) 1998-2005 by Springer-Verlag Berlin, Heidelberg             */
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

#ifndef __RANDOMH__
#define __RANDOMH__

#include "entropy.h"
#include "ripemd.h"
#include "sha1.h"
#include "aes.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Macros for selecting a PRNG */

#define FLINT_RNDAES     1
#define FLINT_RNDRMDSHA1 2
#define FLINT_RNDBBS     3
#define FLINT_RND64      4


/* Macros defining error codes */

#define E_CLINT_RIN    -20
#define E_CLINT_RNG    -21
#define E_CLINT_RGE    -22
#define E_CLINT_RCP    -23


/* Macros maintaining FLINT/C namespace */

#define InitRand64           InitRand64_l
#define InitRandRMDSHA1      InitRandRMDSHA1_l
#define SwitchRandRMDSHA1    SwitchRandRMDSHA1_l
#define PurgeRandRMDSHA1     PurgeRandRMDSHA1_l
#define InitRandAES	     InitRandAES_l
#define SwitchRandAES	     SwitchRandAES_l    
#define PurgeRandAES	     PurgeRandAES_l  
#define InitRandBBS	     InitRandBBS_l   
#define SwitchRandBBS	     SwitchRandBBS_l    
#define bRandBBS	     bRandBBS_l  
#define sRandBBS	     sRandBBS_l       
#define lRandBBS	     lRandBBS_l       
#define PurgeRandBBS         PurgeRandBBS_l
#define ulrandBBS_l          lRandBBS_l
#define usrandBBS_l          sRandBBS_l
#define ucrandBBS_l          bRandBBS_l
#define randbit_l            SwitchRandBBS_l
#define randBBS_l            RandBBS_l
#define seedBBS_l            SeedBBS_l
#define initrandBBS_lt()     InitRandBBS_l("", 0, 0);


/* Macros that generate random numbers using RipeMD-160 SHA-1 based PRNG */

/* 8 bit unsigned */
#define bRandRMDSHA1(S)   ((UCHAR)SwitchRandRMDSHA1_l((S)))
#define bRandRMDSHA1_l(S) ((UCHAR)SwitchRandRMDSHA1_l((S)))

/* 16 bit unsigned */
#define sRandRMDSHA1(S)   (((USHORT)SwitchRandRMDSHA1_l((S)) << 8) | \
                           (USHORT)SwitchRandRMDSHA1_l((S)))
#define sRandRMDSHA1_l(S) (((USHORT)SwitchRandRMDSHA1_l((S)) << 8) | \
                           (USHORT)SwitchRandRMDSHA1_l((S)))
/* 32 bit unsigned */
#define lRandRMDSHA1(S)   (((ULONG)SwitchRandRMDSHA1_l((S)) << 24) | \
                           ((ULONG)SwitchRandRMDSHA1_l((S)) << 16) | \
                           ((ULONG)SwitchRandRMDSHA1_l((S)) <<  8) | \
                            (ULONG)SwitchRandRMDSHA1_l((S))      )    

#define lRandRMDSHA1_l(S) (((ULONG)SwitchRandRMDSHA1_l((S)) << 24) | \
                           ((ULONG)SwitchRandRMDSHA1_l((S)) << 16) | \
                           ((ULONG)SwitchRandRMDSHA1_l((S)) <<  8) | \
                            (ULONG)SwitchRandRMDSHA1_l((S))      )    


/* Macros that generate random numbers using AES based PRNG */

/* 8 bit unsigned */
#define bRandAES(S)       ((UCHAR)SwitchRandAES_l((S)))
#define bRandAES_l(S)     ((UCHAR)SwitchRandAES_l((S)))

/* 16 bit unsigned */
#define sRandAES(S)       (((USHORT)SwitchRandAES_l((S)) << 8) | \
                            (USHORT)SwitchRandAES_l((S)))
#define sRandAES_l(S)     (((USHORT)SwitchRandAES_l((S)) << 8) | \
                            (USHORT)SwitchRandAES_l((S)))
/* 32 bit unsigned */
#define lRandAES(S)       (((ULONG)SwitchRandAES_l((S)) << 24) | \
                           ((ULONG)SwitchRandAES_l((S)) << 16) | \
                           ((ULONG)SwitchRandAES_l((S)) <<  8) | \
                            (ULONG)SwitchRandAES_l((S))      )    

#define lRandAES_l(S)     (((ULONG)SwitchRandAES_l((S)) << 24) | \
                           ((ULONG)SwitchRandAES_l((S)) << 16) | \
                           ((ULONG)SwitchRandAES_l((S)) <<  8) | \
                            (ULONG)SwitchRandAES_l((S))      )    


struct InternalStateRMDSHA1
{
  UCHAR SRMDSHA1[20];     
  UCHAR XRMDSHA1[20];   
  unsigned RoundRMDSHA1;
  int RandRMDSHA1Init;
};

struct InternalStateAES
{
  AESBLOCK XAES;
  UCHAR RandAESKey[32];
  AESWORKSPACE RandAESWorksp;
  AESKEYSCHED RandAESSched;
  unsigned RoundAES;
  unsigned RandAESInit;
  unsigned UpdateKeyAES;
};

struct InternalStateBBS
{
  CLINT XBBS;           
  CLINT MODBBS;       
  int RandBBSInit;
};

typedef struct InternalStateRMDSHA1 STATERMDSHA1;
typedef struct InternalStateBBS STATEBBS;
typedef struct InternalStateAES STATEAES;

struct InternalStatePRNG
{
  STATERMDSHA1 StateRMDSHA1;
  STATEAES StateAES;
  STATEBBS StateBBS;
  int Generator;
};

typedef struct InternalStatePRNG STATEPRNG;


/* Prototypes for 64 bit linear congruential PRNG */

extern int __FLINT_API
InitRand64_l (char *UsrStr, int LenUsrStr, int AddEntropy);


/* Prototypes for RipeMD-160 SHA-1 based PRNG */

extern int __FLINT_API
InitRandRMDSHA1_l (STATERMDSHA1 *state, char *UsrStr, int LenUsrStr, int AddEntropy);

extern UCHAR __FLINT_API
SwitchRandRMDSHA1_l (STATERMDSHA1 *state);

extern int __FLINT_API
RandRMDSHA1_l (CLINT a_l, STATERMDSHA1 *state, int l);

extern void __FLINT_API
PurgeRandRMDSHA1_l (STATERMDSHA1 *state);


/* Prototypes for AES based PRNG */

extern int __FLINT_API
InitRandAES_l (STATEAES *StateAES, char *UsrStr, int LenUsrStr, int AddEntropy, int update);

extern UCHAR __FLINT_API
SwitchRandAES_l (STATEAES *StateAES);

extern int __FLINT_API
RandAES_l (CLINT a_l, STATEAES *StateAES, int l);

extern void __FLINT_API
PurgeRandAES_l (STATEAES *StateAES);


/* Prototypes for BBS PRNG */

extern int __FLINT_API
InitRandBBS_l (STATEBBS *state, char *UsrStr, int LenUsrStr, int AddEntropy);

extern int __FLINT_API  
SwitchRandBBS_l (STATEBBS *state);

extern int __FLINT_API  
SeedBBS_l (STATEBBS *, CLINT);

extern int __FLINT_API  
RandBBS_l (CLINT, STATEBBS * ws, int);

extern UCHAR __FLINT_API  
bRandBBS_l (STATEBBS *state);

extern USHORT __FLINT_API  
sRandBBS_l (STATEBBS *state);

extern ULONG __FLINT_API  
lRandBBS_l (STATEBBS *state);

extern void __FLINT_API
PurgeRandBBS_l (STATEBBS *state);


/* Prototypes for higher level random number functions w/ choice of */
/* FLINT/C random number generators */

int __FLINT_API
InitRand_l (STATEPRNG *, char *, int, int, int);

UCHAR __FLINT_API
bRand_l (STATEPRNG *);

USHORT __FLINT_API
sRand_l (STATEPRNG *);

ULONG __FLINT_API
lRand_l (STATEPRNG *);

int __FLINT_API
Rand_l (CLINT r_l, STATEPRNG *, int l);

int __FLINT_API
PurgeRand_l (STATEPRNG *);


extern int __FLINT_API  
FindPrime_l (CLINT, STATEPRNG *, USHORT);

extern int __FLINT_API  
FindPrimeGcd_l (CLINT, STATEPRNG *, USHORT, CLINT);

extern int __FLINT_API  
FindPrimeMinMaxGcd_l (CLINT, STATEPRNG *, CLINT, CLINT, CLINT);

extern int __FLINT_API  
RandMinMax_l (CLINT, STATEPRNG *, CLINT, CLINT);

#ifdef  __cplusplus
}
#endif

#endif /* #defined __RANDOMHH__ */
