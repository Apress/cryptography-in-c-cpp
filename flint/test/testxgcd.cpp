//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach                                                    */
//                                                                            */
// Module testxgcd.cpp     Revision: 01.07.2002                               */
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
#include "flintpp.h"

#define MAXTESTLEN (CLINTMAXBIT/2)
#define CLINTRNDLN (1 + ulrand64_l() % (MAXTESTLEN/2))

static int general_tests (int);

int main ()
{
  cout << "Testmodule " << __FILE__
       << " compiled for FLINT/C-Library Version "
       << verstr_l () << endl;

  initrand64_lt ();
  general_tests (10000);

  cout << "All tests in " << __FILE__ << " passed." << endl;
  return 0;
}


static int general_tests (int nooftests)
{
  LINT a, b, u, v, g, gg;
  int i, sign_u, sign_v;

  for (i = 1; i <= nooftests; i++)
    {
      a = randl (CLINTRNDLN/2);
      b = randl (CLINTRNDLN/2);

      g = gcd (a, b);
      gg = xgcd (a, b, u, sign_u, v, sign_v);

      if (g != gg)
        {
          cerr << "Error in comparing results of friend functions" << endl;
          cerr << "gcd and xgcd in test " << i << " near line " << __LINE__ << endl;
          exit (-1);
        }

      if (g == 1 && b > 1)
        {
          if ((a * inv (a, b)) % b != 1)
            {
              cerr << "Error in friend function inv in test " << i;
              cerr << " near line " << __LINE__ << endl;
              exit (-1);
            }
        }

      if (a * b != g * lcm (a, b))      // gcd (a, b) * lcm (a, b) = a * b
        {
          cerr << "Error in friend function lcm" << endl;
          cerr << "in test " << i << " near line " << __LINE__ << endl;
          exit (-1);
        }


      a *= u;
      b *= v;

      if (sign_u == 1 && sign_v == 1) a += b;
      if (sign_u == -1 && sign_v == 1) a = b - a;
      if (sign_u == 1 && sign_v == -1) a -= b;

      if (a != gg)
        {
          cerr << "Error in friend function xgcd in test " << i;
          cerr << " near line " << __LINE__ << endl;
          exit (-1);
        }
    }


  for (i = 1; i <= nooftests; i++)
    {
      a = randl (CLINTRNDLN/2);
      b = randl (CLINTRNDLN/2);

      g = a.gcd (b);
      gg = a.xgcd (b, u, sign_u, v, sign_v);
      
      if (g != gg)
        {
          cerr << "Error in comparing results of member functions" << endl;
          cerr << "gcd and xgcd in test " << i << " near line " << __LINE__ << endl;
          exit (-1);
        }

      if (g == 1 && b > 1)
        {
          if ((a * a.inv (b)) % b != 1)
            {
              cerr << "Error in member function inv in test " << i;
              cerr << " near line " << __LINE__ << endl;
              exit (-1);
            }
        }

      if (a * b != g * a.lcm (b))      // gcd (a, b) * lcm (a, b) = a * b
        {
          cerr << "Error in member function lcm" << endl;
          cerr << "in test " << i << " near line " << __LINE__ << endl;
          exit (-1);
        }


      a *= u;
      b *= v;

      if (sign_u == 1 && sign_v == 1) a += b;
      if (sign_u == -1 && sign_v == 1) a = b - a;
      if (sign_u == 1 && sign_v == -1) a -= b;

      if (a != gg)
        {
          cerr << "Error in member function xgcd in test " << i;
          cerr << " near line " << __LINE__ << endl;
          exit (-1);
        }
    }

  return 0;
}


