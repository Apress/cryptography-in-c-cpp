/* dllmain.c : Defines the entry point for the DLL application. */

#include <windows.h>
#include "flint.h"

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                                         )
{
  int error = 0;
  switch (ul_reason_for_call)
    {
      case DLL_PROCESS_ATTACH:
        error = FLINTInit_l();
        break;
      case DLL_THREAD_ATTACH:
        break;
      case DLL_THREAD_DETACH:
        break;
      case DLL_PROCESS_DETACH:
        FLINTExit_l();
    }

if (!error)
  return TRUE;
else
  return FALSE;
}



