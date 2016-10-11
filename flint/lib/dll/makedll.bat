REM Generation of a DLL under Microsoft Visual C/C++, providing the C functions
REM of the FLINT/C package. Deployment of the DLL is accomplished by linking
REM the import library flint.lib to an application. The file flint.def contains
REM a list of the exported symbols.

REM On loading the DLL the run time system automatically calls the function
REM DllMain(). This function is responsible for initializing the randomnumber
REM generators and the bank of CLINT registers. Any application is free to
REM initialize the random number generators on their own with any suitable value
REM (in any case this is recommended for security-critical applications).
REM A reinitialization of the bank of registers by an application is without 
REM effect, though the application has to free the allocated registers in its
REM own responsibility.

cl -O2 -DFLINT_ASM -I..\..\src -LD -MD -DFLINT_INITRAND ..\..\flint.c ..\..\aes.c ..\..\ripemd.c ..\..\sha1.c ..\..\sha256.c ..\..\random.c ..\..\entropy.c dllmain.c ..\flintavc.lib advapi32.lib flint.def

