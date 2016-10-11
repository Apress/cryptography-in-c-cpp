//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module rsakey.h         Revision: 09.12.2002                               */
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

#ifndef __RSAKEYH__
#define __RSAKEYH__             // rsakey.h is #included

#include "flintpp.h"
#include "ripemd.h"
#include "sha1.h"

#define BLOCKTYPE_SIGN 01
#define BLOCKTYPE_ENCR 02


typedef struct
{
    LINT pubexp, prvexp, mod, p, q, ep, eq, r;
    USHORT bitlen_mod;  // Binary length of modulus
    USHORT bytelen_mod; // Length of modulus in bytes
} KEYSTRUCT;


typedef struct
{
    LINT pubexp, mod;
    USHORT bitlen_mod;  // Binary length of modulus
    USHORT bytelen_mod; // Length of modulus in bytes
} PKEYSTRUCT;


class RSAkey
{
  public:
    inline RSAkey (void) {};
    RSAkey (int);
    RSAkey (int, STATEPRNG&, const LINT& = 1);
    PKEYSTRUCT export_public (void) const;
    UCHAR* decrypt (const LINT&, int*);
    LINT sign (const UCHAR* ,int);
    void purge (void);
    RSAkey& operator= (const RSAkey&);

    friend int operator== (const RSAkey&, const RSAkey&);
    friend int operator!= (const RSAkey&, const RSAkey&);
    friend fstream& operator<< (fstream&, const RSAkey&);
    friend fstream& operator>> (fstream&, RSAkey&);

  private:
    KEYSTRUCT key;

    // Auxiliary functions
    int makekey (int, STATEPRNG&, const LINT& = 1);
    int testkey (STATEPRNG&);
    LINT fastdecrypt (const LINT&);
};


class RSApub
{
  public:
    inline RSApub (void) {};
    RSApub (const RSAkey&);
    LINT crypt (const UCHAR*, int, STATEPRNG&);
    int verify (const UCHAR*, int, const LINT&);
    void purge (void);
    RSApub& operator= (const RSApub&);

    friend int operator== (const RSApub&, const RSApub&);
    friend int operator!= (const RSApub&, const RSApub&);
    friend fstream& operator<< (fstream&, const RSApub&);
    friend fstream& operator>> (fstream&, RSApub&);

  private:
    PKEYSTRUCT pkey;
};


///////////////////////////////////////////////////////////////////////////////
// Auxiliary functions external to RSA-classes

// Function for PKCS#1 formatting of encryption blocks
UCHAR* format_pkcs1 (UCHAR*, int, UCHAR, const UCHAR*, int, STATEPRNG&);


// Parser for decrypted Encryption Block, PKCS#1-formatted
int parse_pkcs1 (UCHAR*&, int*, UCHAR*, int LenKeyBytes);

#endif // __RSAKEYH__







