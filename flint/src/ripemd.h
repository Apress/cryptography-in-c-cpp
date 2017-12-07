/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module ripemd.h         Revision: 01.07.2002                               */
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

#ifndef __RIPEMDH__
#define __RIPEMDH__

#ifdef  __cplusplus
extern "C" {
#endif

#define RMDVER 160
#define E_CLINT_RMD -20

/* Macros maintaining FLINT/C namespace */

#define ripeinit        ripeinit_l
#define ripehash	ripehash_l    
#define ripefinish	ripefinish_l
#define ripemd160	ripemd160_l  
#define ripeinit128	ripeinit128_l
#define ripehash128	ripehash128_l
#define ripefinish128	ripefinish128_l
#define ripemd128     	ripemd128_l


struct ripemd_stat {
    unsigned int stomach[5];
    unsigned int total[2];
};

typedef struct ripemd_stat RMDSTAT;


/***********************************************************/
/* Function prototypes                                     */
/***********************************************************/

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


/* RIPEMD-160 */
extern void __FLINT_API  
ripeinit_l (RMDSTAT *);
extern int  __FLINT_API  
ripehash_l (RMDSTAT *, unsigned char *, unsigned int);
extern void __FLINT_API  
ripefinish_l (unsigned char *, RMDSTAT *, unsigned char *, unsigned int);
extern void __FLINT_API 
ripemd160_l (unsigned char *, unsigned char *, unsigned int);

/* RIPEMD-128 */
extern void __FLINT_API  
ripeinit128_l (RMDSTAT *);
extern int  __FLINT_API  
ripehash128_l (RMDSTAT *, unsigned char *, unsigned int);
extern void __FLINT_API  
ripefinish128_l (unsigned char *, RMDSTAT *, unsigned char *, unsigned int);
extern void __FLINT_API 
ripemd128_l (unsigned char *, unsigned char *, unsigned int);

#ifdef  __cplusplus
}
#endif

#endif /* __RIPEMDH__ */

















