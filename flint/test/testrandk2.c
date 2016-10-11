/******************************************************************************/
/*                                                                            */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testrandk2.c     Revision: 08.02.2003                               */
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
/* Compile with                                                               */
/*                                                                            */
/* gcc -O2 -o testrandk2{.exe} testrandk2.c                                   */
/*            flint.c ripemd.c sha1.c aes.c random.c entropy.c                */
/*                                                                            */
/* Call by                                                                    */
/*                                                                            */
/******************************************************************************/

#define RUNS2MAX_AIS        1421
#define RUNS3MIN_AIS         502
#define RUNS3MAX_AIS         748
#define RUNS4MIN_AIS         233
#define RUNS4MAX_AIS         402
#define RUNS5MIN_AIS          90
#define RUNS5MAX_AIS         223
#define RUNS6MIN_AIS          90
#define RUNS6MAX_AIS         233
#define LONGRUN_AIS           34


static UCHAR mask[] = {1, 2, 4, 8, 16, 32, 64, 128};

static int SUMRUNS[8];
static int SUMCONTI;
static int SUMAUTO;
static float SUMPOKER;
static int SUMMONO;

static int AUTOMIN;
static int AUTOMAX;
static int RUNSMIN[8];
static int RUNSMAX[8];
static int LONGRUN;
static float POKERMIN;
static float POKERMAX;
static int MONOMIN;
static int MONOMAX;

static int verbose = 0;

int
main (int argc, char **argv)
{
  char fname[80];
  int i, j, k, strict = 0, testno = 1, error = 0;
  UCHAR *buffer;
  CLINT seed_l;
  STATEPRNG state;
  FILE *filep;

  char *lp;
  strcpy (fname, "sample.bin");

  state.Generator = 0;

  printf ("Testmodule %s, compiled for FLINT/C-Library Version %s\n", __FILE__, verstr_l ());

  if (argc > 1)
    {
      while (--argc > 0 && ( *++argv )[0] == '-')
        {
          lp = *argv + 1;
          switch (*lp)
            {
              case 'f':
                while (isspace (*(++lp))) { /* do nothing */ }
                strcpy (fname, lp);
                break;
              case 's': /* strict' */
                strict = 1;
                break;
	      case 'a':
                InitRand_l (&state, "", 0, 200, FLINT_RNDAES);
                printf ("Testing generator RANDAES\n");
                break;
	      case 'r':
                InitRand_l (&state, "", 0, 200, FLINT_RNDRMDSHA1);
                printf ("Testing generator RANDRMDSHA1\n");
                break;
	      case 'b':
                InitRand_l (&state, "", 0, 200, FLINT_RNDBBS);
                printf ("Testing generator RANDBBS\n");
                break;
	      case '6':
                InitRand_l (&state, "", 0, 200, FLINT_RND64);
                printf ("Testing generator RAND64\n");
                break;
	      case 'v':
                verbose = 1;
                break;
              default:
                printf ("testrandk2 [-arb6fsv]\n");
                exit (-1);
            }
        }
    }
 
  if (!state.Generator)
    {
      InitRand_l (&state, "", 0, 200, FLINT_RNDRMDSHA1);
      printf ("Testing generator RANDRMDSHA1\n");
    }

  create_reg_l ();

  if (strict)
    {
      printf ("Mode is strict: FIPS 140-2 thresholds apply\n");
    }
  InitGlobals (strict);

  if (error)
    {
      fprintf (stderr, "Required entropy not generated\n");
    }

  if ((buffer = (UCHAR*)malloc(2500)) == NULL)
    {
      fprintf (stderr, "Can't allocate memory buffer - aborting\n");
      exit (-1);
    }

  for (i = 1, testno = 0; i <= 257; i++)
    {
      for (j = 0; j < 2500; j++)
        {
          buffer[j] = bRand_l (&state);
        }

      ++testno;
      MonobitTest (buffer, 20000);
      ContinuousTest (buffer, 20000);
      PokerTest (buffer, 20000);
      RunsTest (buffer, 20000);
      AutocorrelationTest (buffer, 20000);
     
      if (verbose)
	{
          printf ("Test sequence %d passed.\n", testno);
	}

      if (0 != CreateFile (fname, 20000, &state))
        {
          fprintf (stderr,"Error in CreateFile() while creating file %s\n", fname);
          exit (-1);
        }

      if ((filep = fopen (fname, RMODE)) == NULL)
        {
          fprintf (stderr, "%s can not be opened for reading - aborting\n", fname);
          exit (-1);
        }

      for (k = 1; k <= RNDBITS; k++)
        {
          if (fread(buffer, 1, 2500, filep) < 2500) 
            {
              fprintf (stderr, "Error while reading file %s - aborting\n", fname);
              exit (-1);
            }

          ++testno;
          MonobitTest (buffer, 20000);
          ContinuousTest (buffer, 20000);
          PokerTest (buffer, 20000);
          RunsTest (buffer, 20000);
          AutocorrelationTest (buffer, 20000);

          if (verbose)
	    {
              printf ("Test sequence %d passed.\n", testno);
	    }
        }

      if (verbose)
	{
          printf ("Test:\t\tAverage value:\ttolerable interval:\n"); 

          printf ("Monobit\t\t%.2f\t[%d,%d]\n", 
                   (float)SUMMONO/(float)testno, MONOMIN, MONOMAX);
          printf ("Continuous\t%f\n", 
                   (float)SUMCONTI/testno);
          printf ("Poker\t\t%.2f\t\t[%.2f,%.2f]\n", 
                   (double)SUMPOKER/(double)testno, POKERMIN, POKERMAX);
          printf ("Runs Length 1\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[1]/(testno*2), RUNSMIN[1], RUNSMAX[1]);
          printf ("Runs Length 2\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[2]/(testno*2), RUNSMIN[2], RUNSMAX[2]);
          printf ("Runs Length 3\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[3]/(testno*2), RUNSMIN[3], RUNSMAX[3]);
          printf ("Runs Length 4\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[4]/(testno*2), RUNSMIN[4], RUNSMAX[4]);
          printf ("Runs Length 5\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[5]/(testno*2), RUNSMIN[5], RUNSMAX[5]);
          printf ("Runs Length 6\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[6]/(testno*2), RUNSMIN[6], RUNSMAX[6]);
          printf ("Longruns\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMRUNS[7]/(testno*2), RUNSMIN[7], RUNSMAX[7]);
          printf ("Autocorrelation\t%.2f\t\t[%d,%d]\n", 
                   (float)SUMAUTO/(float)testno, AUTOMIN, AUTOMAX);
	}
    }

  printf ("Test:\t\tAverage value:\ttolerable interval:\n"); 

  printf ("Monobit\t\t%.2f\t[%d,%d]\n", 
           (float)SUMMONO/(float)testno, MONOMIN, MONOMAX);
  printf ("Continuous\t%f\n", 
           (float)SUMCONTI/testno);
  printf ("Poker\t\t%.2f\t\t[%.2f,%.2f]\n", 
           (double)SUMPOKER/(double)testno, POKERMIN, POKERMAX);
  printf ("Runs Length 1\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[1]/(testno*2), RUNSMIN[1], RUNSMAX[1]);
  printf ("Runs Length 2\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[2]/(testno*2), RUNSMIN[2], RUNSMAX[2]);
  printf ("Runs Length 3\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[3]/(testno*2), RUNSMIN[3], RUNSMAX[3]);
  printf ("Runs Length 4\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[4]/(testno*2), RUNSMIN[4], RUNSMAX[4]);
  printf ("Runs Length 5\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[5]/(testno*2), RUNSMIN[5], RUNSMAX[5]);
  printf ("Runs Length 6\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[6]/(testno*2), RUNSMIN[6], RUNSMAX[6]);
  printf ("Longruns\t%.2f\t\t[%d,%d]\n", 
           (float)SUMRUNS[7]/(testno*2), RUNSMIN[7], RUNSMAX[7]);
  printf ("Autocorrelation\t%.2f\t\t[%d,%d]\n", 
           (float)SUMAUTO/(float)testno, AUTOMIN, AUTOMAX);

  fclose (filep);
  free (buffer);

  rand_test (100, &state);

  free_reg_l ();

#ifdef DEBUG
  MemDumpUnfreed ();
#endif
  printf ("Alle Tests in %s fehlerfrei durchlaufen :-)\n", __FILE__);

  return 0;
}


static int CreateFile (char *fname, int noofbits, STATEPRNG *state)
{
  FILE *filep;  
  UCHAR byte;
  int i, error = 0;
  
  filep = fopen (fname, WMODE);
  if (filep == NULL)
    {
      fprintf (stderr, "%s can not be opened for writing - aborting\n", fname);
      exit (-1);
    }

  if (verbose)
    {
      printf ("creating file %s\n", fname);
    }

  for (i = 0; i < noofbits; i++)
    {
      byte = bRand_l (state);

      if (fwrite (&byte, 1, 1, filep) < 1)
        {
          error = 1;
          break;
        }
    }

  fclose (filep);
  return error;
}


int 
MonobitTest (UCHAR *buffer, int noofbits)
{
  unsigned j;
  int i, noofones = 0;

#ifdef VERBOSE  
  printf ("Entering Monobit Test\n");
#endif

  for (i = 0; i < noofbits/8; i++)
    {
      for (j = 1; j < 256; j <<= 1)
        {
          if (*(buffer + i) & j) ++noofones;
        }
    }

  SUMMONO += noofones;

  if ((MONOMIN <= noofones) && (noofones <= MONOMAX))
    {
#ifdef VERBOSE  
      printf ("Monobit Test passed.\n");
#endif  
      return 0;
    }
  else
    {
      printf ("Monobit Test: %d NOT passed.\n", noofones);
      printf ("Tolerable interval: [%d, %d]\n", MONOMIN, MONOMAX);
      exit (-1);
    }
}


int 
AutocorrelationTest (UCHAR *buffer, int noofbits)
{
  unsigned i, j;
  int imod8, t, timod8, tmin, tmax, ac, automin = 10000, automax = 0;
  
#ifdef VERBOSE  
  printf ("Entering Autocorrelation Test\n");
#endif

  for (t = 1; t <= noofbits/4; t++)
    {
      ac = 0;
      for (i = 0, j = t; i < noofbits/4; i++, j++)
        {
          imod8 = i & 7;
          timod8 = j & 7;

          if ( ((*(buffer + (i >> 3)) & mask[imod8]) >> (imod8)) ^
               ((*(buffer + (j >> 3)) & mask[timod8]) >> (timod8)) )
            {
              ++ac;
            }
        }

      if (ac > automax)
        {
          tmax = t;
          automax = ac;
        }
      else
        if (ac < automin)
          {
            tmin = t;
            automin = ac;
          }
    }

  if (abs (2500 - automin) > abs (automax - 2500))
    {
      t = tmin; /*lint !e644*/
    }
  else
    {
      t = tmax; /*lint !e644*/
    }

  ac = 0;
  for (i = noofbits/4, j = t + noofbits/4; i < noofbits/2; i++, j++)
    {
      imod8 = i & 7;
      timod8 = j & 7;

      if ( ((*(buffer + (i >> 3)) & mask[imod8]) >> (imod8)) ^
           ((*(buffer + (j >> 3)) & mask[timod8]) >> (timod8)) )
        {
          ++ac;
        }
    }  

  SUMAUTO += ac;

  if ((ac < AUTOMIN) || (ac > AUTOMAX))
    {
      printf ("Autocorrelation of sequence of %d bits, ", noofbits/2);
      printf ("distance %d: %d\n", t, ac);
      printf ("Autocorrelation Test NOT passed.\n");
      printf ("Tolerable interval: [%d,%d]\n", AUTOMIN, AUTOMAX);
      exit (-1);
    }

#ifdef VERBOSE  
  printf ("Autocorrelation Test passed.\n");
#endif
  return 0;
}


int 
ContinuousTest (UCHAR *buffer, int noofbits)
{
  int block, error = 0, continuous = 0;
  
#ifdef VERBOSE  
  printf ("Entering Continuous Test\n");
#endif
  for (block = 1; block < noofbits/16; block+=2, buffer += 2)
    {

#ifdef DEBUG_TEST
      printf("%d ", (USHORT)(*buffer << 8 | *(buffer + 1)));
      printf("%d ", (USHORT)(*(buffer + 2) << 8 | *(buffer + 3)));
      printf ("block = %d\n", block);
#endif /* DEBUG_TEST? */
      
      if ((USHORT)(*buffer       << 8 | *(buffer + 1)) == 
          (USHORT)(*(buffer + 2) << 8 | *(buffer + 3)))
        {
          continuous++;
#ifdef DEBUG_TEST
          printf("%d ", (USHORT)(*buffer << 8 | *(buffer + 1)));
          printf("%d ", (USHORT)(*(buffer + 2) << 8 | *(buffer + 3)));
          printf ("block = %d\n", block);
          printf ("Block: %d\n", block);      
#endif /* DEBUG_TEST? */
        }
    }

  SUMCONTI += continuous;

  if (verbose && continuous)
    {          
      printf ("Continuous Test of sequence of 20.000 bits, ");
      printf ("processing 1249 continuous blocks of 16 bits.\n");
      printf ("%d continuous blocks where duplicate", continuous*2);
      printf (" where there should be 0.\n");
      error = -1;
    }
  else
    {
#ifdef VERBOSE  
      printf ("Continuous Test passed.\n");
#endif
    }

  return error;
}


int 
RunsTest (UCHAR *buffer, int noofbits)
{
  int run, i, bitpos, error = 0, runs[8];
  
#ifdef VERBOSE  
  printf ("Entering Runs Test\n");
#endif

  for (i = 0; i < 8; i++) runs[i] = 0;

  for (bitpos = 0; bitpos < noofbits; bitpos++)
    {
      run = CollectRun (buffer, &bitpos, 0, noofbits);

      if (run >= LONGRUN)
        {
          ++runs[7];  /* Longrun */
          ++runs[6];  /* Run >= 6 */
        }
      else
        if (run >= 6) ++runs[6]; /* Run >= 6 but no Longrun */
          else
            ++runs[run]; /* Run < 6 */
    }

  SUMRUNS[1] += runs[1];
  SUMRUNS[2] += runs[2];
  SUMRUNS[3] += runs[3];
  SUMRUNS[4] += runs[4];
  SUMRUNS[5] += runs[5];
  SUMRUNS[6] += runs[6];
  SUMRUNS[7] += runs[7];

  if ((runs[1] >= RUNSMIN[1]) && (runs[1] <= RUNSMAX[1]) &&
      (runs[2] >= RUNSMIN[2]) && (runs[2] <= RUNSMAX[2]) &&
      (runs[3] >= RUNSMIN[3]) && (runs[3] <= RUNSMAX[3]) &&
      (runs[4] >= RUNSMIN[4]) && (runs[4] <= RUNSMAX[4]) &&
      (runs[5] >= RUNSMIN[5]) && (runs[5] <= RUNSMAX[5]) &&
      (runs[6] >= RUNSMIN[6]) && (runs[6] <= RUNSMAX[6]))
    {
#ifdef VERBOSE  
      printf ("Runs Test for runs of type 0 passed.\n");
#endif
    }
  else
    {
      printf ("Runs of length 1: %d\n", runs[1]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[1], RUNSMAX[1]);
      printf ("Runs of length 2: %d\n", runs[2]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[2], RUNSMAX[2]);
      printf ("Runs of length 3: %d\n", runs[3]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[3], RUNSMAX[3]);
      printf ("Runs of length 4: %d\n", runs[4]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[4], RUNSMAX[4]);
      printf ("Runs of length 5: %d\n", runs[5]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[5], RUNSMAX[5]);
      printf ("Runs of length 6 or longer: %d\n", runs[6]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[6], RUNSMAX[6]);
      printf ("Runs Test for runs of type 0 NOT passed.\n");
      error = -1;
    }

  if (runs[7])
    {
      printf ("Longruns Test for runs of type 0 NOT passed.\n");
      error = -1;
    }
  else
    {
#ifdef VERBOSE  
      printf ("Longruns Test for runs of type 0 passed.\n");
#endif
    }

  for (i = 0; i < 8; i++) runs[i] = 0;

  for (bitpos = 0; bitpos < noofbits; bitpos++)
    {
      run = CollectRun (buffer, &bitpos, 1, noofbits);

      if (run >= LONGRUN)
        {
          ++runs[7];  /* Longrun */
          ++runs[6];  /* Run >= 6 */
        }
      else
        if (run >= 6) ++runs[6]; /* Run >= 6 but no Longrun */
          else
            ++runs[run]; /* Run < 6 */
    }

  SUMRUNS[1] += runs[1];
  SUMRUNS[2] += runs[2];
  SUMRUNS[3] += runs[3];
  SUMRUNS[4] += runs[4];
  SUMRUNS[5] += runs[5];
  SUMRUNS[6] += runs[6];
  SUMRUNS[7] += runs[7];

  if ((runs[1] >= RUNSMIN[1]) && (runs[1] <= RUNSMAX[1]) &&
      (runs[2] >= RUNSMIN[2]) && (runs[2] <= RUNSMAX[2]) &&
      (runs[3] >= RUNSMIN[3]) && (runs[3] <= RUNSMAX[3]) &&
      (runs[4] >= RUNSMIN[4]) && (runs[4] <= RUNSMAX[4]) &&
      (runs[5] >= RUNSMIN[5]) && (runs[5] <= RUNSMAX[5]) &&
      (runs[6] >= RUNSMIN[6]) && (runs[6] <= RUNSMAX[6]))
    {
#ifdef VERBOSE  
      printf ("Runs Test for runs of type 1 passed.\n");
#endif
    }
  else
    {
      printf ("Runs of length 1: %d\n", runs[1]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[1], RUNSMAX[1]);
      printf ("Runs of length 2: %d\n", runs[2]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[2], RUNSMAX[2]);
      printf ("Runs of length 3: %d\n", runs[3]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[3], RUNSMAX[3]);
      printf ("Runs of length 4: %d\n", runs[4]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[4], RUNSMAX[4]);
      printf ("Runs of length 5: %d\n", runs[5]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[5], RUNSMAX[5]);
      printf ("Runs of length 6 or longer: %d\n", runs[6]);
      printf ("Tolerable interval: [%d, %d]\n", RUNSMIN[6], RUNSMAX[6]);
      printf ("Runs Test for runs of type 1 NOT passed.\n");
      error = -1;
    }

  if (runs[7])
    {
      printf ("Longruns Test for runs of type 1 NOT passed.\n");
      error = -1;
    }
  else
    {
#ifdef VERBOSE  
      printf ("Longruns Test for runs of type 1 passed.\n");
#endif  
    }

  if (error) exit (-1);

  return 0;
}


int 
PokerTest (UCHAR *buffer, int noofbits)
{
  int i, t, n4 = noofbits/4, f[16];
  float poker;

#ifdef VERBOSE  
  printf ("Entering Poker Test\n");
#endif

  for (i = 0; i < 16; i++)
      f[i] = 0;

  for (i = 0; i < noofbits/8; i++)
    {
      f[*buffer & 0x0f]++;
      f[(*buffer & 0xf0) >> 4]++;
      ++buffer;
    }

  for (i = 0, t = 0; i < 16; i++)
      t += f[i] * f[i];
  
  poker = (float)((float)16.0 * (float)t / (float)(n4)) - n4;

  SUMPOKER += poker;
  
  if ((poker > POKERMIN) && (poker < POKERMAX))
    {
#ifdef VERBOSE  
      printf ("Poker Test passed.\n");
#endif
    }
   else
    {
      printf ("Poker Test: %.2f NOT passed.\n", poker);
      printf ("Tolerable interval: [%.2f, %.2f]\n", POKERMIN, POKERMAX);
      exit (-1);
    }

  return 0;
}      


int 
CollectRun (UCHAR *buffer, int *bitpos, int val, int noofbits)
{
  int run = 0;
  while ((*bitpos < noofbits) && GetBit (buffer, *bitpos) == val)
    {
      run++;
      (*bitpos)++;
    }

  return run;
}


int
GetBit (UCHAR *buffer, int bitpos) /* bitpos from 0 to n -1 */
{
  return ((*(buffer + (bitpos >> 3)) & mask[bitpos & 7]) >> (bitpos & 7));
}



void InitGlobals (int strict)
{
  if (strict)
    {
      AUTOMIN    = AUTOMIN_FIPS;
      AUTOMAX    = AUTOMAX_FIPS;  
      RUNSMIN[1] = RUNS1MIN_FIPS;
      RUNSMIN[2] = RUNS2MIN_FIPS;
      RUNSMIN[3] = RUNS3MIN_FIPS;
      RUNSMIN[4] = RUNS4MIN_FIPS;
      RUNSMIN[5] = RUNS5MIN_FIPS;
      RUNSMIN[6] = RUNS6MIN_FIPS;
      RUNSMIN[7] = 0;             /* No Longruns */
      RUNSMAX[1] = RUNS1MAX_FIPS;
      RUNSMAX[2] = RUNS2MAX_FIPS;
      RUNSMAX[3] = RUNS3MAX_FIPS;
      RUNSMAX[4] = RUNS4MAX_FIPS;
      RUNSMAX[5] = RUNS5MAX_FIPS;
      RUNSMAX[6] = RUNS6MAX_FIPS;
      RUNSMAX[7] = 0;             /* No Longruns */
      LONGRUN    = LONGRUN_FIPS;
      POKERMIN   = POKERMIN_FIPS;
      POKERMAX   = POKERMAX_FIPS;
      MONOMIN    = MONOMIN_FIPS;
      MONOMAX    = MONOMAX_FIPS;
    }
  else
    {
      AUTOMIN    = AUTOMIN_AIS;
      AUTOMAX    = AUTOMAX_AIS;  
      RUNSMIN[1] = RUNS1MIN_AIS;
      RUNSMIN[2] = RUNS2MIN_AIS;
      RUNSMIN[3] = RUNS3MIN_AIS;
      RUNSMIN[4] = RUNS4MIN_AIS;
      RUNSMIN[5] = RUNS5MIN_AIS;
      RUNSMIN[6] = RUNS6MIN_AIS;
      RUNSMIN[7] = 0;             /* No Longruns */
      RUNSMAX[1] = RUNS1MAX_AIS;
      RUNSMAX[2] = RUNS2MAX_AIS;
      RUNSMAX[3] = RUNS3MAX_AIS;
      RUNSMAX[4] = RUNS4MAX_AIS;
      RUNSMAX[5] = RUNS5MAX_AIS;
      RUNSMAX[6] = RUNS6MAX_AIS;
      RUNSMAX[7] = 0;             /* No Longruns */
      LONGRUN    = LONGRUN_AIS;
      POKERMIN   = POKERMIN_AIS;
      POKERMAX   = POKERMAX_AIS;
      MONOMIN    = MONOMIN_AIS;
      MONOMAX    = MONOMAX_AIS;
    }

  SUMRUNS[1] = 0;
  SUMRUNS[2] = 0;
  SUMRUNS[3] = 0;
  SUMRUNS[4] = 0;
  SUMRUNS[5] = 0;
  SUMRUNS[6] = 0;
  SUMRUNS[7] = 0;

  SUMAUTO = 0;
  SUMCONTI = 0;
  SUMPOKER = 0.0;
  SUMMONO = 0;
}


static int rand_test (unsigned int nooftests, STATEPRNG *state)
{
  unsigned i;
  unsigned r;
  INITRAND64_LT();

  switch (state->Generator)
    {
      case FLINT_RNDAES:
        printf ("Test RandAES_l ...\n");
        RandAES_l (r0_l, &state->StateAES, 0);
        RandAES_l (r1_l, &state->StateAES, 1);
        break;
      case FLINT_RNDRMDSHA1:
        printf ("Test RandRMDSHA1_l ...\n");
        RandRMDSHA1_l (r0_l, &state->StateRMDSHA1, 0);
        RandRMDSHA1_l (r1_l, &state->StateRMDSHA1, 1);
        break;
      case FLINT_RNDBBS:
        printf ("Test RandBBS_l ...\n");
        RandBBS_l (r0_l, &state->StateBBS, 0);
        RandBBS_l (r1_l, &state->StateBBS, 1);
        break;
      case FLINT_RND64:
        printf ("Test rand_l ...\n");
        rand_l (r0_l, 0);
        rand_l (r1_l, 1);
        break;
      default:
	;
    }

  if (!eqz_l (r0_l))
    {
      fprintf (stderr, "Fehler bei RandXXX_l() bei Zeile %d\n", __LINE__);
      disperr_l ("r0_l = ", r0_l);
      exit (-1);
    }

  if (!equ_l (r1_l, one_l))
    {
      fprintf (stderr, "Fehler bei RandXXX_l() bei Zeile %d\n", __LINE__);
      disperr_l ("r1_l = ", r0_l);
      exit (-1);
    }

  for (i = 0; i < nooftests; i++)
    {
      r = ulrand64_l () % CLINTMAXDIGIT;
      switch (state->Generator)
        {
          case FLINT_RNDAES:
            RandAES_l (r0_l, &state->StateAES, r);
            break;
          case FLINT_RNDRMDSHA1:
            RandRMDSHA1_l (r0_l, &state->StateRMDSHA1, r);
            break;
          case FLINT_RNDBBS:
            RandBBS_l (r0_l, &state->StateBBS, r);
            break;
          case FLINT_RND64:
            rand_l (r0_l, r);
            break;
	  default:
	    ;
        }

      if (ld_l (r0_l) != r)
        {
          fprintf (stderr, "Fehler bei RandXXX_l() in Zeile %d\n", __LINE__);
          disperr_l ("r0_l = ", r0_l);
          fprintf (stderr, "r = %d\n", r);
          exit (-1);
        }
    }

  return 0;
}
