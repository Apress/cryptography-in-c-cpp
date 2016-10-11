/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testdiv.c        Revision: 07.05.2003                               */
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
/*                                                                            */
/*  History                                                                   */
/*                                                                            */
/*  26.01.2002:                                                               */
/*    Added test with product and factors of Bonn-University-factorization    */
/*    from January 2002                                                       */
/*  07.02.2003:                                                               */
/*    Added test with product and factors of Bonn-University-factorization    */
/*    of RSA-160 from March 2003                                              */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "flint.h"

#define NO_ASSERTS

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_alloc.h"
#endif

#include "_assert.h"


#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

static int neu_test (unsigned int nooftests);
static int small_divisors_test (unsigned int nooftests);
static int divsub_test (unsigned int nooftests);
static int divrem_test (unsigned int nooftests);
static int rsaXXX_test (void);
static int S5_test (void);
static int mod_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);

static void ldzrand_l (CLINT n_l, int bits);

int main ()
{
  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  neu_test (1000);
  small_divisors_test (1000);
  rsaXXX_test ();
  S5_test ();
  divsub_test (1000);
  divrem_test (10000);
  mod_test (1000);

  free_reg_l ();

  printf ("All tests in %s passed.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (unsigned int nooftests)
{
  unsigned long i = 1;

  /* Test division by 0 */

  rand_l (r1_l, CLINTRNDLN);
  setzero_l (r2_l);
  if (0 == div_l (r1_l, r2_l, r3_l, r4_l))
    {
      printf ("Error: Division by 0 not detected in line %d\n", __LINE__);
      exit (-1);
    }


  /* Test division 0 by value > 0 */

  setzero_l (r1_l);
  rand_l (r2_l, CLINTRNDLN);
  div_l (r1_l, r2_l, r3_l, r4_l);
  check (r3_l, nul_l, i, __LINE__);
  check (r4_l, nul_l, i, __LINE__);


  printf ("Test with 1 as neutral element...\n");

  setone_l (r2_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      div_l (r1_l, r2_l, r3_l, r4_l);
      check (r3_l, r1_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test with 1 and leading zeros */
  ldzrand_l (r2_l, 1);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      div_l (r1_l, r2_l, r3_l, r4_l);
      check (r3_l, r1_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }
  return 0;
}


static int small_divisors_test (unsigned int nooftests)
{
  unsigned int i;
  clint n;

  printf ("Test division by small divisors...\n");

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      do
        {
          u2clint_l (r2_l, usrand64_l ());
        }
      while (eqz_l (r2_l));
      div_l (r1_l, r2_l, r3_l, r4_l);
      mul_l (r2_l, r3_l, r5_l);
      add_l (r5_l, r4_l, r6_l);
      check (r6_l, r1_l, i, __LINE__);
    }


  /* Test udiv_l() in accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      cpy_l (r0_l, r1_l);
      do
        {
          n = usrand64_l ();
        }
      while (0 == n);
      u2clint_l (r3_l, n);

      udiv_l (r1_l, n, r1_l, r4_l);
      mul_l (r3_l, r1_l, r5_l);
      add_l (r5_l, r4_l, r5_l);
      check (r5_l, r0_l, i, __LINE__);

      udiv_l (r2_l, n, r5_l, r2_l);
      mul_l (r3_l, r5_l, r5_l);
      add_l (r5_l, r2_l, r6_l);
      check (r6_l, r0_l, i, __LINE__);
    }


  /* Euclidean Test  a = b * q1 + r1, b = c * q2 + r2   */
  /*             ==> a = c * q1 * q2 + r3 * q1 + r1     */

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      do
        {
          u2clint_l (r2_l, usrand64_l ());
        }
      while (eqz_l (r2_l));
      do
        {
          u2clint_l (r3_l, usrand64_l ());
        }
      while (eqz_l (r3_l));

      div_l (r1_l, r2_l, r4_l, r5_l); /* r1_l = r2_l * r4_l + r5_l */
      div_l (r2_l, r3_l, r6_l, r7_l); /* r2_l = r3_l * r6_l + r7_l */

      mul_l (r3_l, r6_l, r8_l);
      mul_l (r8_l, r4_l, r8_l);
      mul_l (r7_l, r4_l, r9_l);
      add_l (r8_l, r9_l, r9_l);
      add_l (r9_l, r5_l, r9_l); /* r9_l = r3_l*r6_l*r4_l + r7_l*r4_l + r5_l */

      check (r9_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int divsub_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test division by repeated subtraction...\n");

  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          rand_l (r1_l, CLINTRNDLN);
          rand_l (r2_l, ( digits_l (r1_l) * BITPERDGT ) - ( ucrand64_l () % 11 ));
        }
      while (eqz_l (r2_l));

      div_l (r1_l, r2_l, r3_l, r4_l); /* r1_l = r2_l*r3_l + r4_l */

      setzero_l (r5_l);
      while (ge_l (r1_l, r2_l))
        {
          sub_l (r1_l, r2_l, r1_l); /* r1_l = r1_l - r2_l - ... - r2_l */
                                    /*        ------ r5_l-times ------ */
          add_l (r5_l, one_l, r5_l);
        }

      check (r3_l, r5_l, i, __LINE__);
      check (r4_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int rsaXXX_test (void)
{
  CLINT rsaXXX_l;

  printf ("Test with RSA-130 ...\n");

  str2clint_l (rsaXXX_l, "18070820886874048059516561644059055662781025167694013"
                         "49170127021450056662540244048387341127590812303371781"
                         "887966563182013214880557", 10);
  str2clint_l (r2_l, "39685999459597454290161126162883786067576449112810064"
                     "832555157243", 10);
  str2clint_l (r3_l, "45534498646735972188403686897274408864356301263205069"
                     "600999044599", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 1, __LINE__);
  check (r11_l, nul_l, 1, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 2, __LINE__);
  check (r11_l, nul_l, 2, __LINE__);

  printf ("Test with RSA-140 ...\n");

  str2clint_l (rsaXXX_l, "21290246318258757547497882016271517497806703963277216"
                         "27823338321538194998405649591136657385302191831678310"
                         "7387995317230889569230873441936471", 10);
  str2clint_l (r2_l, "33987174230284385545301236276138758356339864959695974"
                     "23490929302771479", 10);
  str2clint_l (r3_l, "62642001874012850961516549482644422193020371786235090"
                     "19111660653946049", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  printf ("Test with RSA-155 ...\n");

  str2clint_l (rsaXXX_l, "10941738641570527421809707322040357612003732945449205"
                         "99091384213147634998428893478471799725789126733249762"
                         "5752899781833797076537244027146743531593354333897", 10);
  str2clint_l (r2_l, "102639592829741105772054196573991675900716567808038066803"
                     "341933521790711307779", 10);
  str2clint_l (r3_l, "106603488380168454820927220360012878679207958575989291522"
                     "270608237193062808643", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  printf ("Test with Singh-Challenge ...\n");

  str2clint_l (rsaXXX_l, "10742788291266565907178411279942116612663921794753294"
                         "58887781721035546415098012187903383292623528109075067"
                         "2083504941996433143425558334401855808989426892463", 10);
  str2clint_l (r2_l, "128442051653810314916622590289775531989649843239158643682"
                     "16177647043137765477", 10);
  str2clint_l (r3_l, "836391832187606937820650856449710761904520026199724985596"
                     "729108812301394489219", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  printf ("Test with the Bonn University factorization 1/2002 ...\n");

  str2clint_l (rsaXXX_l, "39505874583265144526419767800614481996020776460304936"
                         "45413937605157935562652945068360972784246821953509354"
                         "4305870490251995655335710209799226484977949442955603", 10);
  str2clint_l (r2_l, "338849583746672139436839320467218152281583036860499304808"
                     "4925840555281177", 10);

  str2clint_l (r3_l, "116588234066712599031483765583832708181310122581463926004"
                     "39520994131344334162924536139", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  printf ("Test with RSA-160 ...\n");

  str2clint_l (rsaXXX_l, "21527411027188897018960152013128254292577735888456759"
                         "80170497676778133145218859135673011059773491059602497"
                         "90711158521430207931466520284014061994699492757040775"
                         "3", 10);

  str2clint_l (r2_l, "45427892858481394071686190649738831"
                     "656137145778469793250959984709250004157335359", 10);

  str2clint_l (r3_l, "47388090603832016196633832303788951"
                     "973268922921040957944741354648812028493909367", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  return 0;
}


static int S5_test (void)
{

  printf ("Test of step 5 in division algorithm  ...\n");

  str2clint_l (r1_l, "e37d3abc904baba7a2ac4b6d8f782b2bf84919d2917347690d9e93d"
        "cdd2b91cee9983c564cf1312206c91e74d80ba479064c8f42bd70aaaa689f80d435a"
        "fc997ce853b465703c8edca", 16);

  str2clint_l (r2_l, "80b0987b72c1667c30c9156a6674c2e73e61a1fd527d4e78b3f1505"
        "603c566658459b83ccfd587ba9b5fcbdc0ad09152e0ac265", 16);

  str2clint_l (r3_l, "1c48a1c798541ae0b9eb2c6327b1fffff4fe5c0e2723", 16);

  str2clint_l (r4_l, "ca2312fbb3f4c23add7655e94c3410b15c6064bd48a4e5fcc33ddf5"
        "53e7cb829bf66fbfd61b4667f5ed6b387ec47c5272cf6fb", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);
  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);

#if !(CLINTMAXDIGIT < 256)
  str2clint_l (r1_l, "100620a6461fc335d8d5be9d9c4039c9d6925da2f9aedea05185ead8"
        "d039a84e5be1ee1858079d45afbdf46045c011b561555ac364d7706eb32a1c4d8f025"
        "ab872ef4a3e6484bc35d114bf9907b7917aa5e4e3cd7adf283d672999460eddee6ca6"
        "b303d534795858191cfd6a3437f1600283732ae7001d385167a951aa7affaed8f5fd8"
        "e6460cf6b31fbb840494f1c1fbeda55cb4fbd78fbf467ee6da98b8e8d30bd59a87223"
        "802740314567d98d6d4a1f67c6ed1b3fb2f6e77c6d14863854e857132c0863a5d3b22"
        "d191b120141e1b0f7c3cd9679cdad3d19cc07b7baba609a8c0cf18af4f73fd9c8a691"
        "cf942c2c3b5bea32d7f1bdfc9b40cb236b2c364ee923e4f2dafa24469b9a97d11919f"
        "59b10dbb7130ddd8a68ea89f1a18e186dd9f09d06da60b14ee7fffbc1ac93d8111249"
        "1c19574da97cffb4696e935d5ef9fb9896e8e51fe0405e97c485de8c53305f325b7d8"
        "833effebd93e889dbefd29a847647dcf0d68ed219b92db7aa669dfa911b4c1eaf4796"
        "b88c7ceec50960b14865ad5a3a0741ab4f76314888364e384c870659b51af1815ce81"
        "a5685d6383a56cefc1e9296d5c0b", 16);

  str2clint_l (r2_l, "bc2adf0cec6fb67ee7b51f89c69e6ac4a8bc1493c15087b0c7b89558"
        "1f6c9369bcd8ef94a5e7caf906da3949ce5a8cb04df692b293e400cc479128e5045e1"
        "2d860ead1c634f2efe031", 16);

  str2clint_l (r3_l, "15cce78a7a83c62ea91adf7f4775543f5edb0cdf8f3efe09aed6073b"
        "7e55178d7fc577c6582675fc939117e52293036e590beb42d2c10812eae3aa1c47afc"
        "7b72aeacc157dd3bb8fc7210b60494c12afe4de4e4c2c08e5f9f9df408d387bac6103"
        "7a9f213c607293675693cb6662103ba09aa629af48918dc8eaa0a351909962aa1a6c1"
        "80021670a39eadc74988483e4b71d3c66affff99b0105495f00c87ffd13749da4bbb2"
        "7cf864597bf2ea71b3fbc15649d35d066e515f60b664577760422d09546d9819b1cd4"
        "289d6b6e706fcc511a8eb0a7e55cbd806d9f99d5959b79675183b878605031ca3bea4"
        "665cca977b344174d4a9b8c00ec5b4a11796f52a8e8ea10373ba497947c0fa4576366"
        "a3276d272895106436a7cbb607ae18115552c1e55cfebfc38b068324b67d9306e4f89"
        "8ed9afc3c557f56f9efe71261a90fbc9fc27095f165c1107448a41cacd7e1831e73aa"
        "2d08b6e239e313ec2301", 16);

  str2clint_l (r4_l, "48114bc17b6310f96a10965127fa5600ecd2f565e2389fe78f51a91f"
        "4bf9e94128eafcc566f05322604ec7a6830b2a01ea9315da71b3e3e8c5a10c37aa8ee"
        "a3557984c47441aaac8da", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);

  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);
#endif /* !(CLINTMAXDIGIT < 256) */

  str2clint_l (r1_l, "7b952b50cba1bd5573a8e0d8ea33e3b7fa5322ece49a3663d72ef457"
        "30dbaf36f1d36156c000c5614586e0debbf6cbf4913417e11f621a3845d6804c8db3b"
        "2cc01b37f198016bf8f3540facf5cadc9059e969e813bf5d", 16);

  str2clint_l (r2_l, "288f8a2d3ccaf166904edea8f67aa27d540bb215d1ea0f50a7311fa7"
        "d07db3b95b947fbc5e", 16);

  str2clint_l (r3_l, "30bfedf9dd29106a454370be597dd621e9d3c65731021de3ef135837"
        "fb5aa548fffff09dc1cd172c95d26735eeb6e1ed9e08", 16);

  str2clint_l (r4_l, "1aa7fd2df02cafd036533325274e90705ea9249d24f121c92357853f"
        "f579a38b426ccdd86d", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);

  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);


  /* Special case: Step 5 will be executed in division with 32 bit words */

  str2clint_l (r1_l, "1f1bf7045d135fbc248134dcb1bb953a548d16e3578b7300", 16);

  str2clint_l (r2_l, "07c6fdc11744d7ef09204d372c6ee54ed37552c7978d7131", 16);

  str2clint_l (r4_l, "7c6fdc11744d7ef09204d372c6ee54dda2d1e8c90e31f6d", 16);

  str2clint_l (r3_l, "3", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);

  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);

  return 0;
}


static int divrem_test (unsigned int nooftests)
{
  unsigned int i;
  CLINTD d_l;

  printf ("Test of division vs. multiplication and addition...\n");

  /* Test with max_l */
  setmax_l (r1_l);
  rand_l (r2_l, CLINTMAXBIT);
  div_l (r1_l, r2_l, r3_l, r4_l);
  mul_l (r2_l, r3_l, r5_l);
  add_l (r5_l, r4_l, r6_l);
  check (r6_l, r1_l, 1, __LINE__);

  /* Test with double long dividend */
  rand_l (d_l, CLINTMAXBIT << 1);
  rand_l (r2_l, CLINTMAXBIT);
  div_l (d_l, r2_l, r3_l, r4_l);
  mul_l (r2_l, r3_l, r5_l);
  add_l (r5_l, r4_l, r6_l);
  mod2_l (d_l, 4096, d_l);
  check (r6_l, d_l, 1, __LINE__);

  /* General tests with leading zeros */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      div_l (r1_l, r2_l, r3_l, r4_l);
      mul_l (r2_l, r3_l, r5_l);
      add_l (r5_l, r4_l, r6_l);
      check (r6_l, r1_l, i, __LINE__);
    }

  /* General tests in accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r7_l, r1_l);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r8_l, r2_l);
      div_l (r1_l, r2_l, r1_l, r2_l);
      mul_l (r8_l, r1_l, r5_l);
      add_l (r5_l, r2_l, r6_l);
      check (r6_l, r7_l, i, __LINE__);
    }

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/

      div_l (r1_l, r2_l, r3_l, r4_l);
      mul_l (r2_l, r3_l, r5_l);
      add_l (r5_l, r4_l, r6_l);
      check (r6_l, r1_l, i, __LINE__);
    }


  /* Euclidean Test  a = b * q1 + r1, b = c * q2 + r2   */
  /*             ==> a = c * q1 * q2 + r3 * q1 + r1     */

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      nzrand_l (r3_l, CLINTRNDLN); /*lint !e666*/

      div_l (r1_l, r2_l, r4_l, r5_l); /* r1_l = r2_l * r4_l + r5_l */
      div_l (r2_l, r3_l, r6_l, r7_l); /* r2_l = r3_l * r6_l + r7_l */

      mul_l (r3_l, r6_l, r8_l);
      mul_l (r8_l, r4_l, r8_l);
      mul_l (r7_l, r4_l, r9_l);
      add_l (r8_l, r9_l, r9_l);
      add_l (r9_l, r5_l, r9_l); /* r9_l = r3_l*r6_l*r4_l + r7_l*r4_l + r5_l */

      check (r9_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int mod_test (unsigned int nooftests)
{
  unsigned int i, j;
  clint n;
  clintd k;
  printf ("Test reduction with mod_l, mod2_l, and umod_l...\n");


  /* Test of mod_l */

  if (0 == mod_l (one_l, nul_l, r1_l))
    {
      fprintf (stderr, "Error: Reduction by 0 not detected by mod_l() in line %d\n", __LINE__);
      exit (-1);
    }

  /* Test with modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  mod_l (r1_l, one_l, r3_l);
  check (r3_l, nul_l, 0, __LINE__);

  /* General tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/

      div_l (r1_l, r2_l, r3_l, r4_l);
      mod_l (r1_l, r2_l, r5_l);
      check (r5_l, r4_l, i, __LINE__);
    }


  /* Test of mod2_l */

  /* Test of mod2_l with 0 */
  mod2_l (nul_l, 0, r0_l);
  mod_l (nul_l, one_l, r4_l);
  check (r0_l, nul_l, i, __LINE__);
  check (r4_l, nul_l, i, __LINE__);

  k = 0;
  rand_l (r0_l, CLINTRNDLN);
  cpy_l (r1_l, r0_l);
  cpy_l (r2_l, one_l);
  mod2_l (r0_l, k, r0_l);
  mod_l (r1_l, one_l, r4_l);
  check (r0_l, r4_l, i, __LINE__);


  /* Test of mod2_l in accumulator mode */
  for (i = 1; i <= nooftests; i++)
    {
      k = ulrand64_l () % MAXTESTLEN;
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      cpy_l (r2_l, one_l);
      for (j = 1; j <= k; j++)
        {
          mul_l (r2_l, two_l, r2_l);
        }
      mod2_l (r0_l, k, r0_l);
      mod_l (r1_l, r2_l, r4_l);
      check (r0_l, r4_l, i, __LINE__);
    }


  /* Test of umod_l() */

  /* Reduction by 0 */
  if (BASEMINONE != umod_l (one_l, 0))
    {
      fprintf (stderr, "Error: Reduction by 0 not detected by umod_l() in line %d\n", __LINE__);
      exit (-1);
    }

  /* Modulus = 1 */
  if (0 != umod_l (two_l, 1))
    {
      fprintf (stderr, "Error: Error in reduction mod 1 in umod_l() in line %d\n", __LINE__);
      exit (-1);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      n = usrand64_l ();
      u2clint_l (r1_l, n);

      u2clint_l (r2_l, umod_l (r0_l, n));
      if (!mequ_l (r0_l, r2_l, r1_l))
        {
          fprintf (stderr, "Error in umod_l() in test %d/line %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int check (CLINT a_l, CLINT b_l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Error in vcheck_l(a_l) in line %d\n", line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }


  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Error in div_l() in test %d/line %d\n", test, line);
      disperr_l ("Dividend = ", r1_l); /*lint !e666 */
      disperr_l ("Divisor = ", r2_l);  /*lint !e666 */
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}


static void ldzrand_l (CLINT n_l, int bits)
{
  unsigned int i;
  rand_l (n_l, bits);
  if (digits_l (n_l) == CLINTMAXDIGIT)
    {
      decdigits_l (n_l);
    }

  for (i = digits_l (n_l) + 1; i <= CLINTMAXDIGIT; i++)
    {
      n_l[i] = 0;
    }
  i = digits_l (n_l);
  do
    {
      setdigits_l (n_l, digits_l (n_l) + ( ulrand64_l () % ( CLINTMAXDIGIT - i + 1 )));
    }
  while (vcheck_l (n_l) != E_VCHECK_LDZ);
}

