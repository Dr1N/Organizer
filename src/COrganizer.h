#include "mystdafx.h"

class COrganizer
{
	//Main handles

	HWND hMainDlg;
	HWND hList, hStatusBar, hToolBar;
	
	//Edit handles

	HWND hEditDlg;
	HWND hTime, hDate, hRemember, hRunApp, hShutDown, hPath;
	HWND hBrowse, hDescription;

	//Remember handles

	HWND hRememberDlg;
	HWND hRememberEdit, hCloseRemember;

	//About handles

	HWND hAboutDlg;
	HWND hAboutEdit, hAboutOk;
	
	HANDLE hWaitTimer;
	HANDLE hThread;
	JOB	   currentJob;
	TCHAR  rememberMessage[STRING_LENGTH];

	NOTIFYICONDATA m_TrayIcon;
	
	//Menu

	HMENU hTrayContextMenu, hListContextMenu;
	
	//Jobs

	JobList jList;

	//Exchange variales

	TCHAR	path[MAX_PATH];
	ACTION	action;
	JOBTYPE type;

	//Main window messages

	BOOL COrganizer_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID COrganizer_OnClose(HWND hwnd);
	VOID COrganizer_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify);
	VOID COrganizer_OnSize(HWND hwnd, UINT state, int cx, int cy);
	VOID COrganizer_OnIcon(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL COrganizer_OnNotify(HWND hwnd, int idFrom, NMHDR* pnmhdr);
	VOID COrganizer_OnTimer(HWND hwnd, UINT id);

	//Edit window messages

	BOOL CEditDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID CEditDlg_OnClose(HWND hwnd);
	VOID CEditDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify);
	BOOL CEditDlg_OnNotify(HWND hwnd, int idFrom, NMHDR* pnmhdr);
	
	//Rememer window messages

	BOOL CRememberDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID CRememberDlg_OnClose(HWND hwnd);
	VOID CRememberDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify);

	//About window messages

	BOOL CAboutDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID CAboutDlg_OnClose(HWND hwnd);
	VOID CAboutDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify);
	
	//ListView (jobs)

	VOID FillList();
	VOID UpdateJobs();
	VOID LoadEditForm();
	VOID ReadEditForm(JOB& jb);
	bool ChangeStatus();
	
	//Files

	VOID OpenJobFile();
	VOID SaveJobFile(bool newFile = false);
	bool ReadSettings(TCHAR* file);
	bool WriteSettings(TCHAR* file);

	//Helpers

	VOID SetColums(double* aRatio, int count);
	VOID SetTrayContextMenu(int count);
	VOID SetListContextMenu(int count);
	VOID SetToolBar();
	VOID SetStatusBar();
	VOID SetJobList();
	VOID SetIcon(int idIcon);
	VOID SetTray(TCHAR* message);
	BOOL SetPrivilegies();
	VOID ShowContextMenu(int pos);
	VOID SetTitle();
		
	VOID Shutdown();
	VOID RunApplication(TCHAR* path);
	VOID ShowRemember();
	VOID ExecuteJob();
	VOID WriteLog(TCHAR* log);
	
public:
	COrganizer();
	static COrganizer* pOrganizerDlg;
	static BOOL CALLBACK DlgMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgRememberProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgAboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};