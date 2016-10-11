/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module flint.h          Revision: 07.05.2003                               */
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

/* Read flint.h only once */

#ifndef __FLINTH__
#define __FLINTH__

/* Turn FLINT secure mode on */
#if !(defined FLINT_SECURE || defined FLINT_UNSECURE)
#define FLINT_SECURE
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#include <time.h>


/******************************************************************************/
/* Macros                                                                     */
/******************************************************************************/
/* Simple makros */
/* Errorcodes    */

#define E_CLINT_OK     0        /* Everything O.K.                            */
#define E_CLINT_DBZ   -1        /* Division by zero                           */
#define E_CLINT_OFL   -2        /* Overflow                                   */
#define E_CLINT_UFL   -3        /* Underflow                                  */
#define E_CLINT_MAL   -4        /* Error in memory allocation                 */
#define E_CLINT_NOR   -5        /* Register not present                       */
#define E_CLINT_BOR   -6        /* Base in str2clint_l() not valid            */
#define E_CLINT_MOD   -7        /* Modulus even in ?mexp?m_l()                */
#define E_CLINT_NPT   -8        /* Null-Pointer received                      */

#define E_VCHECK_OK    0        /* CLINT-format O.K.                          */
#define E_VCHECK_LDZ   1        /* vcheck_l-Warning: Leading zeros            */
#define E_VCHECK_MEM  -1        /* vcheck_l-Error: Null-Pointer               */
#define E_VCHECK_OFL  -2        /* vcheck_l-Error: Overflow                   */


/**************************************************************/
/* Constants referring to the internal CLINT-representation   */
/**************************************************************/

#define BASE            0x10000UL
#define BASEMINONE      0xffffU
#define BASEMINONEL     0xffffUL
#define DBASEMINONE     0xffffffffUL
#define BASEDIV2        0x8000U
#define DBASEDIV2       0x80000000U
#define BITPERDGT       16UL
#define LDBITPERDGT     4U


/*******************************************************/
/* Number of digits of CLINT-ojects to base 0x10000    */

#define CLINTMAXDIGIT   256U
/*******************************************************/

#define CLINTMAXSHORT   (CLINTMAXDIGIT + 1)
#define CLINTMAXLONG    ((CLINTMAXDIGIT >> 1) + 1)
#define CLINTMAXBYTE    (CLINTMAXSHORT << 1)
#define CLINTMAXBIT     (CLINTMAXDIGIT << 4)

/* Number of small prime numbers stored in smallprimes[] */
#define NOOFSMALLPRIMES 6542

/* Default number of registers in register bank */
#define NOOFREGS        16U

/* FLINT/C-Version */
#define FLINT_VERMAJ        3   /* Major-Version */
#define FLINT_VERMIN        0   /* Minor-Version */
/* FLINT/C-Version as USHORT-value 0xhhll, hh=FLINT_VERMAJ, ll=FLINT_VERMIN */
#define FLINT_VERSION   ((FLINT_VERMAJ << 8) + FLINT_VERMIN)

#ifdef FLINT_COMPATIBILITY
/* Macros for Compatibility with version 1.xx */
#define E_OK     0              /* Everything O.K.                            */
#define E_DBZ   -1              /* Division by zero                           */
#define E_OFL   -2              /* Overflow                                   */
#define E_UFL   -3              /* Underflow                                  */
#define E_MAL   -4              /* Error in memory allocation                 */
#define E_NOR   -5              /* Register not present                       */
#define E_BOR   -6              /* Base in str2clint_l() not valid            */
#define E_MOD   -7              /* Modulus even in ?mexp?m_l()                */
#define E_NPT   -8              /* Null-Pointer received                      */
#endif /* FLINT_COMPATIBILITY */

/* Internationalization */
#define ggT_l            gcd_l
#define xggT_l           xgcd_l
#define kgV_l            lcm_l
#define zweiantei_l      twofact_l
#define chinrest_l       chinrem_l
#define primwurz_l       primroot_l


/* LINT_ASM -> FLINT_ASM, LINT_ANSI -> FLINT_ANSI */
#ifdef LINT_ASM
#ifndef FLINT_ASM
#define FLINT_ASM
#endif /* !FLINT_ASM */
#endif /* LINT_ASM */

#ifdef LINT_ANSI
#ifndef FLINT_ANSI
#define FLINT_ANSI
#endif /* !LINT_ANSI */
#endif /* LINT_ANSI */


#ifdef FLINT_ASM
#define _FLINT_ASM       0x61       /* ASCII 'a': Symbol for     */
#else                               /*  Assembler-support        */
#define _FLINT_ASM          0
#endif

#ifdef FLINT_SECURE
#define _FLINT_SECMOD    0x73       /* ASCII 's': Symbol for     */
#else                               /*  security-mode, in which  */
#define _FLINT_SECMOD       0       /*  all CLINT-Variables will */
#endif                              /*  be purged after use      */


/* Makros with parameters */

/* Definition of standard-CLINT-Registers */

#define r0_l  get_reg_l(0)
#define r1_l  get_reg_l(1)
#define r2_l  get_reg_l(2)
#define r3_l  get_reg_l(3)
#define r4_l  get_reg_l(4)
#define r5_l  get_reg_l(5)
#define r6_l  get_reg_l(6)
#define r7_l  get_reg_l(7)
#define r8_l  get_reg_l(8)
#define r9_l  get_reg_l(9)
#define r10_l get_reg_l(10)
#define r11_l get_reg_l(11)
#define r12_l get_reg_l(12)
#define r13_l get_reg_l(13)
#define r14_l get_reg_l(14)
#define r15_l get_reg_l(15)


/* MIN, MAX etc. */

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define MIN_L(a,b) (lt_l ((a), (b)) ? (a) : (b))
#define min_l(a,b) (lt_l ((a), (b)) ? (a) : (b))

#define MAX_L(a,b) (gt_l ((a), (b)) ? (a) : (b))
#define max_l(a,b) (gt_l ((a), (b)) ? (a) : (b))


#ifndef SWAP
#define SWAP(a,b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))
#endif
#ifndef swap
#define swap(a,b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))
#endif

#define SWAP_L(a,b) (xor_l((a),(b),(a)),xor_l((b),(a),(b)),xor_l((a),(b),(a)))
#define swap_l(a,b) (xor_l((a),(b),(a)),xor_l((b),(a),(b)),xor_l((a),(b),(a)))


/* ReMoveLeaDingZeRoS from CLINT-variables */
#define RMLDZRS_L(n_l) \
    while ((DIGITS_L (n_l) > 0) && (*MSDPTR_L (n_l) == 0)) {DECDIGITS_L (n_l);}

#define rmldzrs_l(n_l) \
    while ((DIGITS_L (n_l) > 0) && (*MSDPTR_L (n_l) == 0)) {DECDIGITS_L (n_l);}


/* Copy CLINT types with removal of leading zeros */
#define ZCPY_L(dest_l,src_l)\
    cpy_l ((dest_l), (src_l));\
    RMLDZRS_L ((dest_l))

#define zcpy_l(dest_l,src_l)\
    cpy_l ((dest_l), (src_l));\
    RMLDZRS_L ((dest_l))


/* Reduction modulo Nmax + 1 */
#define ANDMAX_L(a_l)\
    SETDIGITS_L ((a_l), MIN (DIGITS_L (a_l), (USHORT)CLINTMAXDIGIT));\
    RMLDZRS_L ((a_l))

#define andmax_l(a_l)\
    SETDIGITS_L ((a_l), MIN (DIGITS_L (a_l), (USHORT)CLINTMAXDIGIT));\
    RMLDZRS_L ((a_l))


/* Set CLINT-variables to values 0, 1, 2 resp. */
#define SETZERO_L(n_l)\
    (*(n_l) = 0)

#define setzero_l(n_l)\
    (*(n_l) = 0)

#define SETONE_L(n_l)\
    (u2clint_l ((n_l), 1U))

#define setone_l(n_l)\
    (u2clint_l ((n_l), 1U))

#define SETTWO_L(n_l)\
    (u2clint_l ((n_l), 2U))

#define settwo_l(n_l)\
    (u2clint_l ((n_l), 2U))


/* Read the number of digits of a CLINT-variable */
#define DIGITS_L(n_l)\
    ((unsigned short)*(n_l))

#define digits_l(n_l)\
    ((unsigned short)*(n_l))


/* Set the number of digits of a CLINT-variable */
#define SETDIGITS_L(n_l, l)\
    (*(n_l) = (unsigned short)(l))

#define setdigits_l(n_l, l)\
    (*(n_l) = (unsigned short)(l))


/* Increment the number of digits of a CLINT-variable */
#define INCDIGITS_L(n_l)\
    (++*(n_l))

#define incdigits_l(n_l)\
    (++*(n_l))


/* Decrement the number of digits of a CLINT-variable */
#define DECDIGITS_L(n_l)\
    Assert (DIGITS_L (n_l) > 0);\
    (--*(n_l))

#define decdigits_l(n_l)\
    Assert (DIGITS_L (n_l) > 0);\
    (--*(n_l))


/* Pointer to the most significant digit of a CLINT variable */
#define MSDPTR_L(n_l)\
    ((n_l) + DIGITS_L (n_l))

#define msdptr_l(n_l)\
    ((n_l) + DIGITS_L (n_l))


/* Pointer to the least significant digit of a CLINT variable */
#define LSDPTR_L(n_l)\
    ((n_l) + 1)

#define lsdptr_l(n_l)\
    ((n_l) + 1)


/* Comparisons, setting, testing for evenness and oddness */

#define LT_L(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) == -1)        /* a_l < b_l        */

#define lt_l(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) == -1)        /* a_l < b_l        */


#define LE_L(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) < 1)          /* a_l <= b_l       */

#define le_l(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) < 1)          /* a_l <= b_l       */


#define GT_L(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) == 1)         /* a_l > b_l        */

#define gt_l(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) == 1)         /* a_l > b_l        */


#define GE_L(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) > -1)         /* a_l >= b_l       */

#define ge_l(a_l,b_l) \
    (cmp_l ((a_l), (b_l)) > -1)         /* a_l >= b_l       */


#define GTZ_L(a_l) \
    (cmp_l ((a_l), nul_l) == 1)         /* a_l > 0          */

#define gtz_l(a_l) \
    (cmp_l ((a_l), nul_l) == 1)         /* a_l > 0          */


#define EQZ_L(a_l) \
    (equ_l ((a_l), nul_l) == 1)         /* a_l == 0         */

#define eqz_l(a_l) \
    (equ_l ((a_l), nul_l) == 1)         /* a_l == 0         */


#define EQONE_L(a_l) \
    (equ_l ((a_l), one_l) == 1)         /* a_l == 1         */

#define eqone_l(a_l) \
    (equ_l ((a_l), one_l) == 1)         /* a_l == 1         */


#define SET_L(a_l,ul)\
    ul2clint_l ((a_l), (ul))            /* a_l <-- unsigned long ul */

#define set_l(a_l,ul)\
    ul2clint_l ((a_l), (ul))            /* a_l <-- unsigned long ul */


#define ASSIGN_L(dest_l,src_l)\
    cpy_l ((dest_l), (src_l))           /* src_l <-- dest_l */

#define assign_l(dest_l,src_l)\
    cpy_l ((dest_l), (src_l))           /* src_l <-- dest_l */


#define ISEVEN_L(a_l)\
    (DIGITS_L (a_l) == 0 || (DIGITS_L (a_l) > 0 && (*LSDPTR_L (a_l) & 1U) == 0))
                                        /* true: a_l is even   */

#define iseven_l(a_l)\
    (DIGITS_L (a_l) == 0 || (DIGITS_L (a_l) > 0 && (*LSDPTR_L (a_l) & 1U) == 0))
                                        /* true: a_l is even   */

#define ISODD_L(a_l)\
    (DIGITS_L (a_l) > 0 && (*LSDPTR_L (a_l) & 1U) == 1)
                                        /* true: a_l is odd */

#define isodd_l(a_l)\
    (DIGITS_L (a_l) > 0 && (*LSDPTR_L (a_l) & 1U) == 1)
                                        /* true: a_l is odd */



/* Standard-Output of CLINT variables */
#define DISP_L(S,A) printf ("%s%s\n%u bit\n\n", (S), hexstr_l(A), ld_l(A))
#define disp_l(S,A) printf ("%s%s\n%u bit\n\n", (S), hexstr_l(A), ld_l(A))


/* Initialization of pseudorandom number generators */
/* with values derived from time                    */
#define INITRAND_LT()\
        ulseed64_l ((unsigned long)time(NULL))

#define initrand_lt()\
        ulseed64_l ((unsigned long)time(NULL))


#define INITRAND64_LT()\
        ulseed64_l ((unsigned long)time(NULL))

#define initrand64_lt()\
        ulseed64_l ((unsigned long)time(NULL))


/* Compatibility with version 1.0 : Macro clint2str_l */

#define CLINT2STR_L(n_l,b) xclint2str_l ((n_l), (b), 0)
#define clint2str_l(n_l,b) xclint2str_l ((n_l), (b), 0)


/* Implementation of functions xxxstr_l as macros */

#define HEXSTR_L(n_l) xclint2str_l ((n_l), 16, 0)
#define hexstr_l(n_l) xclint2str_l ((n_l), 16, 0)

#define DECSTR_L(n_l) xclint2str_l ((n_l), 10, 0)
#define decstr_l(n_l) xclint2str_l ((n_l), 10, 0)

#define OCTSTR_L(n_l) xclint2str_l ((n_l), 8, 0)
#define octstr_l(n_l) xclint2str_l ((n_l), 8, 0)

#define BINSTR_L(n_l) xclint2str_l ((n_l), 2, 0)
#define binstr_l(n_l) xclint2str_l ((n_l), 2, 0)


/* Simple factorization with sieve_l() */

#define SFACTOR_L(n_l) sieve_l ((n_l), NOOFSMALLPRIMES);
#define sfactor_l(n_l) sieve_l ((n_l), NOOFSMALLPRIMES);


/* Miller-Rabin primality test, parameters according to Lenstra and [MOV] */

#define ISPRIME_L(n) prime_l ((n), 302, 0)
#define isprime_l(n) prime_l ((n), 302, 0)


/* Choice of functions for exponentiation                    */
/* Automatic choice of  mexpk_l oder mexpkm_l is provided by */
/* function mexp_l.                                          */

/*
 * define MEXP_L(a,e,p,n) mexp5_l ((a), (e), (p), (n))
 */

#define MEXP_L(a,e,p,n) mexpk_l ((a), (e), (p), (n))

/*
 * #define MEXP_L(a,e,p,n) mexp5m_l ((a), (e), (p), (n))
 */

/*
 * #define MEXP_L(a,e,p,n) mexpkm_l ((a), (e), (p), (n))
 */


/* Purge of CLINT variables by overwriting the storage */

#ifdef FLINT_SECURE
  #define ZEROCLINT_L(A)  Assert (sizeof(A) >= CLINTMAXBYTE);\
                          purge_l (A)

  #define ZEROCLINTD_L(A) Assert (sizeof(A) >= sizeof(CLINTD));\
                          purged_l (A)

  #define ZEROCLINTQ_L(A) Assert (sizeof(A) >= sizeof(CLINTQ));\
                          purgeq_l (A)
#else
  #define ZEROCLINT_L(A)  (void)0
  #define ZEROCLINTD_L(A) (void)0
  #define ZEROCLINTQ_L(A) (void)0
#endif


/***********************************************************/
/* Typedefs                                                */
/***********************************************************/

typedef unsigned short clint;
typedef unsigned long clintd;
typedef clint CLINT[CLINTMAXSHORT];
typedef clint CLINTD[1 + (CLINTMAXDIGIT << 1)];
typedef clint CLINTQ[1 + (CLINTMAXDIGIT << 2)];
typedef clint *CLINTPTR;
#ifndef UCHAR
typedef unsigned char  UCHAR;
#endif  /* UCHAR */
#ifndef USHORT
typedef unsigned short USHORT;
#endif  /* USHORT */
#ifndef ULONG
typedef unsigned long  ULONG;
#endif  /* ULONG */


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


extern int      __FLINT_API  add_l         (CLINT, CLINT, CLINT);
extern int      __FLINT_API  chinrem_l     (unsigned int, clint**, CLINT);
extern int      __FLINT_API  cmp_l         (CLINT, CLINT);
extern void     __FLINT_API  cpy_l         (CLINT, CLINT);
extern clint *  __FLINT_API  create_l      (void);
extern int      __FLINT_API  create_reg_l  (void);
extern int      __FLINT_API  dec_l         (CLINT);

#if !defined FLINT_ASM
extern int      __FLINT_API  div_l         (CLINT, CLINT, CLINT, CLINT);
#else
extern int      __FLINT_API_A div_l        (CLINT, CLINT, CLINT, CLINT);
#endif /* FLINT_ASM */


extern void     __FLINT_API  and_l         (CLINT, CLINT, CLINT);
extern int      __FLINT_API  byte2clint_l  (CLINT, UCHAR *, int);
extern int      __FLINT_API  clearbit_l    (CLINT, unsigned int);
extern UCHAR *  __FLINT_API  clint2byte_l  (CLINT, int *);
extern char *   __FLINT_API  xclint2str_l  (CLINT, USHORT, int);
extern int      __FLINT_API  equ_l         (CLINT, CLINT);
extern char *   __FLINT_API  fbinstr_l     (CLINT);
extern char *   __FLINT_API  fdecstr_l     (CLINT);
extern char *   __FLINT_API  fhexstr_l     (CLINT);
extern char *   __FLINT_API  foctstr_l     (CLINT);
extern void     __FLINT_API  free_l        (CLINT);
extern void     __FLINT_API  free_reg_l    (void);
extern void     __FLINT_API  fswap_l       (CLINT, CLINT);
extern ULONG *  __FLINT_API  genprimes     (ULONG);
extern clint *  __FLINT_API  get_reg_l     (unsigned int);
extern void     __FLINT_API  gcd_l         (CLINT, CLINT, CLINT);
extern int      __FLINT_API  inc_l         (CLINT);
extern void     __FLINT_API  inv_l         (CLINT, CLINT, CLINT, CLINT);
extern USHORT   __FLINT_API  invmon_l      (CLINT);
extern void     __FLINT_API  iroot_l       (CLINT, CLINT);
extern unsigned __FLINT_API  issqr_l       (CLINT, CLINT);
extern int      __FLINT_API  jacobi_l      (CLINT, CLINT);
extern int      __FLINT_API  lcm_l         (CLINT, CLINT, CLINT);
extern unsigned __FLINT_API  ld_l          (CLINT);
extern int      __FLINT_API  madd_l        (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mequ_l        (CLINT, CLINT, CLINT);
extern int      __FLINT_API  mexp_l        (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mexp2_l       (CLINT, USHORT, CLINT, CLINT);
extern int      __FLINT_API  mexp5_l       (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mexp5m_l      (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mexpk_l       (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mexpkm_l      (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mmul_l        (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mod2_l        (CLINT, ULONG, CLINT);
extern int      __FLINT_API  mod_l         (CLINT, CLINT, CLINT);
extern int      __FLINT_API  msqr_l        (CLINT, CLINT, CLINT);
extern int      __FLINT_API  msub_l        (CLINT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  mul_l         (CLINT, CLINT, CLINT);
extern void     __FLINT_API  mulmon_l      (CLINT, CLINT, CLINT, USHORT, USHORT, CLINT);
extern void     __FLINT_API  or_l          (CLINT, CLINT, CLINT);
extern int      __FLINT_API  prime_l       (CLINT, unsigned int, unsigned int);
extern int      __FLINT_API  primroot_l    (CLINT, unsigned int, clint*[]);
extern int      __FLINT_API  proot_l       (CLINT, CLINT, CLINT);
extern void     __FLINT_API  purge_l       (CLINT);
extern void     __FLINT_API  purged_l      (CLINTD);
extern void     __FLINT_API  purgeq_l      (CLINTQ);
extern int      __FLINT_API  purge_reg_l   (unsigned int);
extern int      __FLINT_API  purgeall_reg_l(void);
extern clint *  __FLINT_API  rand64_l      (void);
extern void     __FLINT_API  rand_l        (CLINT, int);
extern int      __FLINT_API  randbit_l     (void);
extern int      __FLINT_API  root_l        (CLINT, CLINT, CLINT, CLINT);
extern clint *  __FLINT_API  seed64_l      (CLINT);
extern void     __FLINT_API  set_noofregs_l(unsigned int);
extern int      __FLINT_API  setbit_l      (CLINT, unsigned int);
extern clint *  __FLINT_API  setmax_l      (CLINT);
extern int      __FLINT_API  shift_l       (CLINT, long int);
extern int      __FLINT_API  shl_l         (CLINT);
extern int      __FLINT_API  shr_l         (CLINT);
extern USHORT   __FLINT_API  sieve_l       (CLINT, unsigned int);
extern int      __FLINT_API  sqr_l         (CLINT, CLINT);
extern void     __FLINT_API  sqrmon_l      (CLINT, CLINT, USHORT, USHORT, CLINT);
extern int      __FLINT_API  str2clint_l   (CLINT, char *, USHORT);
extern char *   __FLINT_API  strlwr_l      (char *);
extern char *   __FLINT_API  strrev_l      (char *);
extern char *   __FLINT_API  strupr_l      (char *);
extern int      __FLINT_API  sub_l         (CLINT, CLINT, CLINT);
extern int      __FLINT_API  testbit_l     (CLINT, unsigned int);

/* Test for GCC > 3.2.0 */
#if !(__GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || \
                       (__GNUC_MINOR__ == 2 && \
                        __GNUC_PATCHLEVEL__ > 0))))
extern int      __FLINT_API  tolower_l     (int);
extern int      __FLINT_API  toupper_l     (int);
#endif

extern void     __FLINT_API  u2clint_l     (CLINT, USHORT);
extern int      __FLINT_API  uadd_l        (CLINT, USHORT, CLINT);
extern UCHAR    __FLINT_API  ucrand64_l    (void);
extern int      __FLINT_API  udiv_l        (CLINT, USHORT, CLINT, CLINT);
extern void     __FLINT_API  ul2clint_l    (CLINT, ULONG);
extern ULONG    __FLINT_API  ulrand64_l    (void);
extern clint *  __FLINT_API  ulseed64_l    (ULONG);
extern char *   __FLINT_API  ultoa_l       (char *, ULONG, int);
extern int      __FLINT_API  umadd_l       (CLINT, USHORT, CLINT, CLINT);
extern int      __FLINT_API  umexp_l       (CLINT, USHORT, CLINT, CLINT);
extern int      __FLINT_API  umexpm_l      (CLINT, USHORT, CLINT, CLINT);
extern int      __FLINT_API  ummul_l       (CLINT, USHORT, CLINT, CLINT);
extern USHORT   __FLINT_API  umod_l        (CLINT, USHORT);
extern int      __FLINT_API  umsub_l       (CLINT, USHORT, CLINT, CLINT);
extern int      __FLINT_API  umul_l        (CLINT, USHORT, CLINT);
extern USHORT   __FLINT_API  usrand64_l    (void);
extern int      __FLINT_API  usub_l        (CLINT, USHORT, CLINT);
extern int      __FLINT_API  vcheck_l      (CLINT);
extern ULONG    __FLINT_API  version_l     (void);
extern char *   __FLINT_API  verstr_l      (void);
extern int      __FLINT_API  wmexp_l       (USHORT, CLINT, CLINT, CLINT);
extern int      __FLINT_API  wmexpm_l      (USHORT, CLINT, CLINT, CLINT);
extern void     __FLINT_API  xgcd_l        (CLINT, CLINT, CLINT, CLINT, int *, CLINT, int *);
extern void     __FLINT_API  xor_l         (CLINT, CLINT, CLINT);
extern int      __FLINT_API  twofact_l     (CLINT, CLINT);


/* Kernel functions without Overflow-Detection */
#if defined FLINT_ASM
extern void     __FLINT_API_A mult         (CLINT, CLINT, CLINT);
extern void     __FLINT_API_A umul         (CLINT, USHORT, CLINT);
extern void     __FLINT_API_A sqr          (CLINT, CLINT);
#else
extern void     __FLINT_API  mult          (CLINT, CLINT, CLINT);
extern void     __FLINT_API  umul          (CLINT, USHORT, CLINT);
extern void     __FLINT_API  sqr           (CLINT, CLINT);
#endif
extern void     __FLINT_API  add           (CLINT, CLINT, CLINT);
extern void     __FLINT_API  sub           (CLINT, CLINT, CLINT);


/* Functions operating on signed CLINT-Values */
extern int      __FLINT_API  sadd          (CLINT, int, CLINT, int, CLINT);
extern int      __FLINT_API  ssub          (CLINT, int, CLINT, int, CLINT);
extern int      __FLINT_API  smod          (CLINT, int, CLINT, CLINT);


/* Constants */
extern clint  __FLINT_API_DATA nul_l[];
extern clint  __FLINT_API_DATA one_l[];
extern clint  __FLINT_API_DATA two_l[];
extern USHORT __FLINT_API_DATA smallprimes[];

#ifdef  __cplusplus
}
#endif

#endif /* #defined __FLINTH__ */
