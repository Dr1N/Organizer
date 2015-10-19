#include "mystdafx.h"
#include "COrganizer.h"

int WINAPI WinMain( HINSTANCE hInstance,
				    HINSTANCE hPrevInstance,
				    LPSTR	  szCmdLine,
				    int		  iCmdShow )
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, TEXT("ORGANAIZERMUTEX"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return -1;
	}

	COrganizer org;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, COrganizer::DlgMainProc);
	CloseHandle(hMutex);
	return 0;
}