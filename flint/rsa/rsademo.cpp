//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module rsademo.cpp      Revision: 29.11.2002                               */
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

#if defined FLINT_ANSI
#include <string>
#if !defined __WATCOMC__
using namespace std;
#endif // #!defined __WATCOMC__
#else
#include <string.h>
#endif // #defined FLINT_ANSI

#include "rsakey.h"

#define GENERATOR FLINT_RNDRMDSHA1

static void svkey (RSAkey&, RSApub&);
static void rdkey (RSAkey&, RSApub&);
static UCHAR* rndmess (const int, STATEPRNG&);
static void cout_bytes (const char*, const int, const char* const);

// Keylength in bits
#define KEYLENGTH 4096

// Length of test message for signing in bytes
#define MESSLNGTH 1000

// Length of test message for encryption in bytes, maximum blocklength
// in bytes for PKCS#1-Format
#define BLCKLNGTH (((KEYLENGTH + 7) >> 3) - 11)

int main()
{
  int dlen;

  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  // Initialize PRNG 
  STATEPRNG xrstate;

  if (InitRand (xrstate, "", 0, 200, GENERATOR))
    {
      cout << "Info: Requested entropy could not be generated." << endl;
      cout << "      Please check what strategies for the generation of " << endl;
      cout << "      entropy are available with respect to the platform " << endl;
      cout << "      and development environment in use. A platform " << endl;
      cout << "      specific implementation of an entropy generation " << endl;
      cout << "      process can be integrated into the module entropy.c. " << endl;
    }

  // RSA keygeneration

  cout << "Generate RSA key of "<< KEYLENGTH << " bit ..." << flush;
  RSAkey my_key (KEYLENGTH, xrstate);
  cout << endl << " done." << endl;

  // Extract public key from RSAkey object
  RSApub my_public_key (my_key);
  cout << "RSA public key extracted." << endl;

  // Generate Messageblock Message_1
  const UCHAR* const mess1 = rndmess (BLCKLNGTH, xrstate);
  cout << "Random message generated." << endl;
  cout_bytes ((const char*)mess1, BLCKLNGTH, "Message_1");
  cout << endl;

  // Encrypt Message_1
  LINT cipher = my_public_key.crypt (mess1, BLCKLNGTH, xrstate);
  cout << "Message_1 encrypted." << endl;
  cout << "Ciphertext = " << cipher << endl;

  // Generate Messageblock Message_2
  const UCHAR* const mess2 = rndmess (MESSLNGTH, xrstate);
  cout << "Random Message generated." << endl;
  cout_bytes ((const char*)mess2, MESSLNGTH, "Message_2");
  cout << endl;

  // Hashing and signing of Message_2
  LINT signature = my_key.sign (mess2, MESSLNGTH);
  cout << "Message_2 signed." << endl;
  UCHAR HashRes[RMDVER>>3];
  ripemd160 (HashRes, (UCHAR*)mess2, MESSLNGTH);
  cout_bytes ((char *)HashRes, RMDVER >> 3, "Hashvalue of Message_2");
  cout << "Signatur = " << signature << endl;

  // Decryption and checking of ciphertext
  UCHAR* decrypted_mess = my_key.decrypt (cipher, &dlen);
  if (decrypted_mess == NULL) 
    cout << "Error in encryption/decryption operation!" << endl;
  else
    {
      if (dlen == BLCKLNGTH && !memcmp (mess1, decrypted_mess, BLCKLNGTH))
        cout << "Encryption/decryption operation O.K." << endl;
      else
        cout << "Error in encryption/decryption operation!" << endl;
    }

  // Checking of signature
  if (my_public_key.verify (mess2, MESSLNGTH, signature))
    cout << "Verification O.K." << endl;
  else
    cout << "Error in Signature!" << endl;


  // Write-/Read-Test

  RSAkey testk1;  // RSA key, not initialized
  RSApub testk2;  // RSA public key, not initialized

  // Store RSA key on mass storage
  svkey (my_key, my_public_key);

  // Read RSA key from mass storage
  rdkey (testk1, testk2);

  // Compare original and duplicate
  if (testk1 == my_key)
    cout << "Write-/Read-Test RSAkey O.K." << endl;
  else
    cout << "Error in Write-/Read-Test RSAkey!" << endl;

  if (testk2 == my_public_key)
    cout << "Write-/Read-Test RSApub O.K." << endl;
  else
    cout << "Error in Write-/Read-Test RSApub!" << endl;

  delete [] (UCHAR*)mess1;
  delete [] (UCHAR*)mess2;

  return 0;
}


// Auxiliary functions

// Writing to mass storage
static void svkey (RSAkey &prvk, RSApub &pubk)
{
  fstream fout1 ("rsakey.prv",ios::out | ios::binary);
  fout1 << prvk;
  fout1.close ();

  fstream fout2 ("rsakey.pub",ios::out | ios::binary);
  fout2 << pubk;
  fout2.close ();
}


// Reading from mass storage
static void rdkey (RSAkey &prvk, RSApub &pubk)
{
  fstream fin1 ("rsakey.prv",ios::in | ios::binary);
  fin1 >> prvk;
  fin1.close ();

  fstream fin2 ("rsakey.pub",ios::in | ios::binary);
  fin2 >> pubk;
  fin2.close ();
}


// Generation of random messages of len bytes
static UCHAR* rndmess (const int len, STATEPRNG& xrstate)
{
  UCHAR* mess = new UCHAR[sizeof(UCHAR) * (len + 1)];
  if (NULL == mess)
    {
      return NULL;
    }

  UCHAR* tmp = mess;
  for (int i = 0; i < len; i++)
    {
      *tmp++ = bRand_l (&xrstate);
    }
  *tmp = '\0';

  return mess;
}


// Output of messages as Hex-values
static void cout_bytes (const char* mess, const int len, const char* const titel)
{
  cout << titel << " = " << hex
       << setfill ('0') << resetiosflags (ios::showbase);

  for  (int i = 0; i < len; i++)
    {
      cout << setw (2) << (((unsigned)*mess++) & 0xff);
    }

  cout << setfill (' ') << resetiosflags (ios::hex) << endl;
}



