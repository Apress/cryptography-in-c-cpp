//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module testcnst.cpp     Revision: 29.06.2002                               */
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

#include <string.h>
#include <stdlib.h>
#include "flintpp.h"


#define MAXTESTLEN (CLINTMAXBIT/2)
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)

#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

static int testconstr2  (int);
static int testconstr3  (int);
static int testconstr4  (int);
static int testconstr5  (int);
static int testconstr6a (int);
static int testconstr6b (int);
static int testconstr6c (int);
static int testconstr7  (int);
static int testconstr8  (int);
static int testconstr9  (int);
static int testconstr10 (int);
static int testconstr11 (int);
static int testconstr12 (int);

static void check (LINT&, LINT&, int);
static void check (CLINT, LINT&, int);
static void ulcheck (ULONG, LINT&, int);

void local_error_handler (LINT_ERRORS, const char*, int, int, const char*);


int main ()
{
  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  initrand64_lt ();

  LINT::Set_LINT_Error_Handler (local_error_handler);

  testconstr2  (1000);
  testconstr3  (5000);
  testconstr4  (1000);
  testconstr5  (5000);
  testconstr6a (5000);
  testconstr6b (5000);
  testconstr6c (5000);
  testconstr7  (5000);
  testconstr8  (5000);
  testconstr9  (5000);
  testconstr10 (5000);
  testconstr11 (5000);
  testconstr12 (5000);

  cout << "All tests in " << __FILE__ << " passed." << endl;
  return 0;
}


static int
testconstr2 (int nooftests)
{
  cout << "Test of constructor 2 ..." << endl;

  char* ofl;
  int i;
  if ((ofl = new char[(sizeof (char) * ( CLINTMAXDIGIT * 4 + 2 ))]) == NULL)
    {
      cerr << "Error in operator new in testcnstr.cpp, line "
           << __LINE__ << endl;
      abort ();
    }


  // Testing constructor 2 with Zahl > max_l
  // Generate string "1000...0" with CLINTMAXDIGIT*16/4 Hex-zeros

  ofl[0] = '1';
  ofl[CLINTMAXDIGIT * 4 + 1] = '\0';
  for (i = 1; (unsigned)i <= CLINTMAXDIGIT * 4; i++)
    {
      ofl[i] = '0';
    }

  LINT checkval;
#ifdef LINT_EX
  try
    {
      checkval = LINT (ofl, 16);
    }

  catch (LINT_OFL &error)
    {
      // Overflow erkannt, OK, tue nichts
    }

  catch (LINT_Error &error)
    {
      cerr << "Error: Overflow not detected near line ";
      cerr << __LINE__ << " in module " << __FILE__ << endl;
      exit (-1);
    }

  if (checkval.Get_Warning_Status () != E_LINT_VAL) 
  // Observe that chaeckval.status is checked against E_LINT_VAL.
  // checkval here is not initialized because of overflow condition in 
  // constructor. If not successful try resets state of variable checkval.
    {
      cerr << "Error in constructor 2: Overflow not detected." << endl;
      cerr << "checkval.Get_Warning_Status () = "
           << (int)checkval.Get_Warning_Status () << endl;
      exit (-1);
    }
#else
  checkval = LINT (ofl, 16);

  if (checkval.Get_Warning_Status () != E_LINT_OFL)
    {
      cerr << "Error in constructor 2: Overflow not detected." << endl;
      cerr << "checkval.Get_Warning_Status () = "
           << (int)checkval.Get_Warning_Status () << endl;
      exit (-1);
    }
#endif //LINT_EX?


  // Testing constructor 2 with "0"

  ofl[0] = '0';
  ofl[1] = '\0';

  LINT nul (0);
  checkval = LINT (ofl, 16);
  check (nul_l, checkval, __LINE__);


  // Testing constructor 2 with ""

  ofl[0] = '\0';

  checkval = LINT (ofl, 16);
  check (nul_l, checkval, __LINE__);

  delete [] ofl;


  // Tests with random values

  LINT test;

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      checkval = LINT (lint2str (test, 16), 16);
      check (test, checkval, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      checkval = LINT (lint2str (test, 2), 2);
      check (test, checkval, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      checkval = LINT (lint2str (test, 8), 8);
      check (test, checkval, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      checkval = LINT (lint2str (test, 10), 10);
      check (test, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr3 (int nooftests)
{
  cout << "Test of constructor 3 ..." << endl;

  UCHAR* ofl;
  int i, len;

  if ((ofl = new UCHAR[(sizeof (UCHAR) * ( CLINTMAXBYTE + 2 ))]) == NULL)
    {
      cerr << "Error in operator new in testcnstr.cpp, line "
           << __LINE__ << endl;
      abort ();
    }


  // Testing constructor 3 with value > max_l
  // Generate byte array 0xff,0xff,...,0xff,0x01 with CLINTMAXBYTE+1 bytes

  for (i = 0; (unsigned)i < CLINTMAXBYTE; i++)
    {
      ofl[i] = 0xff;
    }
  ofl[CLINTMAXBYTE] = 0x01;

  LINT checkval (ofl, CLINTMAXBYTE+1);
  if (checkval.Get_Warning_Status () != E_LINT_OFL)
    {
      cerr << "Error in constructor 3: Overflow not detected." << endl;
      cerr << "checkval.Get_Warning_Status () = "
           << (int)checkval.Get_Warning_Status () << endl;
      exit (-1);
    }


  // Testing constructor 3 with 0x00

  ofl[0] = 0x00;

  checkval = LINT (ofl, 1);
  check (nul_l, checkval, __LINE__);


  // Testing constructor 3 with byte array of length 0

  checkval = LINT (ofl, 0);
  check (nul_l, checkval, __LINE__);

  delete [] ofl;


  // Tests with random values

  LINT test;

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);        // LINT pseudo random value
      ofl = lint2byte (test, &len);     // transformed into byte array
      checkval = LINT (ofl, len);       // constructor 3 constructs LINT object
      check (test, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr4 (int nooftests)
{
  cout << "Test of constructor 4 ..." << endl;

  char* ofl;
  int i;

  if ((ofl = new char[CLINTMAXBIT + 3]) == NULL)
    {
      cerr << "Error in operator new in testcnstr.cpp, line "
           << __LINE__ << endl;
      abort ();
    }


  ofl[0] = '0';                           // Generate string "0x1000...0" with
  ofl[1] = 'x';                           // CLINTMAXDIGIT*16/4 zeros
  ofl[2] = '1';
  ofl[CLINTMAXDIGIT * 4 + 3] = '\0';
  for (i = 3; (unsigned)i <= CLINTMAXDIGIT * 4 + 2; i++)
    {
      ofl[i] = '0';
    }

  LINT checkval (ofl);
  if (checkval.Get_Warning_Status () != E_LINT_OFL)
    {
      cerr << "Error in constructor 4: Overflow not detected." << endl;
      cerr << "checkval.Get_Warning_Status () = "
           << (int)checkval.Get_Warning_Status () << endl;
      exit (-1);
    }


  // Testing constructor 4 with "0"

  ofl[0] = '0';
  ofl[1] = '\0';

  checkval = LINT (ofl);
  check (nul_l, checkval, __LINE__);


  // Testing constructor 4 with ""

  ofl[0] = '\0';

  checkval = LINT (ofl);
  check (nul_l, checkval, __LINE__);


  // Tests with random values

  LINT test;

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      checkval = LINT (lint2str (test, 10));
      check (test, checkval, __LINE__);
    }

  char* N;
  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      N = lint2str (test, 2);
      strcpy (ofl, "0b");
      strcat (ofl, N);
      checkval = LINT (ofl);
      check (test, checkval, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      N = lint2str (test, 2);
      strcpy (ofl, "0B");
      strcat (ofl, N);
      checkval = LINT (ofl);
      check (test, checkval, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      N = lint2str (test, 16);
      strcpy (ofl, "0x");
      strcat (ofl, N);
      checkval = LINT (ofl);
      check (test, checkval, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      test = randl (CLINTRNDLN);
      N = lint2str (test, 16);
      strcpy (ofl, "0X");
      strcat (ofl, N);
      checkval = LINT (ofl);
      check (test, checkval, __LINE__);
    }

  delete [] (ofl);
  return 0;
}


static int
testconstr5 (int nooftests)
{
  cout << "Test of constructor 5 ..." << endl;


  // Test with 0

  LINT checkval (LINT (0));
  check (nul_l, checkval, __LINE__);


  // Test with max

  CLINT nmax_l;
  setmax_l (nmax_l);
  LINT nmax (nmax_l);                   // Constructor 12 comes into play
  checkval = LINT (nmax);
  check (nmax, checkval, __LINE__);


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      LINT test = randl (CLINTRNDLN);
      checkval = LINT (test);
      check (test, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr6a (int nooftests)
{
  cout << "Test of constructor 6 with char ..." << endl;


  // Test with 0

  char t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);      //lint !e732 Signed/unsigned mismatch
                                        // is acceptable here


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = (ucrand64_l () % 0x80);
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);  //lint !e732 dito
    }

  return 0;
}


static int
testconstr6b (int nooftests)
{
  cout << "Test of constructor 6 with short ..." << endl;


  // Test with 0

  short t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);      //lint !e732 s.o.


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = (usrand64_l () % 0x8000);
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);  //lint !e732 s.o.
    }

  return 0;
}


static int
testconstr6c (int nooftests)
{
  cout << "Test of constructor 6 with int ..." << endl;


  // Test with 0

  int t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);      //lint !e732 s.o.



  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = (ulrand64_l () % 0x80000000);
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);  //lint !e732 s.o.
    }

  return 0;
}


static int
testconstr7 (int nooftests)
{
  cout << "Test of constructor 7 ..." << endl;


  // Test with 0

  long t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);      //lint !e732 s.o.


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = (ulrand64_l () % 0x80000000);
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);  //lint !e732 s.o.
    }

  return 0;
}


static int
testconstr8 (int nooftests)
{
  cout << "Test of constructor 8 ..." << endl;


  // Test with 0

  UCHAR t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = ucrand64_l ();
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr9 (int nooftests)
{
  cout << "Test of constructor 9 ..." << endl;


  // Test with 0

  USHORT t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = usrand64_l ();
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr10 (int nooftests)
{
  cout << "Test of constructor 10 ..." << endl;


  // Test with 0

  unsigned int t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = ulrand64_l ();
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr11 (int nooftests)
{
  cout << "Test of constructor 11 ..." << endl;


  // Test with 0

  ULONG t = 0;
  LINT checkval (t);
  ulcheck (t, checkval, __LINE__);


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      t = ulrand64_l ();
      checkval = LINT (t);
      ulcheck (t, checkval, __LINE__);
    }

  return 0;
}


static int
testconstr12 (int nooftests)
{
  cout << "Test of constructor 12 ..." << endl;

  CLINT test_l;


  // Test with 0

  LINT checkval (nul_l);
  check (nul_l, checkval, __LINE__);


  // Test with max_l in testconstr5 ()


  // Tests with random values

  for (int i = 1; i <= nooftests; i++)
    {
      rand_l (test_l, CLINTRNDLN);
      checkval = LINT (test_l);
      check (test_l, checkval, __LINE__);
    }

  return 0;
}


static void
check (LINT &a, LINT& b, int line)
{
  if (!(b >= 0))                        // Check value of of b.init implicitly
    {
      cerr << "Error in line " << line << endl;
      cerr << "Object not initialized" << endl;
    }

  if (b.Get_Warning_Status () != E_LINT_OK)
    {
      cerr << "Error in line " << line << endl;
      cerr << "Object status not OK" << endl;
    }

  if (a != b)
    {
      cerr << "Error in line " << line << endl;
      cerr << "a = " << a << endl;
      cerr << "b = " << b << endl;
      exit (1);
    }
}


static void
check (CLINT a_l, LINT& b, int line)
{
  if (!(b >= 0))                        // Check value of of b.init implicitly
    {
      cerr << "Error in line " << line << endl;
      cerr << "Object not initialized" << endl;
    }

  if (b.Get_Warning_Status () != E_LINT_OK)
    {
      cerr << "Error in line " << line << endl;
      cerr << "Object status not OK" << endl;
    }

  UCHAR* N1,* N2;
  int len1, len2;

  N1 = clint2byte_l (a_l, &len1);
  N2 = lint2byte (b, &len2);

  if (len1 != len2)
    {
      cerr << "Error in line " << line << endl;
      cerr << "a = " << hexstr_l (a_l) << endl;
      cerr << "b = " << b << endl;
      exit (1);
    }

  if (strncmp ((const char*)N1, (const char*)N2, (unsigned)len1) != 0)
    {
      cerr << "Error in line " << line << endl;
      cerr << "a = " << hexstr_l (a_l) << endl;
      cerr << "b = " << b << endl;
      exit (1);
    }
}


static void
ulcheck (ULONG a, LINT& b, int line)
{
  if (!(b >= 0))                        // Check value of of b.init implicitly
    {
      cerr << "Error in line " << line << endl;
      cerr << "Object not initialized" << endl;
    }

  if (b.Get_Warning_Status () != E_LINT_OK)
    {
      cerr << "Error in line " << line << endl;
      cerr << "Object status not OK" << endl;
      cerr << "Warning status = " << (int)b.Get_Warning_Status() << endl;
    }

  if (LINT (a) != b)
    {
      cerr << "Error in line " << line << endl;
      cerr << "a = " << a << endl;
      cerr << "b = " << b << endl;
      exit (1);
    }
}


void local_error_handler (LINT_ERRORS err, const char* func, int arg, int line, const char* file)
{

#ifdef LINT_EX
  if (err = E_LINT_OFL)
    throw LINT_OFL (func, line, __FILE__);
#endif

  return;
}


