/******************************************************************************/
/*                                                                            */
/* Functions for arithmetic and number theory with large integers in C        */
/* Software supplement to the book "Cryptography in C and C++"                */
/* by Michael Welschenbach                                                    */
/*                                                                            */
/* Module testaes.c        Revision: 20.08.2002                               */
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
#include "aes.h"
#include "flint.h"

#if defined __IBMC__ || defined _MSC_VER
#define MODE "r"
#else
#define MODE "rt"
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 80
#endif

static int testmc (char *fname);
static int testnkt_vkt (char *fname);
static void testcbc (void);
static int scan_bytes (char *Linep, char *buffer, int noofbytes);
static void TestCmp (char * test, char * aesres, char * ref, int Length);

char *fnames[]={
  "cbcmd44.txt",
  "cbcmd46.txt",
  "cbcmd48.txt",
  "cbcmd64.txt",
  "cbcmd66.txt",
  "cbcmd68.txt",
  "cbcmd84.txt",
  "cbcmd86.txt",
  "cbcmd88.txt",
   
  "cbcme44.txt",
  "cbcme46.txt",
  "cbcme48.txt",
  "cbcme64.txt",
  "cbcme66.txt",
  "cbcme68.txt",
  "cbcme84.txt",
  "cbcme86.txt",
  "cbcme88.txt",
   
  "ecbmd44.txt",
  "ecbmd46.txt",
  "ecbmd48.txt",
  "ecbmd64.txt",
  "ecbmd66.txt",
  "ecbmd68.txt",
  "ecbmd84.txt",
  "ecbmd86.txt",
  "ecbmd88.txt",
   
  "ecbme44.txt",
  "ecbme46.txt",
  "ecbme48.txt",
  "ecbme64.txt",
  "ecbme66.txt",
  "ecbme68.txt",
  "ecbme84.txt",
  "ecbme86.txt",
  "ecbme88.txt",
   
  "ecbnk44.txt",
  "ecbnk46.txt",
  "ecbnk48.txt",
  "ecbnk64.txt",
  "ecbnk66.txt",
  "ecbnk68.txt",
  "ecbnk84.txt",
  "ecbnk86.txt",
  "ecbnk88.txt",
   
  "ecbnt44.txt",
  "ecbnt46.txt",
  "ecbnt48.txt",
  "ecbnt64.txt",
  "ecbnt66.txt",
  "ecbnt68.txt",
  "ecbnt84.txt",
  "ecbnt86.txt",
  "ecbnt88.txt",
   
  "ecbvk44.txt",
  "ecbvk46.txt",
  "ecbvk48.txt",
  "ecbvk64.txt",
  "ecbvk66.txt",
  "ecbvk68.txt",
  "ecbvk84.txt",
  "ecbvk86.txt",
  "ecbvk88.txt",
   
  "ecbvt44.txt",
  "ecbvt46.txt",
  "ecbvt48.txt",
  "ecbvt64.txt",
  "ecbvt66.txt",
  "ecbvt68.txt",
  "ecbvt84.txt",
  "ecbvt86.txt",
  "ecbvt88.txt",
  "end-of-list"
};

UCHAR IV1[] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

UCHAR PT1[] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

UCHAR KEY1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

UCHAR CT1[] =  {0x66, 0xE9, 0x4B, 0xD4, 0xEF, 0x8A, 0x2C, 0x3B, 
                0x88, 0x4C, 0xFA, 0x59, 0xCA, 0x34, 0x2B, 0x2E, 
                0xF7, 0x95, 0xBD, 0x4A, 0x52, 0xE2, 0x9E, 0xD7, 
                0x13, 0xD3, 0x13, 0xFA, 0x20, 0xE9, 0x8D, 0xBC, 
                0xA1, 0x0C, 0xF6, 0x6D, 0x0F, 0xDD, 0xF3, 0x40, 
                0x53, 0x70, 0xB4, 0xBF, 0x8D, 0xF5, 0xBF, 0xB3, 
                0x47, 0xC7, 0x83, 0x95, 0xE0, 0xD8, 0xAE, 0x21, 
                0x94, 0xDA, 0x0A, 0x90, 0xAB, 0xC9, 0x88, 0x8A};

UCHAR DCT1[] = {0x14, 0x0F, 0x0F, 0x10, 0x11, 0xB5, 0x22, 0x3D, 
                0x79, 0x58, 0x77, 0x17, 0xFF, 0xD9, 0xEC, 0x3A, 
                0x14, 0x0F, 0x0F, 0x10, 0x11, 0xB5, 0x22, 0x3D, 
                0x79, 0x58, 0x77, 0x17, 0xFF, 0xD9, 0xEC, 0x3A, 
                0x14, 0x0F, 0x0F, 0x10, 0x11, 0xB5, 0x22, 0x3D, 
                0x79, 0x58, 0x77, 0x17, 0xFF, 0xD9, 0xEC, 0x3A, 
                0x14, 0x0F, 0x0F, 0x10, 0x11, 0xB5, 0x22, 0x3D, 
                0x79, 0x58, 0x77, 0x17, 0xFF, 0xD9, 0xEC, 0x3A};
 

UCHAR IV2[] =  {0x8A, 0x05, 0xFC, 0x5E, 0x09, 0x5A, 0xF4, 0x84, 
                0x8A, 0x08, 0xD3, 0x28, 0xD3, 0x68, 0x8E, 0x3D};

UCHAR PT2[] =  {0x20, 0x4F, 0x17, 0xE2, 0x44, 0x43, 0x81, 0xF6, 
                0x11, 0x4F, 0xF5, 0x39, 0x34, 0xC0, 0xBC, 0xD3, 
                0x20, 0x4F, 0x17, 0xE2, 0x44, 0x43, 0x81, 0xF6, 
                0x11, 0x4F, 0xF5, 0x39, 0x34, 0xC0, 0xBC, 0xD3, 
                0x20, 0x4F, 0x17, 0xE2, 0x44, 0x43, 0x81, 0xF6, 
                0x11, 0x4F, 0xF5, 0x39, 0x34, 0xC0, 0xBC, 0xD3, 
                0x20, 0x4F, 0x17, 0xE2, 0x44, 0x43, 0x81, 0xF6, 
                0x11, 0x4F, 0xF5, 0x39, 0x34, 0xC0, 0xBC, 0xD3};

UCHAR KEY2[] = {0x8A, 0x05, 0xFC, 0x5E, 0x09, 0x5A, 0xF4, 0x84, 
                0x8A, 0x08, 0xD3, 0x28, 0xD3, 0x68, 0x8E, 0x3D};

UCHAR CT2[] =  {0xFA, 0x59, 0x05, 0x35, 0xBA, 0x55, 0xAC, 0x78, 
                0x73, 0x56, 0x8A, 0xA5, 0x86, 0x80, 0xB0, 0x22, 
                0x29, 0x89, 0x0C, 0x50, 0x3C, 0x63, 0x17, 0xF2, 
                0xB5, 0x75, 0x0F, 0x52, 0x65, 0x15, 0xE8, 0xF5, 
                0x3E, 0xB0, 0x3A, 0x59, 0x41, 0x15, 0x88, 0x04, 
                0x3F, 0xE4, 0x04, 0x17, 0x96, 0x0E, 0xB1, 0x6A, 
                0x52, 0x39, 0x48, 0x2D, 0x08, 0xE8, 0xC6, 0x1D, 
                0x60, 0x09, 0x38, 0xA9, 0xC5, 0xE6, 0x8B, 0x06};

UCHAR DCT2[] = {0x25, 0xFE, 0x2D, 0xD1, 0xC8, 0xF8, 0xB9, 0xBA, 
                0x0C, 0xAC, 0x3E, 0x0D, 0xD2, 0xF9, 0x9E, 0xB5, 
                0x8F, 0xB4, 0xC6, 0x6D, 0x85, 0xE1, 0xCC, 0xC8, 
                0x97, 0xEB, 0x18, 0x1C, 0x35, 0x51, 0xAC, 0x5B, 
                0x8F, 0xB4, 0xC6, 0x6D, 0x85, 0xE1, 0xCC, 0xC8, 
                0x97, 0xEB, 0x18, 0x1C, 0x35, 0x51, 0xAC, 0x5B, 
                0x8F, 0xB4, 0xC6, 0x6D, 0x85, 0xE1, 0xCC, 0xC8, 
                0x97, 0xEB, 0x18, 0x1C, 0x35, 0x51, 0xAC, 0x5B};
 
int TESTBLOCK = 0;

int main (int argc, char *argv[])
{
  char testfile[_MAX_PATH], testpath[_MAX_PATH], *lp;
  int i = 0;

  printf ("Testmodule %s, compiled for aes.c as part of FLINT/C-Library\n", __FILE__);

  strcpy (testpath, "testvals/") ;
  if (argc > 1)  
    {
      while (--argc > 0 && (*++argv)[0] == '-' ) 
        {
          lp = *argv+1;
          switch (*lp)  
            {
              case 'b': 
                TESTBLOCK = 1;
                break;

              case 't': 
                if (++lp != NULL) 
                  { 
                    strcpy (testpath, lp);
                    strcat (testpath, "/");
                  }
                break;

              default: 
                printf ("Usage: testaes [-b][-t[drive:]path (path of testdata)]\n");
                exit (-1);
            }
         }
    } 

  if (TESTBLOCK)
    {
      printf ("Testing block functions\n");
    }
  else
    {
      printf ("Testing state functions\n");
    }

  while (!strstr (fnames[i], "end-of-list"))
    {
      strcpy (testfile, testpath);
      strcat (testfile, fnames[i]);

      if (strstr (fnames[i], "ecbm") != NULL || 
          strstr (fnames[i], "cbcm") != NULL)
        {
          testmc (testfile);
        }
      else
        {
          testnkt_vkt (testfile);
        }

      ++i;
    }
 
  testcbc ();
}


static int testmc (char *fname)
{
  FILE *filep;  
  AESWORKSPACE ws;
  AESKEYSCHED ks;
  AESXPKEY ExpandedKey, InvExpandedKey;

  char Line[200], *LinePtr;
  UCHAR KeyBuff[32], InBuff[32], OutBuff[32], RefBuff[32];
  UCHAR IVBuff[32], *IVPtr;
  int mode, direction, BlockLength, KeyLength, testno, i, j, error;


  if (strstr (fname, "ecb"))
    {
      mode = AES_ECB;
    }
  else
    {
      mode = AES_CBC;
    }

  filep = fopen (fname, MODE);
  if (filep == NULL)
    {
      fprintf (stderr, "%s can not be opened for reading - aborting\n", fname);
      exit (1);
    }

  /* Read Blocklength */
  do
    {
      fgets (Line, 200, filep);
    }
  while (strstr (Line, "BLOCKSIZE") == NULL);
  LinePtr = strchr (Line, '=');
  while (!isxdigit (*(++LinePtr))) {};
  sscanf (LinePtr, "%ld", &BlockLength);
#ifdef TESTAES_TEST
  printf ("BlockLength = %ld\n", BlockLength);
#endif

  /* Read Keylength */
  do
    {
      fgets (Line, 200, filep);
    }
  while (strstr (Line, "KEYSIZE") == NULL);
  LinePtr = strchr (Line, '=');
  while (!isxdigit (*(++LinePtr))) {};
  sscanf (LinePtr, "%ld", &KeyLength);
#ifdef TESTAES_TEST
  printf ("KeyLength = %ld\n", KeyLength);
#endif

  /* Loop over file */
  while (!feof (filep))
    {
      /* Read test number */
      do
        {
          fgets (Line, 200, filep);
        }
      while (!feof (filep) && (strstr (Line, "TEST") == NULL));

      if (feof (filep))
        {
          break;
        }

      LinePtr = strchr (Line, '=');
      while (!isxdigit (*(++LinePtr))) {};
      sscanf (LinePtr, "%ld", &testno);
#ifdef TESTAES_TEST
      printf ("Test = %ld\n", testno);
#endif

      /* Read key */
      do
        {
          fgets (Line, 200, filep);
        }
      while (strstr (Line, "KEY") == NULL);
      LinePtr = strchr (Line, '=');
      while (!isxdigit (*(++LinePtr))) {};

      scan_bytes (LinePtr, KeyBuff, KeyLength/8);

      /* Read IV if present */
      fgets (Line, 200, filep);
      if (strstr (Line, "IV") != NULL)
        {
          LinePtr = strchr (Line, '=');
          while (!isxdigit (*(++LinePtr))) {};

          scan_bytes (LinePtr, IVBuff, BlockLength/8);
          IVPtr = IVBuff;
        }
      else
        { 
          if (mode == AES_CBC)
            {
              fprintf (stderr, "aestest: Missing IV for CBC mode in test no. %d of file %s\n", testno, fname);
              exit (-1);
            }

          IVPtr = NULL;
        }

      /* Read PT or CT */
      /* Line already in buffer ? */ 
      if (IVPtr != NULL)
        {
          fgets (Line, 200, filep);
        }
      LinePtr = strchr (Line, '=');
      while (!isxdigit (*(++LinePtr))) {};
      scan_bytes (LinePtr, InBuff, BlockLength/8);
      if (strstr (Line, "PT") != NULL)
        {
          direction = AES_ENC;
        }
      else
        {
          if (strstr (Line, "CT") != NULL)
            { 
              direction = AES_DEC;
            }
          else
            {
              fprintf (stderr, "Read error while searching for CT or PT in test %d of file %s\n",
                       testno, fname);
              exit (-1);
            }
        }

      fgets (Line, 200, filep);
      LinePtr = strchr (Line, '=');
      while (!isxdigit (*(++LinePtr))) {};
      scan_bytes (LinePtr, RefBuff, BlockLength/8);
      if ((strstr (Line, "PT") == NULL) && (strstr (Line, "CT") == NULL))
        {
          fprintf (stderr, "Read error while searching for CT or PT in test %d of file %s\n",
                   testno, fname);
          exit (-1);
        }
      
      switch (mode)
        {
          case AES_CBC:
            if (direction == AES_ENC)
              {
                for (i = 0; i < 10000; i++)
                  {
                    for (j = 0; j < BlockLength/8; j++)
                      {
                        InBuff[j] ^= IVBuff[j];
                      }

                    if (TESTBLOCK)
		      {
                        AESEncryptBlock (OutBuff, InBuff, KeyBuff, KeyLength, BlockLength);
		      }
                    else
		      {
                        AESInit (&ws, AES_ECB, BlockLength, NULL, &ks, KeyBuff, KeyLength, AES_ENC);
                        AESCrypt (OutBuff, &ws, &ks, InBuff, BlockLength/8);
		      }

                    memcpy (InBuff, IVBuff, BlockLength/8);
                    memcpy (IVBuff, OutBuff, BlockLength/8);
                  }
              }
            else
              {
                for (i = 0; i < 10000; i++)
                  {
                    if (TESTBLOCK)
		      {
                        AESDecryptBlock (OutBuff, InBuff, KeyBuff, KeyLength, BlockLength);
		      }
                    else
		      {
                        AESInit (&ws, AES_ECB, BlockLength, NULL, &ks, KeyBuff, KeyLength, AES_DEC);
                        AESCrypt (OutBuff, &ws, &ks, InBuff, BlockLength/8);
		      }

                    for (j = 0; j < BlockLength/8; j++) 
                      {
                        OutBuff[j] ^= IVBuff[j];
                      }
                    memcpy (IVBuff, InBuff, BlockLength/8);
                    memcpy (InBuff, OutBuff, BlockLength/8);
                  }
              }
            break;

          case AES_ECB:
            if (direction == AES_ENC)
              {
                for (i = 0; i < 10000; i++)
                  {
                    if (TESTBLOCK)
		      {
                        AESEncryptBlock (OutBuff, InBuff, KeyBuff, KeyLength, BlockLength);
		      }
                    else
		      {
                        AESInit (&ws, AES_ECB, BlockLength, NULL, &ks, KeyBuff, KeyLength, AES_ENC);
                        AESCrypt (OutBuff, &ws, &ks, InBuff, BlockLength/8);
		      }

                    memcpy (InBuff, OutBuff, BlockLength/8);  
                  }
              }
            else
              {
                for (i = 0; i < 10000; i++)
                  {
                    if (TESTBLOCK)
		      {
                        AESDecryptBlock (OutBuff, InBuff, KeyBuff, KeyLength, BlockLength);
		      }
                    else
		      {
                        AESInit (&ws, AES_ECB, BlockLength, NULL, &ks, KeyBuff, KeyLength, AES_DEC);
                        AESCrypt (OutBuff, &ws, &ks, InBuff, BlockLength/8);
		      }

                    memcpy (InBuff, OutBuff, BlockLength/8);  
                  }
              }
            break;
        }

      if (!memcmp (OutBuff, RefBuff, BlockLength/8))
        {
          printf ("Test %d of file %s OK\n", testno, fname);
        }
      else
        {
          printf ("aestest: error in test %d of file %s\n", testno, fname);
          fprintf (stderr, "OutBuff = ");
          for (i = 0; i < BlockLength/8; i++)
            fprintf (stderr, "%.2X", OutBuff[i]);
          fprintf (stderr, "\n");

          fprintf (stderr, "RefBuff = ");
          for (i = 0; i < BlockLength/8; i++)
            fprintf (stderr, "%.2X", RefBuff[i]);
          fprintf (stderr, "\n");

          exit (-1);
        }
    }

  fclose (filep);
  return 0;
}


static int testnkt_vkt (char *fname)
{
  FILE *filep;  
  AESWORKSPACE ws;
  AESKEYSCHED ks;
  AESXPKEY ExpandedKey, InvExpandedKey;

  char Line[200],  *LinePtr;
  UCHAR KeyBuff[32], InBuff[32], OutBuff[32], RefBuff[32];
  int direction, BlockLength, KeyLength, testno;

  filep = fopen (fname, MODE);
  if (filep == NULL)
    {
      fprintf (stderr, "%s can not be opened for reading - aborting\n", fname);
      exit (1);
    }

  /* Read Blocklength */
  do
    {
      fgets (Line, 200, filep);
    }
  while (strstr (Line, "BLOCKSIZE") == NULL);
  LinePtr = strchr (Line, '=');
  while (!isxdigit (*(++LinePtr))) {};
  sscanf (LinePtr, "%ld", &BlockLength);
#ifdef TESTAES_TEST
  printf ("BlockLength = %ld\n", BlockLength);
#endif

  /* Read Keylength */
  do
    {
      fgets (Line, 200, filep);
    }
  while (strstr (Line, "KEYSIZE") == NULL);
  LinePtr = strchr (Line, '=');
  while (!isxdigit (*(++LinePtr))) {};
  sscanf (LinePtr, "%ld", &KeyLength);
#ifdef TESTAES_TEST
  printf ("KeyLength = %ld\n", KeyLength);
#endif

  /* Read Key or PT */
  do
    {
      fgets (Line, 200, filep);
    }
  while ((strstr (Line, "PT") == NULL) && (strstr (Line, "KEY") == NULL));

  LinePtr = strchr (Line, '=');
  while (!isxdigit (*(++LinePtr))) {};

  if (strstr (Line, "PT") != NULL)
    {
      scan_bytes (LinePtr, InBuff, BlockLength/8);
    }
  else
    {
      if (strstr (Line, "KEY") != NULL)
        { 
          scan_bytes (LinePtr, KeyBuff, KeyLength/8);
        }
      else
        {
          fprintf (stderr, "Read error while searching for PT or Key in file %s\n", fname);
          exit (-1);
        }
    }

  /* Loop over test file */
  while (!feof (filep))
    {
      /* Read test number */
      do
        {
          fgets (Line, 200, filep);
        }
      while (!feof (filep) && (strstr (Line, "TEST") == NULL));

      if (feof (filep))
        {
          break;
        }

      if (strstr (Line, "TEST") != NULL)
        { 
          LinePtr = strchr (Line, '=');
          while (!isxdigit (*(++LinePtr))) {};
          sscanf (LinePtr, "%ld", &testno);
        }
      else
        {
          fprintf (stderr, "Read error while searching for test no. in file %s\n", fname);
          exit (-1);
        }

#ifdef TESTAES_TEST
      printf ("Test = %ld\n", testno);
#endif

      /* Read PT or Key */
      fgets (Line, 200, filep);
      LinePtr = strchr (Line, '=');
      while (!isxdigit (*(++LinePtr))) {};
      if (strstr (Line, "KEY") != NULL)
        {
          scan_bytes (LinePtr, KeyBuff, KeyLength/8);
        }
      else
        {
          if (strstr (Line, "PT") != NULL)
            {
              scan_bytes (LinePtr, InBuff, BlockLength/8);
            }
          else
            {
              fprintf (stderr, "Read error while searching for Key or PT in test %d of file %s\n",
                       testno, fname);
              exit (-1);
            }
        }

      /* Read CT */
      fgets (Line, 200, filep);
      if (strstr (Line, "CT") == NULL)
        {
          fprintf (stderr, "Read error while searching for CT in test %d of file %s\n",
                   testno, fname);
          exit (-1);
        }

      LinePtr = strchr (Line, '=');
      while (!isxdigit (*(++LinePtr))) {};
      scan_bytes (LinePtr, RefBuff, BlockLength/8);
    
      if (TESTBLOCK)
	{
          AESEncryptBlock (OutBuff, InBuff, KeyBuff, KeyLength, BlockLength);
	}
      else
	{
          AESInit (&ws, AES_ECB, BlockLength, NULL, &ks, KeyBuff, KeyLength, AES_ENC);
          AESCrypt (OutBuff, &ws, &ks, InBuff, BlockLength/8);
	}

      if (!memcmp (OutBuff, RefBuff, BlockLength/8))
        {
          printf ("Test %d of file %s OK\n", testno, fname);
        }
      else
        {
          printf ("Error in test %d of file %s\n", testno, fname);
          exit (-1);
        }

      if (TESTBLOCK)
	{
          AESDecryptBlock (RefBuff, OutBuff, KeyBuff, KeyLength, BlockLength);
	}
      else
	{
          AESInit (&ws, AES_ECB, BlockLength, NULL, &ks, KeyBuff, KeyLength, AES_DEC);
          AESCrypt (RefBuff, &ws, &ks, OutBuff, BlockLength/8);
	}

      if (!memcmp (RefBuff, InBuff, BlockLength/8))
        {
          printf ("Test %d of file %s OK\n", testno, fname);
        }
      else
        {
          printf ("Error in reverse test %d of file %s\n", testno, fname);
          exit (-1);
        }
    }

  fclose (filep);
  return 0;
}


static int scan_bytes (char *Linep, char *buffer, int noofbytes)
{
  int i, temp;
  int fields_read;

  for (i = 0; i < noofbytes; i++)
    {
      fields_read = sscanf (Linep, "%2X", &temp);
      Linep++;
      Linep++;
      if (fields_read != 1)
        {
          fprintf (stderr, "Read error in function scan_bytes\n");
          exit (1);
        }
      buffer[i] = temp;
    }

  return (0);
}


static void testcbc (void)
{
  AESWORKSPACE ws;
  AESKEYSCHED ks;
  UCHAR cbuff[1024], tbuff[1024];

  printf ("Testing CBC-Mode with multiple message blocks:\n");

  AESInit (&ws, AES_CBC, 128, IV1, &ks, KEY1, 128, AES_ENC);
  AESCrypt (cbuff, &ws, &ks, PT1, sizeof (PT1) - 32);
  AESCrypt (cbuff+sizeof (PT1)-32, &ws, &ks, PT1+sizeof (PT1)-32, 32);

  TestCmp ("enccbc1 (CBC)", cbuff, CT1, sizeof (PT2));

  AESInit (&ws, AES_CBC, 128, IV1, &ks, KEY1, 128, AES_DEC);
  AESCrypt (tbuff, &ws, &ks, cbuff, 32);
  AESCrypt (tbuff+32, &ws, &ks, cbuff+32, sizeof (PT1) - 32);

  TestCmp ("deccbc1 (CBC)", tbuff, PT1, sizeof (PT1));

  AESInit (&ws, AES_CBC, 128, IV1, &ks, KEY1, 128, AES_DEC);
  AESCrypt (cbuff, &ws, &ks, PT1, sizeof (PT1));

  TestCmp ("deccbc2 (CBC)", cbuff, DCT1, sizeof (PT1));

  AESInit (&ws, AES_CBC, 128, IV2, &ks, KEY2, 128, AES_ENC);
  AESCrypt (cbuff, &ws, &ks, PT2, sizeof (PT2) - 32);
  AESCrypt (cbuff+sizeof (PT2)-32, &ws, &ks, PT2+sizeof (PT2)-32, 32);

  TestCmp ("enccbc2 (CBC)", cbuff, CT2, sizeof (PT2));

  AESInit (&ws, AES_CBC, 128, IV2, &ks, KEY2, 128, AES_DEC);
  AESCrypt (tbuff, &ws, &ks, cbuff, 32);
  AESCrypt (tbuff+32, &ws, &ks, cbuff+32, sizeof (PT2) - 32);

  TestCmp ("deccbc3 (CBC)", tbuff, PT2, sizeof (PT2));

  AESInit (&ws, AES_CBC, 128, IV2, &ks, KEY2, 128, AES_DEC);
  AESCrypt (cbuff, &ws, &ks, PT2, sizeof (PT2));
  TestCmp ("deccbc4 (CBC)", cbuff, DCT2, sizeof (PT2));
}


static void
TestCmp (char * test, char * aesres, char * ref, int Length)
{
  int i;

  if (memcmp (aesres, ref, Length))
    {
      printf ("AES %s: Error!\n", test);
      printf ("Computed value:     ");
      for (i = 0 ; i < Length ; i++) 
        {
          printf ("%2.2x", aesres[i] & 0x00ff);
        }
      
      printf ("\nReference value:    ");
      for (i = 0 ; i < Length; i++) 
        {
          printf ("%2.2x", ref[i] & 0x00ff);
        }
      printf ("\n");
      /*      exit (-1); */
    }      
  else
    {
      printf ("AES %s OK\n", test);
    }
}






