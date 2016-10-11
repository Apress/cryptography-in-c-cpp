//*****************************************************************************/
//                                                                            */
// Functions for arithmetic and number theory with large integers in C        */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module flintpp.h        Revision: 19.05.2003                               */
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
//
//  History
//
//  27.01.2002
//    Added member and friend functions lint2clint for export to type CLINT 
//  29.06.2002
//    Added parameter file to function panic and to LINT_Error class 
//    and subclasses
//  01.12.2002
//    Removed const in all function declarations and definitions 
//    for parameters that are passed by value 
//    Removed const in all return types that are passed by value except
//    for operators
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FLINTPPH__
#define __FLINTPPH__            // flintpp.h is #included

//lint -wlib(1)

// Test for gcc 3.2.x
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 2))
#define FLINTPP_ANSI
#endif

#if defined FLINTPP_ANSI
#include <limits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <new>
#include <algorithm>
#if !defined __WATCOMC__
using namespace std;
#endif // #!defined __WATCOMC__
#else
#include <limits.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <new.h>
#endif // #defined FLINTPP_ANSI

#ifndef __CPLUSPLUS__
#define __CPLUSPLUS__
#endif

#ifndef __cplusplus
#define __cplusplus
#endif

#define FLINTCPPHVMAJ   3 // Major version number of flintpp.cpp
#define FLINTCPPHVMIN   0 // Minor version number of flintpp.cpp
#define FLINTCOMPATMAJ  3 // Major version of flint.c required for flintpp.cpp
#define FLINTCOMPATMIN  0 // Minor version of flint.c required for flintpp.cpp

//lint -wlib(4)


// Include FLINT/C C-Library

#include "flint.h"
#include "random.h"


// Version control

#if ((FLINTCOMPATMIN > FLINT_VERMIN) || (FLINTCOMPATMAJ > FLINT_VERMAJ))
#error Versionsfehler: FLINTPP.H  not compatibel to FLINT.H
#endif


// LINT-Errors

enum LINT_ERRORS {
        E_LINT_OK     = 0,      // Everything O.K.
        E_LINT_EOF    = 0x0010, // File-IO-Error
        E_LINT_DBZ    = 0x0020, // Division by zero
        E_LINT_NHP    = 0x0040, // Heap-Error: new returned NULL-pointer
        E_LINT_OFL    = 0x0080, // Overflow in function/operator
        E_LINT_UFL    = 0x0100, // Underflow in function/operator
        E_LINT_VAL    = 0x0200, // Argument of function/operator not initialized
        E_LINT_INV    = 0x0200, // Argument of function/operator not initialized
        E_LINT_BOR    = 0x0400, // Base invalid
        E_LINT_MOD    = 0x0800, // Modulus even in mexp?m
        E_LINT_NPT    = 0x1000, // Argument is Null-pointer
        E_LINT_RIN    = 0x2000, // Random number generator not initialized
        E_LINT_ERR    = 0x8000  // root or chinrem has no solution,
                                // else: unspecific error
};


// LINT-Exceptions

class LINT_Error                        // Abstract base class
{
 public:
  const char* function, *module;
  int argno, lineno;
  virtual void debug_print (void) const = 0;  // Pure virtual function
  virtual ~LINT_Error() {function = 0; module = 0;};
};

class LINT_DivByZero : public LINT_Error      // Division by Zero
{
 public:
  LINT_DivByZero (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_File : public LINT_Error     // File-IO error
{
 public:
  LINT_File (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Init : public LINT_Error     // Argument not initialized
{
 public:
  LINT_Init (const char*, int, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Heap : public LINT_Error     // Heap-error in new
{
 public:
  LINT_Heap (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_OFL : public LINT_Error      // Overflow in function
{
 public:
  LINT_OFL (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_UFL : public LINT_Error      // Underflow in function
{
 public:
  LINT_UFL (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Base : public LINT_Error     // Base invalid
{
 public:
  LINT_Base (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Emod : public LINT_Error     // Modulus even in mexp?m
{
 public:
  LINT_Emod (const char*, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Nullptr : public LINT_Error  // Argument is NULL-pointer
{
 public:
  LINT_Nullptr (const char*, int, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Nonrand : public LINT_Error  // Call to non initialized PRNG
{
 public:
  LINT_Nonrand (const char*, int, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};

class LINT_Mystic : public LINT_Error   // Unknown error ;-)
{
 public:
  LINT_Mystic (const char*, int, int, const char* = "flintpp.cpp");
  void debug_print (void) const;
};


class LintInit
{
 public:
  LintInit (void);
};

// The constructor LintInit() sets the ios-internal value
// ios::iword (flagsindex) (after initialization of LINT::flagsindex) to the
// default values of the LINT-package. Thus the default mode for stream
// output of LINT-objects is defined. A calling program can change the output
// mode by calling LINT manipulators (cf. manipulators like
// LINT::SetLintFlags (LINT::flags)).


// Macros for Internationalization of class LINT

#define ggT            gcd
#define xggT           xgcd
#define kgV            lcm
#define chinrest       chinrem
#define zweianteil     twofact


// Declaration of class LINT

class LINT
{
 public:

  // LINT-FRIENDS

  friend LintInit::LintInit (void);

  // Overloaded operators, implemented as friend functions

  friend const LINT operator+ (const LINT&, const LINT&);
  friend const LINT operator- (const LINT&, const LINT&);
  friend const LINT operator* (const LINT&, const LINT&);
  friend const LINT operator/ (const LINT&, const LINT&);
  friend const LINT operator% (const LINT&, const LINT&);
  friend const LINT operator<< (const LINT&, int);
  friend const LINT operator>> (const LINT&, int);

  // Logical functions

  friend const int operator== (const LINT&, const LINT&);
  friend const int operator!= (const LINT&, const LINT&);
  friend const int operator> (const LINT&, const LINT&);
  friend const int operator< (const LINT&, const LINT&);
  friend const int operator<= (const LINT&, const LINT&);
  friend const int operator>= (const LINT&, const LINT&);

  // Boolean functions

  friend const LINT operator^ (const LINT&, const LINT&);
  friend const LINT operator| (const LINT&, const LINT&);
  friend const LINT operator& (const LINT&, const LINT&);

  // Arithmetic

  friend LINT add (const LINT&, const LINT&);
  friend LINT sub (const LINT&, const LINT&);
  friend LINT mul (const LINT&, const LINT&);
  friend LINT sqr (const LINT&);
  friend LINT divr (const LINT&, const LINT&, LINT&);
  friend LINT mod (const LINT&, const LINT&);
  friend LINT mod2 (const LINT&, USHORT);

  // Swapping

  friend void fswap (LINT&, LINT&);

  // Purging of LINT, overwriting with 0

  friend void purge (LINT&);

  // Modular arithmetic

  friend LINT madd (const LINT&, const LINT&, const LINT&);
  friend LINT msub (const LINT&, const LINT&, const LINT&);
  friend LINT mmul (const LINT&, const LINT&, const LINT&);
  friend LINT msqr (const LINT&, const LINT&);
  friend LINT mexp (const LINT&, const LINT&, const LINT&);
  friend LINT mexp (USHORT, const LINT&, const LINT&);
  friend LINT mexp (const LINT&, USHORT, const LINT&);
  friend LINT mexp5m (const LINT&, const LINT&, const LINT&);
  friend LINT mexpkm (const LINT&, const LINT&, const LINT&);
  friend LINT mexp2 (const LINT&, USHORT, const LINT&);
  friend LINT shift (const LINT&, int);

  // Number theoretic friend functions

  friend int isprime (const LINT&, int noofsmallprimes = 302, int iterations = 0);
  friend LINT issqr (const LINT&);
  friend unsigned int ld (const LINT&);
  friend LINT gcd (const LINT&, const LINT&);
  friend LINT xgcd (const LINT&, const LINT&, LINT&, int&, LINT&, int&);
  friend LINT inv (const LINT&, const LINT&);
  friend LINT lcm (const LINT&, const LINT&);
  friend int jacobi (const LINT&, const LINT&);
  friend LINT root (const LINT&);
  friend LINT root (const LINT&, const LINT&);
  friend LINT root (const LINT&, const LINT&, const LINT&);
  friend LINT primroot (unsigned int, LINT**);
  friend LINT chinrem (unsigned int, LINT**);
  friend int twofact (const LINT&, LINT&);
  friend int iseven (const LINT&);
  friend int isodd (const LINT&);
  friend LINT nextprime (const LINT&, const LINT&);
  friend int mequ (const LINT&, const LINT&, const LINT&);

  // Pseudorandom number generators, generation of pseudorandom primes
  // Functions in module randompp.cpp

  friend int InitRand (STATEPRNG& , const char*, int, int, int);

  friend void PurgeRand (STATEPRNG&);
  friend LINT RandLINT (int, STATEPRNG&);
  friend LINT RandLINT (const LINT&, const LINT&, STATEPRNG&);

  friend LINT FindPrime (USHORT, STATEPRNG&);
  friend LINT FindPrime (USHORT, const LINT&, STATEPRNG&);
  friend LINT FindPrime (const LINT&, const LINT&, const LINT&, STATEPRNG&);
  friend LINT ExtendPrime (USHORT, const LINT&, const LINT&, const LINT&, STATEPRNG&);
  friend LINT ExtendPrime (const LINT&, const LINT&, const LINT&, const LINT&, const LINT&, STATEPRNG&);

  friend LINT StrongPrime (USHORT, STATEPRNG&);
  friend LINT StrongPrime (USHORT, const LINT&, STATEPRNG&);
  friend LINT StrongPrime (USHORT, USHORT, USHORT, USHORT, const LINT&, STATEPRNG&);
  friend LINT StrongPrime (const LINT&, const LINT&, const LINT&, STATEPRNG&);
  friend LINT StrongPrime (const LINT& pmin, const LINT&, USHORT, USHORT, USHORT, const LINT&, STATEPRNG&);

  // Compatibility functions: Calls to Blum-Blum-Shub-Generator
  friend LINT randBBS (int);
  friend LINT randBBS (const LINT&, const LINT&);
  friend int seedBBS (const LINT&);

  // Compatibility functions: Calls to 64 bit linear congruential generator
  // Do not use this generator for cryptographical purposes!
  friend LINT randl (int);
  friend LINT randl (const LINT&, const LINT&);
  friend void seedl (const LINT&);

  // Conversion

  friend char* lint2str (const LINT&, USHORT, int = 0);
  friend UCHAR* lint2byte (const LINT&, int*);

  // Export as CLINT type

  friend clint* lint2clint (const LINT& ln);

  // Overloading of ostream insert operator << for LINT-objects

  friend ostream& operator<< (ostream& s, const LINT& ln);

  // Overloading of fstream/istream/ofstream insert operators >>,<<
  // for file-IO of LINT objects

  friend fstream& operator<< (fstream& s, const LINT& ln);
  friend ofstream& operator<< (ofstream& s, const LINT& ln);

  friend fstream& operator>> (fstream& s, LINT& ln);
  friend ifstream& operator>> (ifstream& s, LINT& ln);


  // LINT-MEMBERS

  // Constructors

  LINT (void);                  // Constructor  1
  LINT (const char*, int);      // Constructor  2
  LINT (const UCHAR*, int);     // Constructor  3
  LINT (const char*);           // Constructor  4
  LINT (const LINT&);           // Constructor  5  (Copy)
  LINT (signed int);            // Constructor  6
  LINT (signed long);           // Constructor  7
  LINT (unsigned char);         // Constructor  8
  LINT (unsigned short);        // Constructor  9
  LINT (unsigned int);          // Constructor 10
  LINT (unsigned long);         // Constructor 11
  LINT (const CLINT);           // Constructor 12

  //  Destructor

  ~LINT (void)
    {
#ifdef FLINT_SECURE
      purge_l (n_l);
#endif // FLINT_SECURE
      delete [] n_l;
    }


  // Overloaded operators, implemented as member functions
  // left (implicit) argument is always of type LINT

  // Assignment

  const LINT& operator= (const LINT&);

  // Arithmetic

  const LINT& operator++ (void);
  const LINT operator++ (int);
  const LINT& operator-- (void);
  const LINT operator-- (int);
  const LINT& operator+= (const LINT&);
  const LINT& operator-= (const LINT&);
  const LINT& operator*= (const LINT&);
  const LINT& operator/= (const LINT&);
  const LINT& operator%= (const LINT&);

  // Bitwise and Boolean functions

  const LINT& operator<<= (int);
  const LINT& operator>>= (int);
  const LINT& operator^= (const LINT&);
  const LINT& operator|= (const LINT&);
  const LINT& operator&= (const LINT&);

  // Member functions
  // Arithmetic

  const LINT& add (const LINT&);
  const LINT& sub (const LINT&);
  const LINT& mul (const LINT&);
  const LINT& sqr (void);
  const LINT& divr (const LINT&, LINT&);
  const LINT& mod (const LINT&);
  const LINT& mod2 (USHORT);

  // Modular arithmetic

  const LINT& madd (const LINT&, const LINT&);
  const LINT& msub (const LINT&, const LINT&);
  const LINT& mmul (const LINT&, const LINT&);
  const LINT& msqr (const LINT&);
  const LINT& mexp (const LINT&, const LINT&);
  const LINT& mexp (USHORT, const LINT&);
  const LINT& mexp5m (const LINT&, const LINT&);
  const LINT& mexpkm (const LINT&, const LINT&);
  const LINT& mexp2 (USHORT, const LINT&);
  const LINT& shift (int);

  // Number theoretic member Funktionen

  LINT gcd (const LINT&) const;
  LINT xgcd (const LINT&, LINT&, int&, LINT&, int&) const;
  LINT inv (const LINT&) const;
  LINT lcm (const LINT&) const;
  int jacobi (const LINT&) const;
  LINT root (void) const;
  LINT root (const LINT&) const;
  LINT root (const LINT&, const LINT&) const;
  int twofact (LINT&) const;
  unsigned int ld (void) const;
  int isprime (int noofsmallprimes = 302, int iterations = 0) const;
  LINT issqr (void) const;
  int mequ (const LINT&, const LINT&) const;
  LINT chinrem (const LINT&, const LINT&, const LINT&) const;
  int iseven (void) const;
  int isodd (void) const;


  // Bit-access

  const LINT& setbit (unsigned int);
  const LINT& clearbit (unsigned int);
  int testbit (unsigned int) const;

  // Swapping

  LINT& fswap (LINT&);

  // Purging of LINT-variables, overwriting with 0

  void purge (void);


  // Conversion & Input/Output

  enum {
      lintdec        = 0x010,
      lintoct        = 0x020,
      linthex        = 0x040,
      lintshowbase   = 0x080,
      lintuppercase  = 0x100,
      lintbin        = 0x200,
      lintshowlength = 0x400
  };

  static long flags (void);
  static long flags (ostream&);
  static long setf (long int);
  static long setf (ostream&, long int);
  static long unsetf (long int);
  static long unsetf (ostream&, long int);
  static long restoref (long int);
  static long restoref (ostream&, long int);


  // LINT-to-string conversion

  inline char* hexstr (void) const
    {
      return lint2str (16);
    }
  inline char* decstr (void) const
    {
      return lint2str (10);
    }
  inline char* octstr (void) const
    {
      return lint2str (8);
    }
  inline char* binstr (void) const
    {
      return lint2str (2);
    }

  // Conversion to string and bytevector

  char* lint2str (USHORT, int = 0) const;
  UCHAR* lint2byte (int*) const;

  // Export as CLINT type

  clint* lint2clint (void) const;

  // preformatted output

  inline void disp (char* str)
    {
      cout << str << lint2str (16) << endl << ld () << " bit\n";
    }


  // Set user-defined error handler

  static void Set_LINT_Error_Handler (void (*)(LINT_ERRORS, const char*, int, int, const char*));

  //Get Error/Warning-Status

  LINT_ERRORS Get_Warning_Status (void);

 private:
  // Pointer to type CLINT
  clint* n_l;

  // Status after an operation on a LINT object
  LINT_ERRORS status;

  // LINT::Default-Error-Handler
  static void panic (LINT_ERRORS, const char*, int, int, const char* = "flintpp.cpp");

  // Dummy object of class LintInit
  static LintInit setup;

  // Pointer to ios flag register to be allocated
   static long flagsindex;
};

// CLASS LINT ends
///////////////////////////////////////////////////////////////////////////////




// Some auxiliary functions for LINT manipulators

// ostream format functions
ostream& _SetLintFlags (ostream& , int);
ostream& _ResetLintFlags (ostream& , int);

// Container-class LINT_omanip
template <class T>
class LINT_omanip
{
 public:
  LINT_omanip (ostream&(*g)(ostream&, T), T j): i(j), f(g) {}
  friend ostream& operator <<(ostream& os, const LINT_omanip<T>& m)
    {
      return (*m.f) (os, m.i);
    }

 protected:
  T i;
  ostream& (*f)(ostream&, T);
}; //lint !e1712  Don't complain about missing default constructor


// Manipulators for output format of LINT objects

LINT_omanip <int> SetLintFlags (int);
LINT_omanip <int> ResetLintFlags (int);

ostream& LintHex (ostream&);
ostream& LintDec (ostream&);
ostream& LintOct (ostream&);
ostream& LintBin (ostream&);
ostream& LintUpr (ostream&);
ostream& LintLwr (ostream&);
ostream& LintShowbase (ostream&);
ostream& LintNobase (ostream&);
ostream& LintShowlength (ostream&);
ostream& LintNolength (ostream&);


// Templates for platform-independent writing and reading of USHORT values

template <class T>
int read_ind_ushort (T& s, clint* dest)
{
  UCHAR buff[sizeof (clint)];
  unsigned i;
  s.read ((char*)buff, sizeof (clint));

  if (!s)
    {
      return -1;
    }
  else
    {
      *dest = 0;
      for (i = 0; i < sizeof (clint); i++)
        {
          *dest |= ((clint)buff[i]) << (i << 3);
        }
      return 0;
    }
}


template <class T>
int write_ind_ushort (T& s, clint src)
{
  UCHAR buff[sizeof (clint)];
  unsigned i, j;
  for (i = 0, j = 0; i < sizeof (clint); i++, j = i << 3)
    {
      buff[i] = (UCHAR)((src & (0xff << j)) >> j);
    }
  s.write ((const char*)buff, sizeof (clint));
  if (!s)
    {
      return -1;
    }
  else
    {
      return 0;
    }
}


// min, max as template-inline-functions
// (Acc. Scott Meyers,"Effective C++", 2nd. Ed. Addison-Wesley 1998)
// min und max should be present in  C++-Standard Template Library, but...

#if !defined FLINTPP_ANSI || (defined __IBMCPP__ && (__IBMCPP__ <=300)) || (defined _MSC_VER && (_MSC_VER <= 1200)) || defined __WATCOMC__

#ifndef min
template <class T>
inline const T& min (const T& a, const T& b)
{ return a < b ? a : b; }
#endif // min

#ifndef max
template <class T>
inline const T& max (const T& a, const T& b)
{ return a > b ? a : b; }
#endif // max
#endif // !(defined __IBMCPP__ || defined FLINTPP_ANSI)


#endif // __FLINTPPH__







