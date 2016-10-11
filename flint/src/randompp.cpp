//*****************************************************************************/
//                                                                            */
// Software supplement to the book "Cryptography in C and C++"                */
// by Michael Welschenbach, published by Apress Berkeley CA, 2001,2005        */
//                                                                            */
// Module randompp.cpp     Revision: 19.05.2003                               */
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

#include "flintpp.h"
#include "random.h"

#if defined FLINTPP_ANSI
#define NOTHROW (nothrow)
#else
#define NOTHROW
#endif

#define NO_ASSERTS

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_assert.h"
#endif

#ifdef NO_ASSERTS
#define Assert(a) (void)0
#endif


// Initialize PRNG

int InitRand (STATEPRNG& xrstate, const char* UsrStr, int LenUsrStr, int Entropy, int Generator)
{
  return InitRand_l (&xrstate, (char*)UsrStr, LenUsrStr, Entropy, Generator);
}


void PurgeRand (STATEPRNG& xrstate)
{
  PurgeRand_l (&xrstate);
}


// Generation of a random number r with 2^(l-1) <= r < 2^l

LINT RandLINT (int l, STATEPRNG& xrstate)
{
  LINT random;

  int error = Rand_l (random.n_l, &xrstate, l);

  if (E_CLINT_OK != error)
    {
      LINT::panic (E_LINT_RIN, "RandLINT", 0, __LINE__, __FILE__);
    }
  else
    {
      random.status = E_LINT_OK;
    }

  return random;
}


// Generation of a random number r rmin <= r <= rmax

LINT RandLINT (const LINT& rmin, const LINT& rmax, STATEPRNG& xrstate)
{
  if (rmin.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "RandLINT", 1, __LINE__, __FILE__);
  if (rmax.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "RandLINT", 2, __LINE__, __FILE__);
  if (rmax < rmin) 
    LINT::panic (E_LINT_INV, "RandLINT", 1, __LINE__, __FILE__);

  LINT random;

  int error = RandMinMax_l (random.n_l, &xrstate, rmin.n_l, rmax.n_l);

  if (E_CLINT_OK != error)
    {
      LINT::panic (E_LINT_RIN, "RandLINT", 0, __LINE__, __FILE__);
    }
  else
    {
      random.status = E_LINT_OK;
    }

  return random;
}


// Generation of a random prime p with 2^(l-1) <= p < 2^l

LINT FindPrime (USHORT l, STATEPRNG& xrstate)
{
  int error;
  LINT rndprime;
  error = FindPrime_l (rndprime.n_l, &xrstate, l);

  if (E_CLINT_OK != error)
    {
      LINT::panic (E_LINT_RIN, "FindPrime", 0, __LINE__, __FILE__);
    }
  else
    {
      rndprime.status = E_LINT_OK;
    }

  return rndprime;
}


// Generation of a random prime p with 2^(l-1) <= p < 2^l
// and gcd (p - 1, f) = 1

LINT FindPrime (USHORT l, const LINT& f, STATEPRNG& xrstate)
{
  if (0 == l || l > CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "FindPrime", 1, __LINE__, __FILE__);
  if (f.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "FindPrime", 3, __LINE__, __FILE__);

  // 0 < f has to be odd
  if (f.iseven ()) 
    LINT::panic (E_LINT_INV, "FindPrime", 3, __LINE__, __FILE__);

  LINT rndprime;

  LINT pmin (0);
  pmin.setbit (l - 1);
  LINT pmax = ((LINT(0).setbit (l - 1) - 1) << 1) + 1; // max number with l bit

  int error = 
    FindPrimeMinMaxGcd_l (rndprime.n_l, &xrstate, pmin.n_l, pmax.n_l, f.n_l);

  if (E_CLINT_OK != error)
    {
      LINT::panic (E_LINT_RIN, "FindPrime", 0, __LINE__, __FILE__);
    }
  else
    {
      rndprime.status = E_LINT_OK;
    }

  return rndprime;
}


// Generation of a random prime number p pmin <= p <= pmax
// and gcd (p - 1, f) = 1

LINT FindPrime (const LINT& pmin, const LINT& pmax, const LINT& f, STATEPRNG& xrstate)
{
  if (pmin.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "FindPrime", 1, __LINE__, __FILE__);
  if (pmax.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "FindPrime", 2, __LINE__, __FILE__);
  if (f.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "FindPrime", 3, __LINE__, __FILE__);

  if (pmin > pmax) 
    LINT::panic (E_LINT_INV, "FindPrime", 1, __LINE__, __FILE__);

  // 0 < f has to be odd
  if (f.iseven ()) 
    LINT::panic (E_LINT_INV, "FindPrime", 3, __LINE__, __FILE__);

  LINT rndprime;

  int error = 
    FindPrimeMinMaxGcd_l (rndprime.n_l, &xrstate, pmin.n_l, pmax.n_l, f.n_l);

  if (E_CLINT_OK != error)
    {
      LINT::panic (E_LINT_RIN, "FindPrime", 0, __LINE__, __FILE__);
    }
  else
    {
      rndprime.status = E_LINT_OK;
    }

  return rndprime;
}


// Find random prime number p of length 2^(l-1) <= p <= 2^l - 1
// with p = a mod q and gcd (p - 1, f) = 1.
// Input parameter: 2 < q prime, a mod q != 0, 0 < f odd

LINT ExtendPrime (const LINT& pmin,
                  const LINT& pmax,
                  const LINT& a,
                  const LINT& q,
                  const LINT& f,
                  STATEPRNG& xrstate)
{
  if (pmin.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 1, __LINE__, __FILE__);
  if (pmax.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 2, __LINE__, __FILE__);
  if (a.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 3, __LINE__, __FILE__);
  if (q.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 4, __LINE__, __FILE__);
  if (f.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 5, __LINE__, __FILE__);

  if (pmin > pmax) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 1, __LINE__, __FILE__);

  // q has to be odd
  if (q.iseven ()) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 3, __LINE__, __FILE__);

  // a must not be divided by q 
  if (a.mequ (0,q)) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 2, __LINE__, __FILE__);

  // 0 < f has to be odd
  if (f.iseven ()) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 4, __LINE__, __FILE__);

  LINT p = RandLINT (pmin, pmax, xrstate);
  LINT twotimesq = q << 1;
  LINT t = pmax - pmin;

  if (p < pmin)
    {
      p += pmin;
    }

  LINT w = p % twotimesq;

  p += (twotimesq - w) + a;

  if (p.iseven ())
    {
      p += q;
    }

  while (p > pmax)
    {
      p = pmin + p % (t + 1);
      w = p % twotimesq;
      p += (twotimesq - w) + a;
      if (p.iseven ())
        {
          p += q;
        }
    }

  while (1 != gcd (p - 1, f) || !p.isprime ())
    {

      Assert (twotimesq < pmax);

      p += twotimesq;

      while (p > pmax)
        {
          p += sRand_l (&xrstate);
          p = pmin + p % (t + 1);

          Assert (p < pmax);

          w = p % twotimesq;
          p += (twotimesq - w) + a;

          Assert (p < pmax);

          if (p.iseven ())
            {
              p += q;
            }

          Assert (p < pmax);

        }
    }

  Assert ((p - a) % q == 0);

  return p; // p is prime with p = a mod q, ggT (p - 1, f) = 1
}


// Find random prime number p of length pmin <= p <= pmax
// with p = a mod q and gcd (p - 1, f) = 1.
// Input parameter: 2 < q prime, a mod q != 0, 0 < f odd

LINT ExtendPrime (USHORT l,
                  const LINT& a,
                  const LINT& q,
                  const LINT& f,
                  STATEPRNG& xrstate)
{
  if (l > CLINTMAXBIT) 
    LINT::panic (E_LINT_INV, "ExtendPrime", 1, __LINE__, __FILE__);

  LINT pmin = LINT(0).setbit (l - 1);
  LINT pmax = LINT(0).setbit (l);
  --pmax;

  return (ExtendPrime (pmin, pmax, a, q, f, xrstate));
}


// Find strong prime p of length 2^(l-1) <= p <= 2^l - 1
// with primes r, s, t, such that
//    r divides p - 1
//    t divides r - 1
//    s divides p + 1
//
// Input parameter: Binary length l of p

LINT StrongPrime (USHORT l, STATEPRNG& xrstate)
{
  if (l >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 1, __LINE__, __FILE__);

  LINT pmin = LINT(0).setbit (l - 1);
  LINT pmax = LINT(0).setbit (l);
  --pmax;

  return (StrongPrime (pmin, pmax, (l>>2)-8, (l>>1)-8, (l>>1)-8, 1, xrstate));
}


// Find strong prime p of length 2^(l-1) <= p <= 2^l - 1
// with gcd (p - 1, f) = 1 and primes r, s, t such that
//    r divides p - 1
//    t divides r - 1
//    s divides p + 1
//
// Input parameter: Binary length l of p,
//                  0 < f odd

LINT StrongPrime (USHORT l, const LINT& f, STATEPRNG& xrstate)
{
  if (l >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 1, __LINE__, __FILE__);

  LINT pmin = LINT(0).setbit (l - 1);
  LINT pmax = LINT(0).setbit (l);
  --pmax;

  return (StrongPrime (pmin, pmax, (l>>2)-8, (l>>1)-8, (l>>1)-8, f, xrstate));
}


// Find strong prime p of length 2^(l-1) <= p <= 2^l - 1
// with gcd (p - 1, f) = 1 and primes r, s, t such that
//    r divides p - 1
//    t divides r - 1
//    s divides p + 1
//
// Input parameter: Binary length l of p,
//                  Lengths lt, lr and ls of primes t, r and s resp.
//                     lt <~ l/4, lr ~ ls <~ l/2 of l
//                  <~ means: smaller than, close to
//                  0 < f odd

LINT StrongPrime (USHORT l,
                  USHORT lt,
                  USHORT lr,
                  USHORT ls,
                  const LINT& f,
                  STATEPRNG& xrstate)
{
  if (l >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 1, __LINE__, __FILE__);
  if (lt >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 2, __LINE__, __FILE__);
  if (lr >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 3, __LINE__, __FILE__);
  if (ls >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 4, __LINE__, __FILE__);

  LINT pmin = LINT(0).setbit (l - 1);
  LINT pmax = LINT(0).setbit (l);
  --pmax;

  return (StrongPrime (pmin, pmax, lt, ls, lr, f, xrstate));
}


// Find strong prime p with pmin <= p <= pmax
// with gcd (p - 1, f) = 1 and primes r, s, t such that
//    r divides p - 1
//    t divides r - 1
//    s divides p + 1
//
// Input parameter: pmin, pmax
//                  0 < f odd

LINT StrongPrime (const LINT& pmin,
                  const LINT& pmax,
                  const LINT& f,
                  STATEPRNG& xrstate)
{
  if (pmin.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "StrongPrime", 1, __LINE__, __FILE__);
  if (pmax.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "StrongPrime", 2, __LINE__, __FILE__);
  if (f.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "StrongPrime", 6, __LINE__, __FILE__);

  // 0 < f muss ungerade sein
  if (f.iseven ()) 
    LINT::panic (E_LINT_INV, "StrongPrime", 5, __LINE__, __FILE__);

  int lt = (ld (pmin) >> 2) - 8;
  int lr = (ld (pmin) >> 1) - 8;
  int ls = lr;

  return StrongPrime (pmin, pmax, lt, ls, lr, f, xrstate);
}


// Find strong prime p with pmin <= p <= pmax
// with gcd (p - 1, f) = 1 and primes r, s, t such that
//    r divides p - 1
//    t divides r - 1
//    s divides p + 1
//
// Input parameters: pmin, pmax,
//                   Lengths lt, lr and ls of primes t, r and s resp.
//                     lt <~ l/4, lr ~ ls <~ l/2 of l
//                   <~ means: smaller than, close to
//                   0 < f odd

LINT StrongPrime (const LINT& pmin,
                  const LINT& pmax,
                  USHORT lt,
                  USHORT lr,
                  USHORT ls,
                  const LINT& f,
                  STATEPRNG& xrstate)
{
  if (pmin.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "StrongPrime", 1, __LINE__, __FILE__);
  if (pmax.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "StrongPrime", 2, __LINE__, __FILE__);
  if (lt >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 3, __LINE__, __FILE__);
  if (lr >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 4, __LINE__, __FILE__);
  if (ls >= CLINTMAXBIT) 
    LINT::panic (E_LINT_OFL, "StrongPrime", 5, __LINE__, __FILE__);
  if (f.status == E_LINT_INV) 
    LINT::panic (E_LINT_INV, "StrongPrime", 6, __LINE__, __FILE__);

  // 0 < f muss ungerade sein
  if (f.iseven ()) 
    LINT::panic (E_LINT_INV, "StrongPrime", 5, __LINE__, __FILE__);

  LINT t = FindPrime (lt, 1, xrstate);
  LINT r = ExtendPrime (lr, 1, t, 1, xrstate);
  LINT s = FindPrime (ls, 1, xrstate);

  LINT p = inv (r,s);  // p := r^(-1) mod s
  p *= r;              // p := r^(-1) * r
  p <<= 1;             // p := 2*r^(-1) * r
  LINT rs = r*s;
  p = msub (1,p,rs);   // p := 1 - 2*r^(-1) * r mod r*s = s*u - r*v mod r*s
                       // mit u := s^(-1) mod r und v := r^(-1) mod s

  Assert ((p - 1) % r == 0);
  Assert ((p + 1) % s == 0);

  p = ExtendPrime (pmin, pmax, p, rs, f, xrstate);

  Assert ((p - 1) % r == 0);
  Assert ((p + 1) % s == 0);

  return p;
}


// Compatibility functions for prior versions of Blum-Blum-Shub-Generator
// not thread-safe!

static STATEPRNG xrstate_loc;

int seedBBS (const LINT& seed)
{
  if (seed.status == E_LINT_INV) LINT::panic (E_LINT_INV, "seedBBS", 0, __LINE__);
  xrstate_loc.Generator = FLINT_RNDBBS;
  return (seedBBS_l (&xrstate_loc.StateBBS, seed.n_l));
}


LINT randBBS (int l)
{
  return RandLINT (l, xrstate_loc);
}


LINT randBBS (const LINT& rmin, const LINT& rmax)
{
  return RandLINT (rmin, rmax, xrstate_loc);
}









