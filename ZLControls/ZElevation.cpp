#include "StdAfx.h"
#include "ZElevation.h"

CZElevation::CZElevation()
{
}
CZElevation::~CZElevation()
{
}

BOOL CZElevation::GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin)
{
	   HANDLE hToken = NULL;
   DWORD dwSize; 

   // Get current process token
   if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
      return(FALSE);

   BOOL bResult = FALSE;

   // Retrieve elevation type information 
   if (GetTokenInformation(hToken, TokenElevationType, 
      pElevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize)) {
      // Create the SID corresponding to the Administrators group
      byte adminSID[SECURITY_MAX_SID_SIZE];
      dwSize = sizeof(adminSID);
      CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, 
         &dwSize);

      if (*pElevationType == TokenElevationTypeLimited) {
         // Get handle to linked token (will have one if we are lua)
         HANDLE hUnfilteredToken = NULL;
         GetTokenInformation(hToken, TokenLinkedToken, (VOID*) 
            &hUnfilteredToken, sizeof(HANDLE), &dwSize);

         // Check if this original token contains admin SID
         if (CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin)) {
            bResult = TRUE;
         }

         // Don't forget to close the unfiltered token
         CloseHandle(hUnfilteredToken);
      } else {
         *pIsAdmin = IsUserAnAdmin();
         bResult = TRUE;
      }
   }

   // Don't forget to close the process token
   CloseHandle(hToken);

   return(bResult);
}

DWORD CZElevation::StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine)
{
	   // Initialize the structure.
   SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

   // Ask for privileges elevation.
   sei.lpVerb = TEXT("runas");

   // Pass the application to start with high privileges.
   sei.lpFile = szExecutable;

   // Pass the command line.
   sei.lpParameters = szCmdLine;

   // Don't forget this parameter otherwise the window will be hidden.
   //sei.nShow = SW_SHOWNORMAL;

   ShellExecuteEx(&sei);
   return(GetLastError());
}