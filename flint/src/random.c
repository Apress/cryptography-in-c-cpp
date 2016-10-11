/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module random.c         Revision: 17.04.2005                               */
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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "flint.h"
#include "random.h"

#define NO_ASSERTS 1

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_assert.h"
#include "_alloc.h"
#ifdef COVERAGE
#include "utclog.h"
#endif
#endif

#ifdef NO_ASSERTS
#define Assert(a) (void)0
#endif

static void *
local_memset (void *ptr, int val, size_t len);


static int Rand64Init = 0;


/******************************************************************************/
/*                                                                            */
/*  Function:  Initialization of the linear congruential PRNG Rand64          */
/*             Do not use Rand64 for any cryptographic purposes!              */
/*  Syntax:    int InitRand64_l (char *UsrStr, int LenUsrStr,                 */
/*                                                    int AddEntropy);        */
/*  Input:     UsrStr (User supplied byte vector for initialization of        */
/*                     Rand64)                                                */
/*             LenUsrStr (Length of UsrString in byte)                        */
/*             AddEntropy (Number of additionally required entropy bytes)     */
/*  Output:    -                                                              */
/*  Returns:   Number of requested entropy bytes missing                      */
/*             0: OK                                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
InitRand64_l (char *UsrStr, int LenUsrStr, int AddEntropy)
{
  int MissingEntropy;
  CLINT Seed_l;

  /* Get Required Entropy and Seed */
  MissingEntropy = GetEntropy_l (Seed_l, NULL, AddEntropy, UsrStr, LenUsrStr);

  /* Initialize internal state */
  seed64_l (Seed_l);

#ifdef FLINT_SECURE
  /* Security: Purge random variable Seed_l */
  local_memset (Seed_l, 0, sizeof (CLINT));
#endif

  /* Set Flag: PRNG is initialized */
  Rand64Init = 1;

  return MissingEntropy;
}


/************************* RipeMD-160 / SHA-1 based PRNG **********************/

/******************************************************************************/
/*                                                                            */
/*  Function:  Initialisation of the SHA-1/Ripemd-160 based Pseudorandom-     */
/*             numbergenerator (RandRMDSHA1)                                  */
/*  Syntax:    int InitRandRMDSHA1_l (STATERMDSHA1 *rstate, char *UsrStr,     */
/*                                     int LenUsrStr, int AddEntropy);        */
/*  Input:     UsrStr (User supplied byte vector for initialization of        */
/*                     RandRMDSHA1)                                           */
/*             LenUsrStr (Length of UsrString in byte)                        */
/*             AddEntropy (Number of additionally required entropy bytes)     */
/*  Output:    rstate (Initialized internal state of PRNG)                    */
/*  Returns:   Number of requested entropy bytes missing                      */
/*             0: OK                                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
InitRandRMDSHA1_l (STATERMDSHA1 *rstate, char *UsrStr, int LenUsrStr, int AddEntropy)
{
  int MissingEntropy;
  /* Get Required Entropy and Seed */
  MissingEntropy = GetEntropy_l (NULL, rstate->XRMDSHA1, AddEntropy, UsrStr, LenUsrStr);

  /* Switch internal state */
  ripemd160_l (rstate->XRMDSHA1, rstate->XRMDSHA1, 20);

  /* Initialize RoundRMDSHA1 for picking single bytes out of random stream */
  rstate->RoundRMDSHA1 = 1;

  /* Set Flag: PRNG is initialized */
  rstate->RandRMDSHA1Init = 1;

  return MissingEntropy;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Switching of RandRMDSHA1 to next state, generating one         */
/*             random byte                                                    */
/*  Syntax:    UCHAR SwitchRandRMDSHA1_l (STATERMDSHA1 *rstate)               */
/*  Input:     rstate (Initialized State)                                     */
/*  Output:    rstate (Internal State)                                        */
/*  Returns:   One byte random value                                          */
/*                                                                            */
/******************************************************************************/
UCHAR __FLINT_API
SwitchRandRMDSHA1_l (STATERMDSHA1 *rstate)
{
  UCHAR rbyte;
  
  /* Generate random value */
  sha1_l (rstate->SRMDSHA1, rstate->XRMDSHA1, 20);
  rbyte = rstate->SRMDSHA1[(rstate->RoundRMDSHA1)++ & 15]; /* RoundRMDSHA1 mod 16 */

  /* Switch internal state */
  ripemd160_l (rstate->XRMDSHA1, rstate->XRMDSHA1, 20);

#ifdef FLINT_SECURE
  /* Security: Purge random variable SRMDSHA1 */
  local_memset (rstate->SRMDSHA1, 0, sizeof (rstate->SRMDSHA1));
#endif

  return rbyte;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of pseudorandom number of CLINT-Type                */
/*             (Preceding Initialization by InitRandRMDSHA1() required)       */
/*  Syntax:    int RandRMDSHA1_l (STATERMDSHA1 *rstate, CLINT r_l, int l);    */
/*  Input:     rstate (Initialized state)                                     */
/*             l (Number of binary digits of random number to generated)      */
/*  Output:    r_l (Pseudorandom number)                                      */
/*             rstate (New internal state of PRNG)                            */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RIN if generator is not initialized                    */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
RandRMDSHA1_l (CLINT r_l, STATERMDSHA1 *rstate, int l)
{
  USHORT i, j;
  USHORT ls, lr;

  if (!rstate->RandRMDSHA1Init)
    {
      return E_CLINT_RIN;
    }

  l = MIN ((unsigned int)l, CLINTMAXBIT);
  ls = (USHORT) l >> LDBITPERDGT;
  lr = (USHORT) l & ((USHORT)BITPERDGT - 1);

  for (i = 1; i <= ls; i++)
    {
      r_l[i] = sRandRMDSHA1_l (rstate);
    }
  if (lr > 0)
    {
      r_l[++ls] = sRandRMDSHA1_l (rstate);
      j = 1U << (lr - 1);                         /* j <- 2^(lr - 1) */
      r_l[ls] = (r_l[ls] | j) & ((j << 1) - 1);   /* Bit lr set to 1, higher bits to 0 */
    }
  else
    {
      r_l[ls] |= BASEDIV2;
    }

  SETDIGITS_L (r_l, ls);
  return 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of internat status of RandRMDSHA1                     */
/*  Syntax:     void PurgeRandRMDSHA1_l (STATERMDSHA1 *rstate);               */
/*  Input:      rstate (Initialized state)                                    */
/*  Output:     rstate (Purged internal state)                                */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
PurgeRandRMDSHA1_l (STATERMDSHA1 *rstate)
{
  /* Purge status of RipeMD-160/Sha-1 based PRNG */
  memset (rstate->XRMDSHA1, 0, sizeof (rstate->XRMDSHA1));
  memset (rstate->SRMDSHA1, 0, sizeof (rstate->SRMDSHA1));
  rstate->RoundRMDSHA1 = 0;

  /* Reset initialization flag */
  rstate->RandRMDSHA1Init = 0;
}


/************************* AES based PRNG with key update *********************/

static const unsigned char RandAESKey[] =
{0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78,
 0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0, 
 0x12, 0x23, 0x34, 0x45, 0x56, 0x78, 0x89, 0x9a,
 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0, 0x01, 0x12};

/******************************************************************************/
/*                                                                            */
/*  Function:  Initialisation of the AES based Pseudorandom-                  */
/*             numbergenerator (RandAES)                                      */
/*  Syntax:    int InitRandRMDSHA1_l (STATEAES *rstate, char *UsrStr,         */
/*                                    int LenUsrStr, int AddEntropy);         */
/*  Input:     UsrStr (User supplied byte vector for initialization of        */
/*                     RandAES)                                               */
/*             LenUsrStr (Length of UsrString in byte)                        */
/*             AddEntropy (Number of additionally required entropy bytes)     */
/*             update (Frequency AES-key-update by XORing with output value:  */
/*                     update = 0: Never update key, fastest                  */
/*                     update = 1: Always update key, slowest, most secure    */
/*                     update = 1 < n: Update key every n'th call to RandAES  */
/*  Output:    rstate (Initialized internal state of PRNG)                    */
/*  Returns:   Number of requested entropy bytes missing                      */
/*             0: OK                                                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
InitRandAES_l (STATEAES *rstate, char *UsrStr, int LenUsrStr, int AddEntropy, int update)
{
  int MissingEntropy, i;
  /* Get Required Entropy and Seed */
  MissingEntropy = GetEntropy_l (NULL, rstate->XAES, AddEntropy, UsrStr, LenUsrStr);

  /* Initialize AES */
  for (i = 0; i < 32; i++)
    {
      rstate->RandAESKey[i] ^= RandAESKey[i];
    }

  AESInit_l (&rstate->RandAESWorksp, AES_ECB, 192, NULL, &rstate->RandAESSched, rstate->RandAESKey, 256, AES_ENC);

  /* Switch internal state */
  AESCrypt_l (rstate->XAES, &rstate->RandAESWorksp, &rstate->RandAESSched, rstate->XAES, 24); 

  /* Set parameter for frequency of key update */
  rstate->UpdateKeyAES = update;
  
  /* Initialize RoundAES for picking single bytes out of random stream */
  rstate->RoundAES = 1;

  /* Set Flag: PRNG is initialized */
  rstate->RandAESInit = 1;

  return MissingEntropy;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Switching of RandAES to the next state, generating one         */
/*             random byte                                                    */
/*  Syntax:    UCHAR SwitchRandAES_l (STATEAES *rstate)                       */
/*  Input:     rstate (Initialized state of PRNG)                             */
/*  Output:    rstate (New internal state of PRNG)                            */
/*  Returns:   One byte random value                                          */
/*                                                                            */
/******************************************************************************/
UCHAR __FLINT_API
SwitchRandAES_l (STATEAES *rstate)
{
  int i;
  UCHAR rbyte;

  AESCrypt_l (rstate->XAES, &rstate->RandAESWorksp, &rstate->RandAESSched, rstate->XAES, 24); 

  rbyte = rstate->XAES[(rstate->RoundAES)++ & 15]; /* RoundAES mod 16 */
  
  if (rstate->UpdateKeyAES)
    {
      if (0 == (rstate->RoundAES % rstate->UpdateKeyAES))
        {
          for (i = 0; i < 32; i++)
            {
              rstate->RandAESKey[i] ^= rstate->XAES[i];
            }

          AESInit_l (&rstate->RandAESWorksp, AES_ECB, 192, NULL, &rstate->RandAESSched, 
                                                rstate->RandAESKey, 256, AES_ENC);
        }
    }

  return rbyte;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of pseudorandom number of CLINT-Type                */
/*             (Preceding Initialization by InitRandAES() required)           */
/*  Syntax:    int RandAES_l (CLINT r_l, STATEAES *rstate, int l);            */
/*  Input:     l (Number of binary digits of random number to generated)      */
/*  Output:    r_l (Pseudorandom number)                                      */
/*             rstate (Internal state of PRNG)                                */
/*  Returns:   E_CLINT_OK  if everything is OK                                */
/*             E_CLINT_RIN if generator is not initialized                    */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
RandAES_l (CLINT r_l, STATEAES *rstate, int l)
{
  USHORT i, j;
  USHORT ls, lr;

  if (!rstate->RandAESInit)
    {
      return E_CLINT_RIN;
    }

  l = MIN ((unsigned int)l, CLINTMAXBIT);
  ls = (USHORT) l >> LDBITPERDGT;
  lr = (USHORT) l & ((USHORT)BITPERDGT - 1);

  for (i = 1; i <= ls; i++)
    {
      r_l[i] = sRandAES_l(rstate);
    }
  if (lr > 0)
    {
      r_l[++ls] = sRandAES_l(rstate);
      j = 1U << (lr - 1);                         /* j <- 2^(lr - 1) */
      r_l[ls] = (r_l[ls] | j) & ((j << 1) - 1);   /* Bit lr set to 1, higher bits to 0 */
    }
  else
    {
      r_l[ls] |= BASEDIV2;
    }

  SETDIGITS_L (r_l, ls);
  return 0;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of internat status of RandAES                         */
/*  Syntax:     void PurgeRandAES_l (STATEAES *rstate);                       */
/*  Input:      rstate (Initialized internal state of PRNG)                   */
/*  Output:     rstate (Purged state of PRNG)                                 */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
PurgeRandAES_l (STATEAES *rstate)
{
  /* Purge status of AES based PRNG */
  memset (rstate->RandAESKey, 0, sizeof (rstate->RandAESKey));
  memset (rstate->XAES, 0, sizeof (rstate->XAES));
  memset (&rstate->RandAESWorksp, 0, sizeof (rstate->RandAESWorksp));
  memset (&rstate->RandAESSched, 0, sizeof (rstate->RandAESSched));
  rstate->RoundAES = 0;

  /* Reset initialization flag */
  rstate->RandAESInit = 0;
}


/******************************************************************************/

#if (CLINTMAXDIGIT >= 128)
static const char *MODBBSSTR =
"81aa5c97200fb2504c08b92e2f8b7e0805775c72b139b455846028653793ba9d616cd01cef719"
"5b2f952e046ab62d063b048c8f62b21c7fc7fa4e71eef2588aa59b81f355b5539a471ee483b02"
"2bdab25b3fb41bc6224d60c17bbcb091294f76cb64c3b6da5504085a958b679d1f1147158189d"
"4fa76ab721c535a3ecfe4452cc61b9466e315ba4b4392db04f686dbb61084b21c9540f972718f"
"c96ed25a40a917a07c668048683ec116219621a301610bfba40e1ab11bd1e13aa8476aa6d37b2"
"61228df85c7df67043c51b37a250b27401aaf837101d2db1a55572dd7a79646ff6e5d20a24e4b"
"43c6d8ab5e9a77becd76d5f0355252f4318e2066d3f9c42f25";
#elif (CLINTMAXDIGIT >= 64)
static const char *MODBBSSTR =
"a1c0a7edba2a2aee2cb3947c3d1c0468ee5a5791ec3ebb97238bd4c3bdad1a00280f0a7518d56"
"523003d5cee48a60d606d78b818d81b0ef963555b9b62fc3b5f796815946ed28987596f84ccc1"
"7f87b9ca5959fc9763bc43521aa467cdcec60cd9fa7548268169750adf746df899cc64b059b7c"
"194ab4ba492c04c3a6c630103";
#elif (CLINTMAXDIGIT >= 48)
static const char *MODBBSSTR =
"c58d49cd9529aed21da56db12844522c04ace305362219478a99da74751213f8ccdfb52fb7a8b"
"fc2d5ce18c86c9e447f78b9013071d2fbb4be506f942cc45793e752733c71b07f40c3e54a9bdc"
"3d9bb18a2c9411e8f898b28d060ea0dc9b309b";
#elif (CLINTMAXDIGIT >= 32)
static const char *MODBBSSTR =
"a3d46604762377bccd0ab8562b46132740b75feb0e3ca7a79022736c6a5ca0b17a03465222af1"
"a074e31224ea01fc48b3150579c06ef8f073673a5169e8ea021";
#elif (CLINTMAXDIGIT >= 16)
static const char *MODBBSSTR =
"b2c31d33668afb5600be97e13b769fe4f558fc96bc46b8d174d94fb468ff31a5";
#elif (CLINTMAXDIGIT >= 8)
static const char *MODBBSSTR =
"845196304e498ea78ff06d51bd58c9e3";
#endif


/******************************************************************************/
/*                                                                            */
/*  Function:  Initialisation of the Blum-Blum-Shub-PNRG (RandBBS)            */
/*             with entropy generated by GetEntropy_l                         */
/*  Syntax:    int InitRandBBS_l (STATEBBS *rstate, char *UsrStr,             */
/*                                      int LenUsrStr, int AddEntropy);       */
/*  Input:     UsrStr (String for initialization optionally provided by user) */
/*             LenUsrStr (Length of UsrStr in bytes)                          */
/*             AddEntropy (Number of additionally requested entropy bytes     */
/*  Output:    rstate (Initialized internal state of PRNG)                    */
/*  Returns:   0: OK                                                          */
/*             n > 0: Number of requested but not generated entropy bytes     */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
InitRandBBS_l (STATEBBS *rstate, char *UsrStr, int LenUsrStr, int AddEntropy)
{
  CLINT Seed_l;
  int MissingEntropy;

  /* Get Required Entropy and Seed */
  MissingEntropy = GetEntropy_l (Seed_l, NULL, AddEntropy, UsrStr, LenUsrStr);

  /* Switch internal state */
  SeedBBS_l (rstate, Seed_l);

  /* Security: Purge seed variable Seed_l */
  local_memset (Seed_l, 0, sizeof (CLINT));

  return MissingEntropy;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Initialization of BLUM-BLUM-SHUB-PRNG with seed of type CLINT  */
/*  Syntax:    int SeedBBS_l (STATEBBS *rstate, CLINT seed_l);                */
/*  Input:     seed_l (Seed)                                                  */
/*  Output:    rstate (Internal state of PRNG)                                */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RCP if seed and modulus are not coprime                */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
SeedBBS_l (STATEBBS *rstate, CLINT seed_l)
{
  CLINT g_l;

  str2clint_l (rstate->MODBBS, (char*)MODBBSSTR, 16);
  gcd_l (rstate->MODBBS, seed_l, g_l);

  if (!EQONE_L (g_l))
    {
      return E_CLINT_RCP;
    }

  msqr_l (seed_l, rstate->XBBS, rstate->MODBBS);

  /* Set Flag: PRNG is initialized */
  rstate->RandBBSInit = 1;

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a random bit acc. to Blum-Blum-Shub              */
/*  Syntax:    int SwitchRandBBS_l (STATEBBS *rstate);                        */
/*  Input:     rstate (Initialized internal state of PRNG)                    */
/*  Output:    rstate (Internal state of PRNG)                                */
/*  Returns:   Random bit 0 or 1                                              */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
SwitchRandBBS_l (STATEBBS * rstate)
{
  msqr_l (rstate->XBBS, rstate->XBBS, rstate->MODBBS);

  /* Output least significant bit of rstate->XBBS */
  return (*LSDPTR_L (rstate->XBBS) & 1);
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type UCHAR              */
/*  Syntax:    UCHAR bRandBBS_l (STATEBBS *rstate);                           */
/*  Input:     rstate (Initialized internal state of PRNG)                    */
/*  Output:    rstate (Internal state of PRNG)                                */
/*  Returns:   Pseudorandom number of type UCHAR                              */
/*                                                                            */
/******************************************************************************/
UCHAR __FLINT_API
bRandBBS_l (STATEBBS * rstate)
{
  int i;
  UCHAR r = SwitchRandBBS_l(rstate);
  for (i = 1; i < (sizeof (UCHAR) << 3); i++)
    {
      r = (r << 1) + SwitchRandBBS_l(rstate);
    }

  return r;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type USHORT             */
/*  Syntax:    USHORT sRandBBS_l (STATEBBS *rstate);                          */
/*  Input:     rstate (Initialized internal state of PRNG)                    */
/*  Output:    rstate (Internal state of PRNG)                                */
/*  Returns:   Pseudorandom number of type USHORT                             */
/*                                                                            */
/******************************************************************************/
USHORT __FLINT_API
sRandBBS_l (STATEBBS * rstate)
{
  int i;
  USHORT r = SwitchRandBBS_l(rstate);
  for (i = 1; i < (sizeof (USHORT) << 3); i++)
    {
      r = (r << 1) + SwitchRandBBS_l(rstate);
    }

  return r;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type ULONG              */
/*  Syntax:    ULONG lRandBBS_l (STATEBBS *rstate);                           */
/*  Input:     rstate (Initialized internal state of PRNG)                    */
/*  Output:    rstate (Internal state of PRNG)                                */
/*  Returns:   Pseudorandom number of type ULONG                              */
/*                                                                            */
/******************************************************************************/
ULONG __FLINT_API
lRandBBS_l (STATEBBS * rstate)
{
  int i;
  ULONG r = SwitchRandBBS_l(rstate);
  for (i = 1; i < (sizeof (ULONG) << 3); i++)
    {
      r = (r << 1) + SwitchRandBBS_l (rstate);
    }

  return r;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type CLINT              */
/*             (Preceding Initialization by InitRandBBS() required)           */
/*  Syntax:    int RandBBS_l (CLINT r_l, STATEBBS *rstate, int l);            */
/*  Input:     rstate (Initialized internal state of PRNG)                    */
/*             l (Number of binary digits of random number to generated)      */
/*  Output:    r_l (Pseudorandom number)                                      */
/*             rstate (Internal state of PRNG)                                */
/*  Returns:   E_CLINT_OK  if everything is OK                                */
/*             E_CLINT_RIN if generator is not initialized                    */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
RandBBS_l (CLINT r_l, STATEBBS * rstate, int l)
{
  USHORT i, j;
  USHORT ls, lr;

  if (!rstate->RandBBSInit)
    {
      return E_CLINT_RIN;
    }

  l = (int)MIN ((unsigned int)l, CLINTMAXBIT);
  ls = (USHORT)l >> LDBITPERDGT;
  lr = (USHORT)l & ((USHORT)BITPERDGT - 1);

  for (i = 1; i <= ls; i++)
    {
      r_l[i] = sRandBBS_l (rstate);
    }

  if (lr > 0)
    {
      ++ls;
      r_l[ls] = sRandBBS_l (rstate);
      j = 1U << (lr - 1);                         /* j <- 2^(lr - 1) */
      r_l[ls] = (r_l[ls] | j) & ((j << 1) - 1);   /* bt lr to 1, higher bits to */
    }
  else
    {
      r_l[ls] |= BASEDIV2;
    }

  SETDIGITS_L (r_l, ls);

  /* Purging ov variables */
  local_memset (&i, 0, sizeof (i));
  local_memset (&j, 0, sizeof (j));
  local_memset (&ls, 0, sizeof (ls));
  local_memset (&lr, 0, sizeof (lr));

  return E_CLINT_OK;
}


/******************************************************************************/
/*                                                                            */
/*  Function:   Purging of the internal state of RandBBS                      */
/*  Syntax:     void PurgeRandBBS_l (STATEBBS * rstate);                      */
/*  Input:      rstate (Internal state of PRNG)                               */
/*  Output:     rstate (Purged indernal state of PRNG)                        */
/*  Returns:    -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
PurgeRandBBS_l (STATEBBS * rstate)
{
  /* Purge status of BBS-PRNG */
  memset (rstate->XBBS, 0, sizeof (CLINT));

  /* Reset initialization flag */
  rstate->RandBBSInit = 0;
}


/*** Higher Level Functions for Generation of Random Numbers of Type CLINT ****/


/******************************************************************************/
/*                                                                            */
/*  Function:  Initialisation of a chosen PRNG                                */
/*             with entropy generated by GetEntropy_l                         */
/*  Syntax:    int InitRand_l (STATEPRNG *xrstate, char *UsrStr,              */
/*                             int LenUsrStr, int AddEntropy, int Generator); */
/*  Input:     UsrStr (String for initialization optionally provided by user) */
/*             LenUsrStr (Length of UsrStr in bytes)                          */
/*             AddEntropy (Number of additionally requested entropy bytes     */
/*             Generator (PRNG to be initialized: one of FLINT_RND64          */
/*                                                       FLINT_RNDRMDSHA1     */
/*                                                       FLINT_RNDAES         */
/*                                                       FLINT_RNDBBS)        */
/*  Output:    xrstate (Initialized state of random number generator)         */
/*  Output:    -                                                              */
/*  Returns:   0: OK                                                          */
/*             n > 0: Number of requested but not generated entropy bytes     */
/*             n < 0: Requested generator does not exist, RND64 initialized   */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
InitRand_l (STATEPRNG *xrstate, char *UsrStr, int LenUsrStr, int AddEntropy, int Generator)
{
  int error;

  switch (Generator)
    {
      case FLINT_RNDBBS:
        error = InitRandBBS_l (&xrstate->StateBBS, (char*)UsrStr, LenUsrStr, AddEntropy);
        xrstate->Generator = FLINT_RNDBBS;
        break;
      case FLINT_RNDRMDSHA1:
        error = InitRandRMDSHA1_l (&xrstate->StateRMDSHA1, (char*)UsrStr, LenUsrStr, AddEntropy);
        xrstate->Generator = FLINT_RNDRMDSHA1;
        break;
      case FLINT_RNDAES:
        error = InitRandAES_l (&xrstate->StateAES, (char*)UsrStr, LenUsrStr, AddEntropy, 10);
        xrstate->Generator = FLINT_RNDAES;
        break;
      case FLINT_RND64:
        error = InitRand64_l ((char*)UsrStr, LenUsrStr, AddEntropy);
        xrstate->Generator = FLINT_RND64;
        break;
      default:
        InitRand64_l ((char*)UsrStr, LenUsrStr, AddEntropy);
        xrstate->Generator = FLINT_RND64;
        error = -AddEntropy;
    }

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type UCHAR              */
/*  Syntax:    UCHAR bRand_l (STATEPRNG *xrstate)                             */
/*  Input:     xrstate (Initialized state of chosen random number generator)  */
/*  Output:    -                                                              */
/*  Returns:   Pseudorandom number of type UCHAR                              */
/*                                                                            */
/******************************************************************************/
UCHAR __FLINT_API
bRand_l (STATEPRNG *xrstate)
{
  UCHAR r;

  switch (xrstate->Generator)
    {
      case FLINT_RNDBBS:
        r = bRandBBS_l (&xrstate->StateBBS);
        break;
      case FLINT_RNDRMDSHA1:
        r = bRandRMDSHA1_l (&xrstate->StateRMDSHA1);
        break;
      case FLINT_RNDAES:
        r = bRandAES_l (&xrstate->StateAES);
        break;
      case FLINT_RND64:
        r = ucrand64_l ();
        break;
      default:
        r = ucrand64_l ();
    }

  return r;
}



/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type USHORT             */
/*  Syntax:    USHORT sRand_l (STATEPRNG *xrstate);                           */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*  Output:    xrstate (State of chosen PRNG)                                 */
/*  Returns:   Pseudorandom number of type USHORT                             */
/*                                                                            */
/******************************************************************************/
USHORT __FLINT_API
sRand_l (STATEPRNG *xrstate)
{
  USHORT r;

  switch (xrstate->Generator)
    {
      case FLINT_RNDBBS:
        r = sRandBBS_l (&xrstate->StateBBS);
        break;
      case FLINT_RNDRMDSHA1:
        r = sRandRMDSHA1_l (&xrstate->StateRMDSHA1);
        break;
      case FLINT_RNDAES:
        r = sRandAES_l (&xrstate->StateAES);
        break;
      case FLINT_RND64:
        r = usrand64_l ();
        break;
      default:
        r = usrand64_l ();
    }

  return r;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type ULONG              */
/*  Syntax:    ULONG lRand_l (STATEPRNG *xrstate);                            */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*  Output:    xrstate (State of chosen PRNG)                                 */
/*  Returns:   Pseudorandom number of type ULONG                              */
/*                                                                            */
/******************************************************************************/
ULONG __FLINT_API
lRand_l (STATEPRNG *xrstate)
{
  ULONG r;

  switch (xrstate->Generator)
    {
      case FLINT_RNDBBS:
        r = lRandBBS_l (&xrstate->StateBBS);
        break;
      case FLINT_RNDRMDSHA1:
        r = lRandRMDSHA1_l (&xrstate->StateRMDSHA1);
        break;
      case FLINT_RNDAES:
        r = lRandAES_l (&xrstate->StateAES);
        break;
      case FLINT_RND64:
        r = usrand64_l ();
        break;
      default:
        r = usrand64_l ();
    }

  return r;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type CLINT              */
/*             (Preceding Initialization by InitRand_l() required)            */
/*  Syntax:    int Rand_l (CLINT r_l, STATEPRNG *xrstate, int l);             */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*             l (Number of binary digits of random number to generated)      */
/*  Output:    r_l (Pseudorandom number)                                      */
/*             xrstate (State of chosen PRNG)                                 */
/*  Returns:   E_CLINT_OK  if everything is OK                                */
/*             E_CLINT_RIN if requested generator is not initialized or       */
/*                         invalid choice                                     */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
Rand_l (CLINT r_l, STATEPRNG *xrstate, int l)
{
  int error = E_CLINT_OK;

  switch (xrstate->Generator)
    {
      case FLINT_RNDBBS:
        error = RandBBS_l (r_l, &xrstate->StateBBS, MIN (l, (int)CLINTMAXBIT));
        break;
      case FLINT_RNDAES:
        error = RandAES_l (r_l, &xrstate->StateAES, MIN (l, (int)CLINTMAXBIT));
        break;
      case FLINT_RNDRMDSHA1:
        error = RandRMDSHA1_l (r_l, &xrstate->StateRMDSHA1, MIN (l, (int)CLINTMAXBIT));
        break;
      case FLINT_RND64:
        rand_l (r_l, MIN (l, (int)CLINTMAXBIT));
        break;
      default:
        rand_l (r_l, MIN (l, (int)CLINTMAXBIT));
        error = E_CLINT_RIN;
    }

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Purging of the internal state of a chosen PRNG                 */
/*  Syntax:    int PurgeRand_l (STATEPRNG *xrstate);                          */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*  Output:    xrstate (Purged internal state of PRNG)                        */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RNG if choice of generator in xrstate is invalid       */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
PurgeRand_l (STATEPRNG *xrstate)
{
  int error = E_CLINT_OK;

  switch (xrstate->Generator)
    {
      case FLINT_RNDBBS:
        PurgeRandBBS_l (&xrstate->StateBBS);
        break;
      case FLINT_RNDAES:
        PurgeRandAES_l (&xrstate->StateAES);
        break;
      case FLINT_RNDRMDSHA1:
        PurgeRandRMDSHA1_l (&xrstate->StateRMDSHA1);
        break;
      case FLINT_RND64:
        break;
      default:
        error = E_CLINT_RNG;
    }
 
  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a pseudorandom number of type CLINT              */
/*             w/ rmin_l <= r_l <= rmax_l, using one of the FLINT/C PRNG,     */
/*             preceeding initialization required!                            */
/*  Syntax:    int RandMinMax_l (CLINT r_l, STATEPRNG *xrstate, CLINT rmin_l, */
/*                                                        CLINT rmax_l);      */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*             rmin_l (lower bound for r_l)                                   */
/*             rmax_l (upper bound for r_l)                                   */
/*  Output:    r_l (pseudorandom number)                                      */
/*             xrstate (State of chosen PRNG)                                 */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RGE if rmin_l > rmax_l                                 */
/*             E_CLINT_RNG if choice of generator in xrstate is invalid       */
/*             E_CLINT_RIN if PRNG is not initialized                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
RandMinMax_l (CLINT r_l, STATEPRNG *xrstate, CLINT rmin_l, CLINT rmax_l)
{
  CLINT t_l;
  int error = E_CLINT_OK;
  USHORT l = (ld_l (rmin_l) + ld_l (rmax_l)) >> 1;

  /* Plausibility: rmin_l <= rmax_l? */
  if (GT_L (rmin_l, rmax_l))
    {
      return E_CLINT_RGE;
    }

  sub_l (rmax_l, rmin_l, t_l);
  inc_l (t_l);

  switch (xrstate->Generator)
    {
      case FLINT_RNDAES:
        error = RandAES_l (r_l, &xrstate->StateAES, MIN (l, (int)CLINTMAXBIT));
        break;
      case FLINT_RNDRMDSHA1:
        error = RandRMDSHA1_l (r_l, &xrstate->StateRMDSHA1, MIN (l, (int)CLINTMAXBIT));
        break;
      case FLINT_RNDBBS:
        error = RandBBS_l (r_l, &xrstate->StateBBS, MIN (l, (int)CLINTMAXBIT));
        break;
      case FLINT_RND64:
        rand_l (r_l, MIN (l, (int)CLINTMAXBIT));
        /*      error = rand_l (r_l, MIN (l, (int)CLINTMAXBIT)); */
        break;
      default:
        return E_CLINT_RNG;
    }

  if (E_CLINT_OK != error)
    {
      return error;
    }

  if (LT_L (r_l, rmin_l))
    {
      add_l (r_l, rmin_l, r_l);
    }

  if (GT_L (r_l, rmax_l))
    {
      mod_l (r_l, t_l, r_l);
      add_l (r_l, rmin_l, r_l);
    }

  Assert (GE_L (r_l, rmin_l) && LE_L (r_l, rmax_l));

#ifdef FLINT_SECURE
  /* Purging of variables */
  local_memset (t_l, 0, sizeof (t_l));
  local_memset (&l, 0, sizeof (l));
#endif

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a random prime p_l of type CLINT                 */
/*             w/ 2^(l-1) <= p_l < 2^l, using one of the FLINT/C PRNG,        */
/*             preceeding initialization required!                            */
/*  Syntax:    int FindPrime_l (CLINT p_l, STATEPRNG *xrstate, USHORT l);     */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*             l (number of binary digits of p_l)                             */
/*  Output:    p_l (random prime, determined with probabilistic MR-Test)      */
/*             xrstate (State of chosen PRNG)                                 */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RGE if l = 0                                           */
/*             E_CLINT_RNG if choice of generator in xrstate is invalid       */
/*             E_CLINT_RIN if PRNG is not initialized                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
FindPrime_l (CLINT p_l, STATEPRNG *xrstate, USHORT l)
{
  return (FindPrimeGcd_l (p_l, xrstate, l, one_l));
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a random prime p_l of type CLINT                 */
/*             w/ 2^(l-1) <= p_l < 2^l and ggT (p_l - 1, f_l) = 1, using one  */
/*             of the FLINT/C PRNG, preceeding initialization required!       */
/*  Syntax:    int FindPrimeGcd_l (CLINT p_l, STATEPRNG *xrstate, USHORT l,   */
/*                                                         CLINT f_l);        */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*             l (number of binary digits of p_l)                             */
/*             f_l (number coprime to p_l - 1)                                */
/*  Output:    p_l (random prime, determined with probabilistic MR-Test)      */
/*             xrstate (State of chosen PRNG)                                 */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RGE if l = 0 or f_l is even                            */
/*             E_CLINT_RNG if choice of generator in xrstate is invalid       */
/*             E_CLINT_RIN if PRNG is not initialized                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
FindPrimeGcd_l (CLINT p_l, STATEPRNG *xrstate, USHORT l, CLINT f_l)
{
  CLINT rmin_l;
  clint rmax_l[CLINTMAXSHORT + 1];
  int error;
  if (0 == l)
    {
      return E_CLINT_RGE;
    }

  SETZERO_L (rmin_l);
  SETZERO_L (rmax_l);
  setbit_l (rmin_l, l - 1);
  setbit_l (rmax_l, l);
  dec_l (rmax_l);
  error = FindPrimeMinMaxGcd_l (p_l, xrstate, rmin_l, rmax_l, f_l);

#ifdef FLINT_SECURE
  /* Purging of variables */
  local_memset (rmin_l, 0, sizeof (rmin_l));
  local_memset (rmax_l, 0, sizeof (rmax_l));
#endif

  return error;
}


/******************************************************************************/
/*                                                                            */
/*  Function:  Generation of a random prime p_l of type CLINT                 */
/*             w/ rmin_l <= p_l <= rmax_l und ggT (p_l - 1, f_l) = 1, using   */
/*             one of the FLINT/C PRNG, preceeding initialization required!   */
/*  Syntax:    int FindPrimeMinMaxGcd_l (CLINT p_l, STATEPRNG *xrstate,       */
/*                                    CLINT rmin_l, CLINT rmax_l, CLINT f_l); */
/*  Input:     xrstate (Choice and initialized state of PRNG)                 */
/*             rmin_l (lower bound for p_l)                                   */
/*             rmax_l (upper bound for p_l)                                   */
/*             f_l (number coprime to p_l - 1)                                */
/*  Output:    p_l (random prime, determined with probabilistic MR-Test)      */
/*             xrstate (State of chosen PRNG)                                 */
/*  Returns:   E_CLINT_OK if everything is OK                                 */
/*             E_CLINT_RGE if rmin_l > rmax_l or f_l is even or prime that    */
/*                         meets the conditions can not be found              */
/*             E_CLINT_RNG if choice of generator in xrstate is invalid       */
/*             E_CLINT_RIN if PRNG is not initialized                         */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
FindPrimeMinMaxGcd_l (CLINT p_l, STATEPRNG *xrstate, CLINT rmin_l, 
                                                     CLINT rmax_l, CLINT f_l)
{
  CLINT t_l, rmin1_l, g_l;
  CLINT Pi_rmin_l, Pi_rmax_l, NoofCandidates_l, junk_l;
  int error;

  /* 0 < f_l has to be uneven */
  if (ISEVEN_L (f_l))
    {
      return E_CLINT_RGE;
    }

  udiv_l (rmin_l, (USHORT)ld_l (rmin_l), Pi_rmin_l, junk_l);
  udiv_l (rmax_l, (USHORT)ld_l (rmax_l), Pi_rmax_l, junk_l);
  sub_l (Pi_rmax_l, Pi_rmin_l, NoofCandidates_l);

  /* rmin_l <- ceil ((rmin_l - 1)/2) */
  dec_l (rmin_l);
  div_l (rmin_l, two_l, rmin_l, junk_l);
  if (GTZ_L (junk_l))
    {
      inc_l (rmin_l);
    }  

  /* rmax_l <- floor ((rmax_l - 1)/2) */
  dec_l (rmax_l);
  shr_l (rmax_l);

  do
    {
      /* Test if estimated number of candidates is counted back to zero */
      if (EQZ_L (NoofCandidates_l))
	{
          return (E_CLINT_RGE);
	}

      if (E_CLINT_OK != (error = RandMinMax_l (p_l, xrstate, rmin_l, rmax_l)))
        {
          return error;
        }

      /* p_l <- 2*p_l + 1 hence p_l is odd */
      shl_l (p_l);
      inc_l (p_l);

      cpy_l (rmin1_l, p_l);
      dec_l (rmin1_l);
      gcd_l (rmin1_l, f_l, g_l);

      dec_l (NoofCandidates_l);
    }
  while (!(EQONE_L (g_l) && ISPRIME_L (p_l)));


#ifdef FLINT_SECURE
  /* Purging of variables */
  local_memset (t_l, 0, sizeof (t_l));
  local_memset (rmin1_l, 0, sizeof (rmin1_l));
  local_memset (g_l, 0, sizeof (g_l));
#endif

  return error;
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
