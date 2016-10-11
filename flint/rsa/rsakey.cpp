//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module rsakey.cpp       Revision: 19.04.2005                               */
//                                                                            */
//  Copyright (C) 1998-2005 by Michael Welschenbach                           */
//  Copyright (C) 1998-2005 by Springer-Verlag Berlin, Heidelberg             */
//  Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA                      */
//  Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland                      */
//  Copyright (C) 2002-2005 by PHEI, P.R.China                                */
//  Copyright (C) 2002-2005 by InfoBook, Korea                                */
//  Copyright (C) 2002-2005 by Triumph Publishing, Russia                     */
//                                                                            */
//  All Rights Reserved                                                       */
//                                                                            */
//  The software may be used for noncommercial purposes and may be altered,   */
//  as long as the following conditions are accepted without any              */
//  qualification:                                                            */
//                                                                            */
//  (1) All changes to the sources must be identified in such a way that the  */
//      changed software cannot be misinterpreted as the original software.   */
//                                                                            */
//  (2) The statements of copyright may not be removed or altered.            */
//                                                                            */
//  (3) The following DISCLAIMER is accepted:                                 */
//                                                                            */
//  DISCLAIMER:                                                               */
//                                                                            */
//  There is no warranty for the software contained in this distribution, to  */
//  the extent permitted by applicable law. The copyright holders provide the */
//  software `as is' without warranty of any kind, either expressed or        */
//  implied, including, but not limited to, the implied warranty of fitness   */
//  for a particular purpose. The entire risk as to the quality and           */
//  performance of the program is with you.                                   */
//                                                                            */
//  In no event unless required by applicable law or agreed to in writing     */
//  will the copyright holders, or any of the individual authors named in     */
//  the source files, be liable to you for damages, including any general,    */
//  special, incidental or consequential damages arising out of any use of    */
//  the software or out of inability to use the software (including but not   */
//  limited to any financial losses, loss of data or data being rendered      */
//  inaccurate or losses sustained by you or by third parties as a result of  */
//  a failure of the software to operate with any other programs), even if    */
//  such holder or other party has been advised of the possibility of such    */
//  damages.                                                                  */
//                                                                            */
//*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "rsakey.h"

#ifdef FLINT_TEST
static void cout_mess (const char*, int, const char*);
#endif


// Member functions of class RSAkey

// Constructor 1
RSAkey::RSAkey (int bitlen)
{
  STATEPRNG xrstate;
  int done;
  InitRand (xrstate, "", 0, 200, FLINT_RNDRMDSHA1);

  do
    {
      done = RSAkey::makekey (bitlen, xrstate);
    }
  while (!done);

  PurgeRand (xrstate);
}


// Constructor 2 with optional public exponent PubExp.
// The constructor generates RSA keys of distinct binary length and optional
// given public exponent (must be odd). In case that PubExp == 1 or omitted, 
// a random exponent is created. If the given exponent is even, an error 
// condition is generated, that can be handled by try() and catch() if 
// exceptions are enabled.

RSAkey::RSAkey (int bitlen, STATEPRNG& xrstate, const LINT& PubExp)
{
  int done;

  do
    {
      done = RSAkey::makekey (bitlen, xrstate, PubExp);
    }
  while (!done);
}


// Export public key
PKEYSTRUCT RSAkey::export_public (void) const
{
  PKEYSTRUCT pktmp;

  pktmp.pubexp = key.pubexp;
  pktmp.mod = key.mod;
  pktmp.bitlen_mod = key.bitlen_mod;
  pktmp.bytelen_mod = key.bytelen_mod;

  return pktmp;
}


// Decryption
UCHAR* RSAkey::decrypt (const LINT& Ciph, int* LenEB)
{
  UCHAR* EB = lint2byte (fastdecrypt (Ciph), LenEB);
  UCHAR* Mess = NULL;

#ifdef FLINT_TEST
  cout_mess ((const char*)EB, key.bytelen_mod - 1, "Encryption Block after decryption");
#endif

  // Parsing decrypted Encryption Block, PKCS#1-formatted
  if (2 != parse_pkcs1 (Mess, LenEB, EB, key.bytelen_mod)) 
    {
      return (UCHAR*)NULL; // False block type or format of encryption block    
    }
  else
    {
      return Mess; // Return pointer to decrypted message
    }
}


// Sign
// Returns 0 if message too long
LINT RSAkey::sign (const UCHAR* Mess, int LenMess)
{

  STATEPRNG junk;

#ifdef FLINT_TEST
  cout << "Length of modulus = " << key.bytelen_mod << " byte." << endl;
#endif
  int LenEncryptionBlock = key.bytelen_mod;
  UCHAR HashRes[RMDVER>>3];
  UCHAR* EncryptionBlock = new UCHAR[LenEncryptionBlock];

  sha1_l (HashRes, (UCHAR*)Mess, (ULONG)LenMess);
  if (NULL == format_pkcs1 (EncryptionBlock, LenEncryptionBlock,
                            BLOCKTYPE_SIGN, HashRes, RMDVER >> 3, junk))
    {
      delete [] EncryptionBlock;
      return LINT (0);             // Error: Message too long
    }

#ifdef FLINT_TEST
  cout_mess ((const char*)EncryptionBlock, LenEncryptionBlock, "Encryption Block");
#endif

  // Convert Encryption Block into LINT value (Constructor 3)
  LINT m = LINT (EncryptionBlock, LenEncryptionBlock);
  delete [] EncryptionBlock;

  return fastdecrypt (m);
}


// Key deletion
void RSAkey::purge (void)
{
  key.pubexp.purge ();
  key.prvexp.purge ();
  key.mod.purge ();
  key.p.purge ();
  key.q.purge ();
  key.ep.purge ();
  key.eq.purge ();
  key.r.purge ();
  key.bitlen_mod = 0;
  key.bytelen_mod = 0;
}



// RSAkey auxiliary functions


// Generation of RSA keys acc. to IEEE P1363, Annex A.
// A public exponent may be given in PubExp. If PubExp is omitted or
// PubExp == 1 a public exponent of half the modulus length
// is choosen at random.

int RSAkey::makekey (int length, STATEPRNG& xrstate, const LINT& PubExp)
{
  // Generate prime p
  // 2^(m - r - 1) <= p < 2^(m - r), with
  // m = floor((length + 1)/2) and r randomly chosen from intervall 2 <= r < 15
  const USHORT m = (((const USHORT)length + 1) >> 1) - 2 - sRand_l (&xrstate) % 13;

  key.p = FindPrime (m, PubExp, xrstate);

  // Determine intervall qmin and qmax for prime q
  // Set qmin = floor ((2^(length - 1))/p + 1)
  LINT qmin = LINT(0).setbit (length - 1)/key.p + 1;
  // Set qmax = floor ((2^length - 1)/p)
  LINT qmax = (((LINT(0).setbit (length - 1) - 1) << 1) + 1)/key.p; 

  // Generate prime q > p
  // qmin <= q <= qmax
  key.q = FindPrime (qmin, qmax, PubExp, xrstate);

  // Generate modulus p*q
  // 2^(length - 1) <= p*q < 2^length
  key.mod = key.p * key.q;

  // Calculate Euler's Phi-function
  LINT phi_n = key.mod - key.p - key.q + 1;

  // Generate public key, if not defined in PubExp, of half the number of 
  // the modulus digits
  if (1 == PubExp)
    {
      key.pubexp = RandLINT (length/2, xrstate) | 1; 
      while (gcd (key.pubexp, phi_n) != 1)
        {
          ++key.pubexp;
          ++key.pubexp;
        }
    }
  else
    {
      key.pubexp = PubExp;
    }

  // Generate secret key
  key.prvexp = key.pubexp.inv (phi_n);

  // Generate secret key components for fast decryption
  // acc. to Chinese Remainder Theorem
  key.ep = key.prvexp % (key.p - 1);
  key.eq = key.prvexp % (key.q - 1);
  key.r = inv (key.p, key.q);     // r = p^(-1) mod q, as an alternative 
                                  // r = q^(-1) mod p is possible

  // Store keylength
  key.bitlen_mod = ld (key.mod);
  key.bytelen_mod = key.bitlen_mod >> 3;
  if ((key.bitlen_mod % 8) > 0)
    {
      ++key.bytelen_mod;
    }

#ifdef FLINT_TEST
  cout << "Modulus = " << key.mod << endl;
  cout << "Public exponent e = " << key.pubexp << endl;
  cout << "Private exponent d = " << key.prvexp << endl;
  cout << "p = " << key.p << endl;
  cout << "q = " << key.q << endl;
  cout << "d mod p-1 = " << key.ep << endl;
  cout << "d mod q-1 = " << key.eq << endl;
  cout << "Inverse of p mod q = " << key.r << endl;
#endif // FLINT_TEST

  return testkey (xrstate);
}


// Test keys
int RSAkey::testkey (STATEPRNG& xrstate)
{
  LINT mess = RandLINT (ld (key.mod) >> 1, xrstate);
  return (mess == fastdecrypt (mexpkm (mess, key.pubexp, key.mod)));
}


// Fast RSA-decryption acc. to Chinese Remainder Theorem (CRT)
LINT RSAkey::fastdecrypt (const LINT& mess)
{
  LINT m, w;                           // If alternative CRT key component
  m = mexpkm (mess, key.ep, key.p);    // r = q^(-1) mod p 
  w = mexpkm (mess, key.eq, key.q);    // is in use:
  w.msub (m, key.q);                   // m.msub (w, key.p);
  w = w.mmul (key.r, key.q) * key.p;   // m = m.mmul (key.r, key.p) * key.q;
  return (w + m);
}


// Operators =, ==, != in class RSAkey

RSAkey& RSAkey::operator= (const RSAkey &k)
{
  if ((&k != this)) // Don't copy object into itself
    {
      key.pubexp      = k.key.pubexp;
      key.prvexp      = k.key.prvexp;
      key.mod         = k.key.mod;
      key.p           = k.key.p;
      key.q           = k.key.q;
      key.ep          = k.key.ep;
      key.eq          = k.key.eq;
      key.r           = k.key.r;
      key.bitlen_mod  = k.key.bitlen_mod;
      key.bytelen_mod = k.key.bytelen_mod;
    }
  return *this;
}


int operator== (const RSAkey& k1, const RSAkey& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 1;
    }

  return (k1.key.pubexp      == k2.key.pubexp      &&
          k1.key.prvexp      == k2.key.prvexp      &&
          k1.key.mod         == k2.key.mod         &&
          k1.key.p           == k2.key.p           &&
          k1.key.q           == k2.key.q           &&
          k1.key.ep          == k2.key.ep          &&
          k1.key.eq          == k2.key.eq          &&
          k1.key.r           == k2.key.r           &&
          k1.key.bitlen_mod  == k2.key.bitlen_mod  &&
          k1.key.bytelen_mod == k2.key.bytelen_mod);

  // Operator == returns 1 if k1 == k2, 0 else
}


int operator!= (const RSAkey& k1, const RSAkey& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 0;
    }

  return (k1.key.pubexp      != k2.key.pubexp      ||
          k1.key.prvexp      != k2.key.prvexp      ||
          k1.key.mod         != k2.key.mod         ||
          k1.key.p           != k2.key.p           ||
          k1.key.q           != k2.key.q           ||
          k1.key.ep          != k2.key.ep          ||
          k1.key.eq          != k2.key.eq          ||
          k1.key.r           != k2.key.r           ||
          k1.key.bitlen_mod  != k2.key.bitlen_mod  ||
          k1.key.bytelen_mod != k2.key.bytelen_mod);

  // Operator != returns 1 if k1 != k2, 0 else
}


fstream& operator<< (fstream& s, const RSAkey& k)
{
  s << k.key.pubexp << k.key.prvexp << k.key.mod << k.key.p << k.key.q
    << k.key.ep << k.key.eq << k.key.r;

  write_ind_ushort (s, k.key.bitlen_mod);
  write_ind_ushort (s, k.key.bytelen_mod);

  return s;
}


fstream& operator>> (fstream& s, RSAkey& k)
{
  s >> k.key.pubexp >> k.key.prvexp >> k.key.mod >> k.key.p >> k.key.q
    >> k.key.ep >> k.key.eq >> k.key.r;

  read_ind_ushort (s, &k.key.bitlen_mod);
  read_ind_ushort (s, &k.key.bytelen_mod);

  return s;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Member functions of class RSApub

// Constructor
RSApub::RSApub (const RSAkey& k)
{
  pkey = k.export_public ();
}


// RSA-encryption
LINT RSApub::crypt (const UCHAR* Mess, int LenMess, STATEPRNG& xrstate)
{
  int LenEncryptionBlock = pkey.bytelen_mod;
  UCHAR* EncryptionBlock = new UCHAR[LenEncryptionBlock];

  // Format Encryption Block acc. to PKCS#1
  if (NULL == format_pkcs1 (EncryptionBlock,
                            LenEncryptionBlock,
                            BLOCKTYPE_ENCR,
                            Mess,
                            (ULONG)LenMess, 
                            xrstate))
    {
      delete [] EncryptionBlock;
      return LINT (0);             // Error: Message too long
    }

#ifdef FLINT_TEST
  cout_mess ((const char*)EncryptionBlock, LenEncryptionBlock, "Encryption Block before encryption");
#endif

  // Convert Encryption Block into LINT-value (Constructor 3)
  LINT m = LINT (EncryptionBlock, LenEncryptionBlock);
  delete [] EncryptionBlock;

  return (mexpkm (m, pkey.pubexp, pkey.mod));
}


// Verify RSA-signature
int RSApub::verify (const UCHAR* Mess, int LenMess, const LINT& Signature)
{
  int length, BlockType, verification = 0;
  UCHAR H1[RMDVER>>3];
  UCHAR* H2 = NULL;
  UCHAR* EB = lint2byte (mexpkm (Signature, pkey.pubexp, pkey.mod), &length);

  sha1_l (H1, (UCHAR*)Mess, (ULONG)LenMess);

#ifdef FLINT_TEST
  cout_mess ((const char*)H2, pkey.bytelen_mod - 1, "Encryption Block when signature is being verified");
#endif

  // Read data from decrypted Encryption Block, PKCS#1-formatted
  BlockType = parse_pkcs1 (H2, &length, EB, pkey.bytelen_mod);

  if ((BlockType == 0 || BlockType == 1) && 
      (H2 != NULL) && (length == (RMDVER >> 3)))
    {
      verification = !memcmp ((char *)H1, (char*)H2, RMDVER >> 3);
    }

#ifdef FLINT_TEST
  cout_mess ((const char*)H2, length, "Hash-Wert when signature is being verified");
  cout << "Verification = " << verification << endl;
#endif

  return verification;
}


// Delete public key
void RSApub::purge (void)
{
  pkey.pubexp.purge ();
  pkey.mod.purge ();
  pkey.bitlen_mod = 0;
  pkey.bytelen_mod = 0;
}


// Operators =, ==, != of class RSApub

RSApub& RSApub::operator= (const RSApub &k)
{
  if ((&k != this)) // Don't copy object into itself
    {
      pkey.pubexp      = k.pkey.pubexp;
      pkey.mod         = k.pkey.mod;
      pkey.bitlen_mod  = k.pkey.bitlen_mod;
      pkey.bytelen_mod = k.pkey.bytelen_mod;
    }
  return *this;
}


int operator== (const RSApub& k1, const RSApub& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 1;
    }

  return (k1.pkey.pubexp      == k2.pkey.pubexp      &&
          k1.pkey.mod         == k2.pkey.mod         &&
          k1.pkey.bitlen_mod  == k2.pkey.bitlen_mod  &&
          k1.pkey.bytelen_mod == k2.pkey.bytelen_mod);

  // Operator == returns 1 if k1 == k2, 0 else
}


int operator!= (const RSApub& k1, const RSApub& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 0;
    }

  return (k1.pkey.pubexp      != k2.pkey.pubexp      ||
          k1.pkey.mod         != k2.pkey.mod         ||
          k1.pkey.bitlen_mod  != k2.pkey.bitlen_mod  ||
          k1.pkey.bytelen_mod != k2.pkey.bytelen_mod);

  // Operator != returns 1 if k1 != k2, 0 else
}


fstream& operator<< (fstream& s, const RSApub& k)
{
  s << k.pkey.pubexp << k.pkey.mod;

  write_ind_ushort (s, k.pkey.bitlen_mod);
  write_ind_ushort (s, k.pkey.bytelen_mod);

  return s;
}


fstream& operator>> (fstream& s, RSApub& k)
{
  s >> k.pkey.pubexp >> k.pkey.mod;

  read_ind_ushort (s, &k.pkey.bitlen_mod);
  read_ind_ushort (s, &k.pkey.bytelen_mod);

//s >> k.pkey.bitlen_mod >> k.pkey.bytelen_mod;

  return s;
}


// Format Encryption Blocks (EB) acc. to PKCS#1
// EB    = 00||BT||PS1...PSl||00||DATA1...DATAk
// BT    = Block Type public key operation:
//             01 private key operation (signing)
//             02 public key operation (encryption)
// PSi   = BT 01: Value FF (hex), 8 Byte at least
//         BT 02: Random values, not zero, 8 bytes at least
// DATAi = Data bytes, k <= (byte length of modulus) - 11, pointed to by
//         parameter data, length k in parameter LenData
// Length of modulus in bytes is passed in LenMod
// Parameter EB points to buffer for Encryption Block
// buffer  must be allocated by calling function, length of buffer at least
// byte length of modulus

UCHAR* format_pkcs1 (UCHAR* EB, int LenMod, UCHAR BlockType, const UCHAR* data, int LenData, STATEPRNG& xrstate)
{
  // Calculate length lps of padding block
  int lps = LenMod - 3 - LenData;

  if (lps < 8)                     // PKCS#1: Length padding block >= 8
    {
      return NULL;                 // NULL pointer indicates error status
    }

  UCHAR* hlp = EB;
  *hlp++ = 0x00;                   // Leading octet 0x00                

  switch (BlockType)
    {
      case BLOCKTYPE_SIGN: 
        *hlp++ = 0x01;             // Block Type 01: Private Key Operation
        while (lps-- > 0)
          {
            *hlp++ = 0xff;         // 8 <= lps bytes 0xFF
          }
        break;
      case BLOCKTYPE_ENCR: 
        *hlp++ = 0x02;             // Block Type 02: Public Key Operation
        while (lps-- > 0)
          {
            do
              {  
                *hlp = bRand_l (&xrstate);   
              }                    // 8 <= lps random bytes not zero
            while (*hlp == 0);
            ++hlp;
          }
        break;
      default:  
        return NULL;               // Error: Undefined Block Type in BlockType
    }         

  *hlp++ = 0x00;                   // Separating octet

  for (int l = 1; l <= LenData; l++)
    {
      *hlp++ = *data++;
    }

  return (UCHAR*)EB;
}


// Parser for decrypted Encryption Block, PKCS#1-formatted
// Pointer to data is stored in PayLoad
// Length of data (number of bytes) is stored in LenData
// Returns determined Block Type
// -1 is returned to indicate format error status

int parse_pkcs1 (UCHAR*& PayLoad, int* LenData, UCHAR* EB, int LenMod)
{
  PayLoad = EB;
  UCHAR BlockType;
  int error = 0, noofpadc = 0;
  
  // Check implicitly for the leading 0x00-octet. The leading 0x00-octet as 
  // leading zero is not reconstructed. However, would the octet be different 
  // from 0x00, it would be reconstructed resulting in a length of the 
  // reconstructed encryption block equal to the length of the modulus. This 
  // error condition is checked here.

  // Check Block Type explicitly: 00, 01, 02 are valid
  if ((*LenData != LenMod - 1) || (BlockType = *EB) > 2)
    {
      return -1;
    }

  ++PayLoad;
  --*LenData;
  switch (BlockType)
    {
      case 0:
      case 1:                 // Block Type 00 or 01: Private Key Operation
        while (*PayLoad != 0 && *LenData > 0)  // Analyze padding string PS
          {
            if (*PayLoad != 0xff)
              {
                error = 1;    // Check padding == 0xFF for Block Type 01
                break;        // Set errorflag error to 1 if byte != 0xFF
              }
            ++PayLoad;
            --*LenData;
            ++noofpadc;
          };
        break;
      case 2:                 // Block Type 02: Public Key Operation
        while (*PayLoad != 0 && *LenData > 0)  // Search end of padding string PS
          {
            ++PayLoad;
            --*LenData;
            ++noofpadc;
          };
        break;
      default:
        PayLoad = (UCHAR*)NULL;  
        return -1;            // Invalid Block Type
    }

  if (noofpadc < 8 || error || *LenData == 0)
    {                         
      PayLoad = (UCHAR*)NULL; // Summarize result of padding check
      return -1;              // Error status indicated by -1
    }
  else
    {
      ++PayLoad;              // Skip separation byte 00
      --*LenData;             // Byte length of payload in LenData
    }

  return BlockType;           //Return Block Type
}


#ifdef FLINT_TEST
// Output of message as Hex-value
static void cout_mess (const char* mess, int len, const char* titel)
{
  cout << titel << " = " << hex
       << setfill ('0') << resetiosflags (ios::showbase);

  if (mess > NULL)
    {
      for  (int i = 0; i < len; i++)
        {
          cout << setw (2) << (((unsigned)*mess++) & 0xff);
        }
    }

  cout << setfill (' ') << resetiosflags (ios::hex) << endl;
}
#endif

