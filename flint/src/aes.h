/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module aes.h            Revision: 10.07.2002                               */
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

#ifndef __AESH__
#define __AESH__

#ifdef  __cplusplus
extern "C" {
#endif


/* Macros */

#define MAXCKEY              8  /* Maximum length of user key in 4-byte words */
#define MAXNCOL              8  /* Maximum blocklength in 4-byte words */
#define MAXNRND             14  /* Maximum number of rounds */
#define BITSPERBLOCK       128  /* Default number of bits in a cipher block */

#define AES_ENC              0
#define AES_DEC              1
#define AES_ECB             10
#define AES_CBC             11

#define AES_OK               0
#define AES_ERR_INIT        -1
#define AES_ERR_MODE        -2
#define AES_ERR_BLKLENGTH   -4
#define AES_ERR_KEYLENGTH   -8
#define AES_ERR_DIRECTION  -16

#if !(defined AES_SECURE || defined AES_UNSECURE)
#define AES_SECURE
#endif

/* Macros maintaining FLINT/C namespace */

#define AESInit               AESInit_l
#define	AESKeySched	      AESKeySched_l
#define	AESCrypt	      AESCrypt_l    
#define	AESPurge	      AESPurge_l       
#define AESKeyExpansion       AESKeyExpansion_l
#define	AESInvKeyExpansion    AESInvKeyExpansion_l
#define	AESEncryptBlock       AESEncryptBlock_l
#define	AESDecryptBlock       AESDecryptBlock_l
#define	AESEncBlock           AESEncBlock_l
#define	AESDecBlock           AESDecBlock_l    
#define	AESEncState           AESEncState_l    
#define	AESDecState           AESDecState_l    
#define	AESEncBlockRounds     AESEncBlockRounds_l
#define	AESDecBlockRounds     AESDecBlockRounds_l

/* Conversion of a vector of type UCHAR into ULONG word */

#if !defined AES_BIGENDIAN
#define UC2UL(ucptr)  *(unsigned long *)((ucptr))
#else
#define UC2UL(ucptr) \
          (((unsigned long) *((ucptr)+3) << 24) | \
           ((unsigned long) *((ucptr)+2) << 16) | \
           ((unsigned long) *((ucptr)+1) <<  8) | \
           ((unsigned long) *(ucptr)))
#endif

/* AES specific types */
typedef unsigned long AESXPKEY[(MAXNRND+1)*MAXNCOL];
typedef unsigned long AESSTATE[MAXNCOL];
typedef unsigned long AESROUNDKEY[MAXNCOL];
typedef unsigned char AESBLOCK[4*MAXNCOL];
typedef unsigned char AESKEY[4*MAXCKEY];

typedef struct
{
  AESXPKEY ExpandedKey; 
  int mode;            /* Mode of Operation: AES_ECB, AES_CBC */
  int KeyLength;       /* 128, 192, 256 */
  int BlkLength;       /* 128, 192, 256 */
  int DFlag;           /* AES_ENC, AES_DEC */
} AESKEYSCHED;


typedef struct
{
  int mode;            /* Mode of Operation: AES_ECB, AES_CBC */
  int BlkLength;       /* 128, 192, 256 */
  AESBLOCK IV;         /* Initialization Vector for CBC Mode */
  AESSTATE State;      /* State Buffer */
  AESSTATE Block;      /* Previous State Buffer for CBC Decryption */
} AESWORKSPACE;
  

/******************************************************************************/
/* AES API: Public Functions */

#ifndef __FLINT_API
#ifdef FLINT_USEDLL
#define __FLINT_API                   __cdecl
#else
#define __FLINT_API                   /**/
#endif /* FLINT_USEDLL */
#endif /* !defined __FLINT_API */

#if !defined __FLINT_API_A
#if defined __GNUC__ && !defined __cdecl
#define __FLINT_API_A                 /**/
#else
#define __FLINT_API_A                 __cdecl
#endif /* !defined __GNUC__ */
#endif /* !defined __FLINT_API_A */


/* If the FLINT/C-Package is used under MS Visual C/C++ as DLL,               */
/* all modules accessing data nul_l, one_l, two_l or smallprimes from outside */
/* the DLL must be compiled with -D__FLINT_API_DATA=__declspec(dllimport)     */

#ifndef __FLINT_API_DATA
#if (defined _MSC_VER && _MSC_VER >= 11) && defined FLINT_USEDLL
#define __FLINT_API_DATA              __declspec(dllimport)
#else
#define __FLINT_API_DATA              /**/
#endif /* MSC_VER && FLINT_USEDLL */
#endif /* !defined __FLINT_API_DATA */


int __FLINT_API 
AESInit_l (AESWORKSPACE *   ws, 
           int              mode, 
           int              BlkLength, 
           AESBLOCK         IV, 
           AESKEYSCHED *    AESKey, 
           AESKEY           RawKey, 
           int              KeyLength, 
           int              DFlag);

int __FLINT_API 
AESCrypt_l (AESBLOCK        OutBlock, 
            AESWORKSPACE *  ws, 
            AESKEYSCHED *   ks, 
            AESBLOCK        Inblock, 
            int             InLength);

int __FLINT_API 
AESKeySched_l (AESKEYSCHED *AESKey, 
               AESKEY       RawKey, 
               int          KeyLength, 
               int          BlkLength, 
               int          DFlag, 
               int          mode);

void __FLINT_API
AESPurge_l (AESWORKSPACE *  ws, 
            AESKEYSCHED *   ks);


/******************************************************************************/
/* AES public kernel functions */

/* Key expansion for encryption / decryption */
int __FLINT_API 
AESKeyExpansion_l (AESXPKEY ExpandedKey, AESKEY RawKey, int KeyLength, int BlockLength);

int __FLINT_API
AESInvKeyExpansion_l (AESXPKEY InvExpandedKey, AESKEY RawKey, int KeyLength, int BlockLength);

/* Encryption of a single block w/ KeyExpansion */
int __FLINT_API 
AESEncryptBlock_l (AESBLOCK CipherBlock, AESBLOCK MessBlock, AESKEY RawKey, int KeyLength, int BlockLength);

/* decryption of a single block w/ InvKeyExpansion */
int __FLINT_API
AESDecryptBlock_l (AESBLOCK MessBlock, AESBLOCK CipherBlock, AESKEY RawKey, int KeyLength, int BlockLength);

/* Encryption of a single block w/o KeyExpansion */
int __FLINT_API
AESEncBlock_l (AESBLOCK CipherBlock, AESBLOCK MessBlock, AESXPKEY ExpandedKey, int KeyLength, int BlockLength);

/* Decryption of a single block w/o InvKeyExpansion */
int __FLINT_API 
AESDecBlock_l (AESBLOCK MessBlock, AESBLOCK CipherBlock, AESXPKEY ExpandedKey, int KeyLength, int BlockLength);

/* State encryption */
int __FLINT_API
AESEncState_l (AESSTATE State, AESXPKEY ExpandedKey, int Nk, int Nb);

/* State decryption */
int  __FLINT_API
AESDecState_l (AESSTATE State, AESXPKEY InvExpandedKey, int Nk, int Nb);

/* Block encryption w/ variable number of rounds */
int __FLINT_API
AESEncBlockRounds_l (AESBLOCK CipherBlock, AESBLOCK MessBlock, AESXPKEY ExpandedKey, int KeyLength, int BlockLength, int Rounds);

/* Block decryption w/ variable number of rounds */
int __FLINT_API
AESDecBlockRounds_l (AESBLOCK CipherBlock, AESBLOCK MessBlock, AESXPKEY ExpandedKey, int KeyLength, int BlockLength, int Rounds);

#ifdef  __cplusplus
}
#endif

#endif /* __AESH__ */
