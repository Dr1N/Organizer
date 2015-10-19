#include "COrganizer.h"

COrganizer* COrganizer::pOrganizerDlg = NULL;

COrganizer::COrganizer()
{
	lstrcpy(path, TEXT(""));
	pOrganizerDlg = this;
	ReadSettings(TEXT("Settings.ini"));
	jList.OpenFile(path);
	jList.SetStatus();
}

BOOL CALLBACK COrganizer::DlgMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ICON:
			pOrganizerDlg->COrganizer_OnIcon(hWnd, message, wParam, lParam);
		break;
		HANDLE_MSG(hWnd, WM_INITDIALOG, pOrganizerDlg->COrganizer_OnInitDialog);
		HANDLE_MSG(hWnd, WM_CLOSE,		pOrganizerDlg->COrganizer_OnClose);
		HANDLE_MSG(hWnd, WM_COMMAND,	pOrganizerDlg->COrganizer_OnCommand);
		HANDLE_MSG(hWnd, WM_SIZE,		pOrganizerDlg->COrganizer_OnSize);
		HANDLE_MSG(hWnd, WM_NOTIFY,		pOrganizerDlg->COrganizer_OnNotify);
		HANDLE_MSG(hWnd, WM_TIMER,		pOrganizerDlg->COrganizer_OnTimer);
	}
	return FALSE;
}

BOOL CALLBACK COrganizer::DlgEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hWnd, WM_INITDIALOG, pOrganizerDlg->CEditDlg_OnInitDialog);
		HANDLE_MSG(hWnd, WM_CLOSE,		pOrganizerDlg->CEditDlg_OnClose);
		HANDLE_MSG(hWnd, WM_COMMAND,	pOrganizerDlg->CEditDlg_OnCommand);
		HANDLE_MSG(hWnd, WM_NOTIFY,		pOrganizerDlg->CEditDlg_OnNotify);
	}
	return FALSE;
}

BOOL CALLBACK COrganizer::DlgRememberProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hWnd, WM_INITDIALOG, pOrganizerDlg->CRememberDlg_OnInitDialog);
		HANDLE_MSG(hWnd, WM_CLOSE,		pOrganizerDlg->CRememberDlg_OnClose);
		HANDLE_MSG(hWnd, WM_COMMAND,	pOrganizerDlg->CRememberDlg_OnCommand);
	}
	return FALSE;
}

BOOL CALLBACK COrganizer::DlgAboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hWnd, WM_INITDIALOG, pOrganizerDlg->CAboutDlg_OnInitDialog);
		HANDLE_MSG(hWnd, WM_CLOSE,		pOrganizerDlg->CAboutDlg_OnClose);
		HANDLE_MSG(hWnd, WM_COMMAND,	pOrganizerDlg->CAboutDlg_OnCommand);
	}
	return FALSE;
}

BOOL COrganizer::COrganizer_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hMainDlg = hwnd;
	hList = GetDlgItem(hwnd, IDC_LIST1);
	
	RECT rect = {0};
	int width = 0;
	int heigth = 0;
	
	SetStatusBar();
	SetToolBar();
	SetJobList();
	double wRatio[6] = { 0.1, 0.15, 0.1, 0.1, 0.1, 0.45 }; //column width
	SetColums(wRatio, COLUMNS);
	SetTray(TEXT("Органайзер"));
	SetTrayContextMenu(TRAY_MENU_ITEMS);
	SetListContextMenu(LIST_MENU_ITEMS);
	SetIcon(IDI_ICON1);
	SetTitle();
	FillList();
	SetTimer(hwnd, TIMER_ID, 1000, NULL);
	
	return TRUE;
}

VOID COrganizer::COrganizer_OnClose(HWND hwnd)
{
	jList.SaveFile(path);
	DestroyMenu(hTrayContextMenu);
	DestroyMenu(hListContextMenu);
	WriteSettings(TEXT("Settings.ini"));
	KillTimer(hMainDlg, TIMER_ID);
	EndDialog(hwnd, 0);
}

VOID COrganizer::COrganizer_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify)
{
	switch(id)
	{
	case ID_FILE_OPEN_FILE:
		OpenJobFile();
		break;
	case ID_FILE_SAVE_FILE:
		SaveJobFile();
		break;
	case ID_FILE_SAVE_FILE_AS:
		SaveJobFile(1);
		break;
	case ID_EDIT_ADD:
		action = ADD;
		DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_EDITDIALOG), hMainDlg, COrganizer::DlgEditProc);
		break;
	case ID_EDIT_EDIT:
	{
		action = EDIT;
		int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
		if(iPos != -1)
		{
			DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_EDITDIALOG), hMainDlg, COrganizer::DlgEditProc);
		}
		break;
	}
	case ID_EDIT_DELETE:
	{
		action = DEL;
		int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
		if(iPos != -1)
		{
			DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_EDITDIALOG), hMainDlg, COrganizer::DlgEditProc);
		}
		break;
	}
	case ID_FILE_EXIT:
		jList.SaveFile(path);
		DestroyMenu(hTrayContextMenu);
		DestroyMenu(hListContextMenu);
		WriteSettings(TEXT("Settings.ini"));
		KillTimer(hMainDlg, TIMER_ID);
		EndDialog(hwnd, 0);
		break;
	case ID_HELP_ABOUT:
		DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_ABOUTDIALOG), hMainDlg, COrganizer::DlgAboutProc);
		break;
	case IDC_LIST_CONTEXT_CANCEL:
	case IDC_LIST_CONTEXT_RETURN:
		if( ChangeStatus() )
		{
			jList.SaveFile(path);
			FillList();
		}
		break;
	case IDC_TRAY_CONTEXT_SHOW:
		ShowWindow(hwnd, SW_NORMAL);
		SetForegroundWindow(hwnd);
		Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
		break;
	case IDC_TRAY_CONTEXT_EXIT:
		Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
		DestroyMenu(hTrayContextMenu);
		DestroyMenu(hListContextMenu);
		EndDialog(hwnd, 0);
		break;
	}
}

VOID COrganizer::COrganizer_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if(state == SIZE_MINIMIZED)
	{
		ShowWindow(hwnd, SW_HIDE);
		Shell_NotifyIcon(NIM_ADD, &m_TrayIcon);
	}
}

VOID COrganizer::COrganizer_OnIcon(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( wParam != m_TrayIcon.uID || lParam != WM_LBUTTONDOWN && lParam != WM_RBUTTONUP )
	{
		return;
	}
	POINT point;
	if(lParam == WM_RBUTTONUP)
	{
		GetCursorPos(&point);
		TrackPopupMenu(hTrayContextMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, NULL, hwnd, NULL);
	}
	else if(lParam == WM_LBUTTONDOWN)
	{
		ShowWindow(hwnd, SW_NORMAL);
		SetForegroundWindow(hwnd);
		Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
	}
}

BOOL COrganizer::COrganizer_OnNotify(HWND hwnd, int idFrom, NMHDR* pnmhdr)
{
	if(idFrom == IDC_LIST1)
	{
		int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
		if(iPos == -1)
		{
			for(int i = 0; i < BARS; i++)
			{
				SendMessage(hStatusBar, SB_SETTEXT, i, (LPARAM)TEXT(""));
			}
			SendMessage(hToolBar, TB_ENABLEBUTTON, ID_EDIT_EDIT, FALSE);
			SendMessage(hToolBar, TB_ENABLEBUTTON, ID_EDIT_DELETE, FALSE);
			return TRUE;
		}
		POINT point = {0};
		TCHAR szBuff[STRING_LENGTH] = {0};
		switch(pnmhdr->code)
		{
		case NM_DBLCLK:
			action = EDIT;
			DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_EDITDIALOG), hMainDlg, COrganizer::DlgEditProc);
			break;
		case NM_RCLICK:
			GetCursorPos(&point);
			ShowContextMenu(iPos);
			TrackPopupMenu(hListContextMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, NULL, hwnd, NULL);
			break;
		case LVN_ITEMCHANGED:
		{
			JOB jb; 
			jb = jList.GetJb(iPos);
			SYSTEMTIME tm = jb.time;

			switch(jb.type)
			{
			case REMEMBER:
				lstrcpy(szBuff, TEXT("Напоминание"));
				break;
			case RUNAPPLICATION:
				lstrcpy(szBuff, TEXT("Запуск"));
				break;
			case SHUTDOWN:
				lstrcpy(szBuff, TEXT("Выключение"));
				break;
			}
			SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)szBuff);
				
			wsprintf(szBuff, TEXT("Время: %02d:%02d Дата: %02d.%02d.%4d"), tm.wHour, tm.wMinute, tm.wDay, tm.wMonth, tm.wYear);
			SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)szBuff);
						
			wsprintf(szBuff, TEXT("Осталось: %d ч."), 0);
			SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)szBuff);
			
			SendMessage(hToolBar, TB_ENABLEBUTTON, ID_EDIT_EDIT, TRUE);
			SendMessage(hToolBar, TB_ENABLEBUTTON, ID_EDIT_DELETE, TRUE);
			break;
		}
		}
		return TRUE;
	}
	return FALSE;
}

VOID COrganizer::COrganizer_OnTimer(HWND hwnd, UINT id)
{
	if(jList.IsEmpty())
	{
		return;
	}

	//Execute jobs

	for(int i = 0; i < jList.GetCount(); i++)
	{
		JOB& jb = jList.GetJb(i);
		if(jb.status == ACTIVE)
		{
			SYSTEMTIME sTime = {0};
			GetLocalTime(&sTime);
			if( sTime.wYear == jb.time.wYear &&
				sTime.wMonth == jb.time.wMonth &&
				sTime.wDay == jb.time.wDay &&
				sTime.wHour == jb.time.wHour &&
				sTime.wMinute == jb.time.wMinute)
			{
				jb.status = DONE;
				switch(jb.type)
				{
				case REMEMBER:
					lstrcpy(rememberMessage, jb.description);
					ShowRemember();
					break;
				case RUNAPPLICATION:
					RunApplication(jb.path);
					break;
				case SHUTDOWN:
					jList.SaveFile(path);
					Shutdown();
					break;
				}
				FillList();
			}
		}
	}
}

BOOL COrganizer::CEditDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hEditDlg = hwnd;
	hTime = GetDlgItem(hwnd, IDC_TIME);
	hDate = GetDlgItem(hwnd, IDC_DATE);
	hRemember = GetDlgItem(hwnd, IDC_REMEMBER);
	hRunApp = GetDlgItem(hwnd, IDC_RUN);
	hShutDown = GetDlgItem(hwnd, IDC_SHUTDOWN);
	hPath = GetDlgItem(hwnd, IDC_PATH);
	hBrowse = GetDlgItem(hwnd, IDC_OPENDLG);
	hDescription = GetDlgItem(hwnd, IDC_DESCRIPTION);
	
	DateTime_SetFormat(hTime, TEXT("HH':'mm"));

	SYSTEMTIME range[2] = {0};
	switch (action)	
	{
	case ADD:
		SetWindowText(hEditDlg, TEXT("Добавление"));
		GetLocalTime(&range[0]);
		range[1].wYear = 2100;
		range[1].wMonth = 12;
		range[1].wDay = 31;
		DateTime_SetRange(hDate, GDTR_MIN | GDTR_MAX, range);
		SendDlgItemMessage(hwnd, IDC_REMEMBER, BM_SETCHECK, WPARAM(BST_CHECKED), 0);
		EnableWindow(hBrowse, FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC1), FALSE);
		type = REMEMBER;
		break;
	case EDIT:
		SetWindowText(hEditDlg, TEXT("Редактирование"));
		LoadEditForm();
		break;
	case DEL:
		SetWindowText(hEditDlg, TEXT("Удаление"));
		LoadEditForm();
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC1), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC2), TRUE);
		EnableWindow(hTime, FALSE);
		EnableWindow(hDate, FALSE);
		EnableWindow(hRemember, FALSE);
		EnableWindow(hRunApp, FALSE);
		EnableWindow(hShutDown, FALSE);
		EnableWindow(hPath, FALSE);
		EnableWindow(hBrowse, FALSE);
		EnableWindow(hDescription, FALSE);
		break;
	}
	return TRUE;
}

VOID COrganizer::CEditDlg_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

VOID COrganizer::CEditDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify)
{
	switch(id)
	{
	case IDC_REMEMBER:
		type = REMEMBER;
		EnableWindow(hBrowse, FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC1), FALSE);
		EnableWindow(hDescription, TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC2), TRUE);
		break;
	case IDC_RUN:
		type = RUNAPPLICATION;
		EnableWindow(hBrowse, TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC1), TRUE);
		EnableWindow(hDescription, FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC2), FALSE);
		break;
	case IDC_SHUTDOWN:
		type = SHUTDOWN;
		EnableWindow(hBrowse, FALSE);
		EnableWindow(hDescription, FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC1), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_STATIC2), FALSE);
		break;
	case IDC_OPENDLG:
	{
		TCHAR szPath[MAX_PATH] = TEXT("");
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hEditDlg;
		ofn.lpstrTitle = TEXT("Выберете файл");
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFile = szPath;
		ofn.lpstrFile[0] = 0;
		ofn.lpstrFilter = TEXT("Executable (*.exe)\0*.exe\0");
		
		GetOpenFileName(&ofn);
		if(lstrlen(ofn.lpstrFile))
			SetWindowText(hPath, ofn.lpstrFile);
		break;
	}
	case IDOK:
		UpdateJobs();
		EndDialog(hwnd, 0);
		break;
	case IDCANCEL:
		EndDialog(hwnd, 0);
		break;
	}
}

BOOL COrganizer::CEditDlg_OnNotify(HWND hwnd, int idFrom, NMHDR* pnmhdr)
{
	return FALSE;
}

VOID COrganizer::UpdateJobs()
{
	JOB jb = {0};
	int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
		
	switch (action) 
	{
	case ADD:
		ReadEditForm(jb);
		jList.AddJb(jb);
		break;
	case EDIT:
		ReadEditForm(jb);
		jList.RemoveJb(iPos);
		jList.AddJb(jb);
		break;
	case DEL:
		jList.RemoveJb(iPos);
		break;
	}
	jList.SaveFile(path);
	FillList();
}

BOOL COrganizer::CRememberDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hRememberDlg = hwnd;
	hRememberEdit = GetDlgItem(hwnd, IDC_REMEMBER);
	hCloseRemember = GetDlgItem(hwnd, IDC_CLOSEREMEMBER);
	SetWindowText(hRememberEdit, rememberMessage);
	BringWindowToTop(hwnd);
	return TRUE;
}

VOID COrganizer::CRememberDlg_OnClose(HWND hwnd)
{
	EndDialog(hRememberDlg, 0);
}

VOID COrganizer::CRememberDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify)
{
	switch(id)
	{
	case IDC_CLOSEREMEMBER:
		EndDialog(hRememberDlg, 0);
		break;
	}
}

BOOL COrganizer::CAboutDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hAboutDlg = hwnd;
	hAboutEdit = GetDlgItem(hwnd, IDC_ABOUT);
	hAboutOk = GetDlgItem(hwnd, IDOK);
	TCHAR szBuff[STRING_LENGTH] = {0};
	LoadString((HINSTANCE)GetModuleHandle(0), IDS_ABOUT, szBuff, STRING_LENGTH);
	SetWindowText(hAboutEdit, szBuff);

	return TRUE;
}

VOID COrganizer::CAboutDlg_OnClose(HWND hwnd)
{
	EndDialog(hAboutDlg, 0);
}

VOID COrganizer::CAboutDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtr, UINT codeNotify)
{
	switch(id)
	{
	case IDOK:
		EndDialog(hAboutDlg, 0);
		break;
	}
}

VOID COrganizer::LoadEditForm()
{
	int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
	if(iPos == -1)
	{
		return;
	}
	
	JOB jb = {0};
	jb = jList.GetJb(iPos);
	type = jb.type;
	
	switch(jb.type)
	{
	case REMEMBER:
 		SendDlgItemMessage(hEditDlg, IDC_REMEMBER, BM_SETCHECK, WPARAM(BST_CHECKED), 0);
		SetWindowText(hDescription, jb.description);
		EnableWindow(hBrowse, FALSE);
		EnableWindow(GetDlgItem(hEditDlg, IDC_STATIC1), FALSE);
		EnableWindow(hDescription, TRUE);
		EnableWindow(GetDlgItem(hEditDlg, IDC_STATIC2), TRUE);
		break;
	case RUNAPPLICATION:
		SendDlgItemMessage(hEditDlg, IDC_RUN, BM_SETCHECK, WPARAM(BST_CHECKED), 0);
		SetWindowText(hPath, jb.path);
		EnableWindow(hBrowse, TRUE);
		EnableWindow(GetDlgItem(hEditDlg, IDC_STATIC1), TRUE);
		EnableWindow(hDescription, FALSE);
		EnableWindow(GetDlgItem(hEditDlg, IDC_STATIC2), FALSE);
		break;
	case SHUTDOWN:
		SendDlgItemMessage(hEditDlg, IDC_SHUTDOWN, BM_SETCHECK, WPARAM(BST_CHECKED), 0);
		EnableWindow(hBrowse, FALSE);
		EnableWindow(hDescription, FALSE);
		EnableWindow(GetDlgItem(hEditDlg, IDC_STATIC1), FALSE);
		EnableWindow(GetDlgItem(hEditDlg, IDC_STATIC2), FALSE);
		break;
	}
	DateTime_SetSystemtime(hDate, GDT_VALID, &jb.time);
	DateTime_SetSystemtime(hTime, GDT_VALID, &jb.time);
}

VOID COrganizer::ReadEditForm(JOB& jb)
{
	SYSTEMTIME sTime = {0};
	SYSTEMTIME sDate = {0};
	TCHAR szBuff[STRING_LENGTH] = {0};
	
	jb.type = type;
	jb.status = ACTIVE;
	DateTime_GetSystemtime(hTime, &sTime);
	DateTime_GetSystemtime(hDate, &sDate);
	jb.time.wDay = sDate.wDay;
	jb.time.wMonth = sDate.wMonth;
	jb.time.wYear = sDate.wYear;
	jb.time.wHour = sTime.wHour;
	jb.time.wMinute = sTime.wMinute;
	if(type == REMEMBER)
	{
		GetWindowText(hDescription, szBuff, STRING_LENGTH);
		lstrcpy(jb.description, szBuff);
		lstrcpy(jb.path, TEXT(""));
	}
	else if(type == RUNAPPLICATION)
	{
		GetWindowText(hPath, szBuff, STRING_LENGTH);
		lstrcpy(jb.path, szBuff);
		lstrcpy(jb.description, TEXT(""));
	}
	else if(type == SHUTDOWN)
	{
		lstrcpy(jb.description, TEXT("Отключение питания компьютера"));
	}
}

bool COrganizer::ChangeStatus()
{
	int iPos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
	
	if(iPos == -1)
		return false;

	JOB& jb = jList.GetJb(iPos);
	switch(jb.status)
	{
	case ACTIVE:
		jb.status = CANCELED;
		break;
	case CANCELED:
		jb.status = ACTIVE;
		break;
	default:
		return false;
	}
	return true;
}

VOID COrganizer::OpenJobFile()
{
	TCHAR szPath[MAX_PATH] = TEXT("");
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hMainDlg;
	ofn.lpstrTitle = TEXT("Выберете файл");
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = szPath;
	ofn.lpstrFile[0] = 0;
	ofn.lpstrFilter = TEXT("Jobs (*.dat)\0*.dat\0");
	GetOpenFileName(&ofn);
	
	if(!lstrcmp(ofn.lpstrFile, TEXT("")))
		return;

	lstrcpy(path, ofn.lpstrFile);
	jList.OpenFile(path);
	SetTitle();
	FillList();
	WriteSettings(TEXT("Settings.ini"));
}

VOID COrganizer::SaveJobFile(bool newFile)
{
	if(newFile || !lstrcmp(path, TEXT("")))
	{
		TCHAR szPath[MAX_PATH] = TEXT("");
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hMainDlg;
		ofn.lpstrTitle = TEXT("Сохранение");
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFile = szPath;
		ofn.lpstrFile[0] = 0;
		ofn.lpstrFilter = TEXT("Jobs (*.dat)\0*.dat\0");
		GetSaveFileName(&ofn);
		if(lstrlen(ofn.lpstrFile))
		{
			lstrcpy(path, ofn.lpstrFile);
			jList.SaveFile(path);
			SetTitle();
		}
	}
	else
	{
		jList.SaveFile(path);
	}
}

VOID COrganizer::FillList()
{
	ListView_DeleteAllItems(hList);
	if(!jList.IsEmpty())
	{
		LVITEM lvi;
		TCHAR szBuff[STRING_LENGTH] = {0};
		JOB jb = {0};
		SYSTEMTIME t = {0};
		
		for(int i = 0; i < jList.GetCount(); i++)
		{
			jb = jList.GetJb(i);
			t = jb.time;
			
			wsprintf(szBuff, TEXT("%d"), i + 1);
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = szBuff;
			lvi.mask = LVIF_TEXT;
			SendMessage(hList, LVM_INSERTITEM, i, (LPARAM)&lvi);
			
			switch(jb.type)
			{
			case REMEMBER:
				lstrcpy(szBuff, TEXT("Напоминание"));
				break;
			case RUNAPPLICATION:
				lstrcpy(szBuff, TEXT("Запуск"));
				break;
			case SHUTDOWN:
				lstrcpy(szBuff, TEXT("Выключение"));
				break;
			}
			lvi.iItem = i;
			lvi.iSubItem = 1;
			lvi.pszText = szBuff;
			lvi.mask = LVIF_TEXT;
			SendMessage(hList, LVM_SETITEM, i, (LPARAM)&lvi);
			
			wsprintf(szBuff, TEXT("%02d.%02d.%4d"), t.wDay, t.wMonth, t.wYear);
			lvi.iItem = i;
			lvi.iSubItem = 2;
			lvi.pszText = szBuff;
			lvi.mask = LVIF_TEXT;
			SendMessage(hList, LVM_SETITEM, i, (LPARAM)&lvi);
			
			wsprintf(szBuff, TEXT("%02d:%02d"), t.wHour, t.wMinute);
			lvi.iItem = i;
			lvi.iSubItem = 3;
			lvi.pszText = szBuff;
			lvi.mask = LVIF_TEXT;
			SendMessage(hList, LVM_SETITEM, i, (LPARAM)&lvi);
			
			switch(jb.status)
			{
			case ACTIVE:
				lstrcpy(szBuff, TEXT("Активно"));
				break;
			case DEACTIVE:
				lstrcpy(szBuff, TEXT("Не активно"));
				break;
			case DONE:
				lstrcpy(szBuff, TEXT("Выполнено"));
				break;
			case CANCELED:
				lstrcpy(szBuff, TEXT("Отменено"));
				break;
			}
			lvi.iItem = i;
			lvi.iSubItem = 4;
			lvi.pszText = szBuff;
			lvi.mask = LVIF_TEXT;
			SendMessage(hList, LVM_SETITEM, i, (LPARAM)&lvi);
			
			lvi.iItem = i;
			lvi.iSubItem = 5;
			jb.type == RUNAPPLICATION ? lvi.pszText = jb.path : lvi.pszText = jb.description;
			lvi.mask = LVIF_TEXT;
			SendMessage(hList, LVM_SETITEM, i, (LPARAM)&lvi);
		}
	}
}

VOID COrganizer::SetColums(double* aRatio, int count)
{
	RECT rect = {0};
	GetClientRect(hList, &rect);
	int lvWidth = rect.right - rect.left;
	int sum = 0;

	TCHAR szBuff[STRING_LENGTH] = {0};
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	for(int i = 0; i < count; i++)
	{
		LoadString((HINSTANCE)GetModuleHandle(0), IDS_COL_1 + i, szBuff, STRING_LENGTH);
		(i != count - 1) ? lvc.cx = lvWidth * aRatio[i] : lvc.cx = lvWidth - sum;
		lvc.pszText = szBuff;
		lvc.iSubItem = i;
		SendMessage(hList, LVM_INSERTCOLUMN, i, (LPARAM)&lvc);
		sum += lvc.cx;
	}
}

VOID COrganizer::SetTrayContextMenu(int count)
{
	hTrayContextMenu = CreatePopupMenu();
	TCHAR szBuff[STRING_LENGTH];
	for(int i = 0; i < count; i++)
	{
		LoadString((HINSTANCE)GetModuleHandle(0), IDS_TRAY_CONTEXT_1 + i, szBuff, STRING_LENGTH);
		AppendMenu(hTrayContextMenu, MF_STRING, (UINT_PTR)IDC_TRAY_CONTEXT_SHOW + i, szBuff);
	}
}

VOID COrganizer::SetListContextMenu(int count)
{
	hListContextMenu = CreatePopupMenu();
	TCHAR szBuff[STRING_LENGTH];
	for(int i = 0; i < count; i++)
	{
		LoadString((HINSTANCE)GetModuleHandle(0), IDS_LIST_CONTEXT_1 + i, szBuff, STRING_LENGTH);
		if(i == 0) 
		{
			AppendMenu(hListContextMenu, MF_STRING, (UINT_PTR)ID_EDIT_EDIT, szBuff);
		}
		else if(i == 2)
		{
			AppendMenu(hListContextMenu, MF_STRING, (UINT_PTR)ID_EDIT_DELETE, szBuff);
		}
		else
		{
			AppendMenu(hListContextMenu, MF_STRING, (UINT_PTR)IDC_LIST_CONTEXT_CANCEL + i, szBuff);
		}
	}
}

VOID COrganizer::SetToolBar()
{
	const int ImageListID    = 0;
    const int numButtons     = 3;
    const int bitmapSize     = 16;
    
    const DWORD buttonStyles = BTNS_AUTOSIZE;

    // Create the toolbar

    hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, 
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_FLAT, 0, 0, 0, 0, 
		hMainDlg, NULL, GetModuleHandle(0), NULL);
        
    if (hToolBar == NULL)
        return;

    // Create the image list

    HIMAGELIST g_hImageList = ImageList_Create(bitmapSize, bitmapSize,
		ILC_COLOR16 | ILC_MASK,
		numButtons, 0);
	
	HICON hIconPlus = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON4));
	HICON hIconMinus = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON2));
	HICON hIconEdit = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON3));
	
	ImageList_AddIcon(g_hImageList, hIconPlus);
	ImageList_AddIcon(g_hImageList, hIconMinus);
	ImageList_AddIcon(g_hImageList, hIconEdit);

    // Set the image list

    SendMessage(hToolBar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)g_hImageList);
	  
    // Initialize button info

    TBBUTTON tbButtons[numButtons] = 
    {
        { 0, ID_EDIT_ADD,	 TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)TEXT("Добавить") },
        { 2, ID_EDIT_EDIT,	 TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)TEXT("Редактировать"}),
        { 1, ID_EDIT_DELETE, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)TEXT("Удалить"})
    };

    // Add buttons

    SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hToolBar, TB_ADDBUTTONS,       (WPARAM)numButtons,       (LPARAM)&tbButtons);

    // Resize the toolbar, and then show it

    SendMessage(hToolBar, TB_AUTOSIZE, 0, 0); 
	SendMessage(hToolBar, TB_ENABLEBUTTON, ID_EDIT_EDIT, FALSE);
	SendMessage(hToolBar, TB_ENABLEBUTTON, ID_EDIT_DELETE, FALSE);
    
	ShowWindow(hToolBar,  TRUE);
}

VOID COrganizer::SetStatusBar()
{
	RECT rect = {0};
	hStatusBar = CreateWindowEx(NULL, STATUSCLASSNAME, TEXT(""), 
			WS_VISIBLE | WS_CHILD | CCS_BOTTOM | SBARS_SIZEGRIP | SBT_TOOLTIPS, 
			0, 0, 0, 0, hMainDlg, (HMENU)IDC_STATUSBAR, 0, 0); 
	GetClientRect(hMainDlg, &rect);
	int width = rect.right - rect.left;
	int heigth = rect.bottom - rect.top;
	int wStatusPanel = width / BARS;
	int aPanels[4] = { wStatusPanel, 2 * wStatusPanel, 3 * wStatusPanel, -1 };
	SendMessage(hStatusBar, SB_SETPARTS, BARS, LPARAM(aPanels));
}

VOID COrganizer::SetJobList()
{
	RECT rect = {0};
	GetClientRect(hMainDlg, &rect);
	int width = rect.right - rect.left;
	int heigth = rect.bottom - rect.top;
	GetClientRect(hStatusBar, &rect);
	heigth -= rect.bottom;
	GetClientRect(hToolBar, &rect);
	MoveWindow(hList, 0, rect.bottom, width, heigth, TRUE);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

VOID COrganizer::SetIcon(int idIcon)
{
	HICON hIconSm, hIcon;
	hIconSm = LoadIcon((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(idIcon));
	hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(idIcon));
	SetClassLong(hMainDlg, GCL_HICON, (LONG)hIcon);
	SetClassLong(hMainDlg, GCL_HICONSM, (LONG)hIconSm);
}

VOID COrganizer::SetTray(TCHAR* message)
{
	m_TrayIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_TrayIcon.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1));
	m_TrayIcon.hWnd = hMainDlg;
	lstrcpy(m_TrayIcon.szTip, message);
	m_TrayIcon.uCallbackMessage = WM_ICON;
	m_TrayIcon.uFlags = NIF_TIP | NIF_MESSAGE | NIF_ICON;
	m_TrayIcon.uID = IDI_ICON1;
}

BOOL COrganizer::SetPrivilegies()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL, 0);

	if(GetLastError() != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}

bool COrganizer::ReadSettings(TCHAR* f)
{
	TCHAR szBuff[STRING_LENGTH] = {0};
	char line[STRING_LENGTH] = {0};
	std::string strLine;
	std::fstream file;
	
	file.open(f, std::ios::in);
	if(file.is_open())
	{
		int nBegin;
		std::string settingPath = "path:";
		while(!file.eof())
		{
			file.getline(line, STRING_LENGTH);
			strLine = line;
			std::string strPath = "";
			nBegin = strLine.find(settingPath);
			if(nBegin !=  std::string::npos)
			{
				strPath = strLine.substr(settingPath.size(), strLine.size());
				#ifdef _UNICODE
				int length = MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), -1, NULL, 0);
				wchar_t *ptrPath = new wchar_t[length];
				MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), -1, ptrPath, length);
				lstrcpy(path, ptrPath);
				delete[] ptrPath;
				#else
				lstrcpy(path, strPath.c_str());
				#endif
			}
		}
	}
	else
	{
		wsprintf(szBuff, TEXT("Не удалось открыть файл %s"), f);
		MessageBox(NULL, f, TEXT(""), MB_OK | MB_ICONERROR);
		return false;
	}
	file.close();
	return true;
}

bool COrganizer::WriteSettings(TCHAR* f)
{
	TCHAR szBuff[STRING_LENGTH] = {0};

	char settingLine[STRING_LENGTH] = {0};
	std::fstream file;
	file.open(f, std::ios::out);
	if(file.is_open())
	{
		#ifdef _UNICODE
		int length = WideCharToMultiByte(CP_ACP, 0, path, -1, NULL, 0, 0, 0);
		char *ansipath = new char[length];
		WideCharToMultiByte(CP_ACP, 0, path, -1, ansipath, length, 0, 0);
		strcpy_s(settingLine, "path:");
		strcat_s(settingLine, ansipath);
		#else
		settingLine = "path:" + path;
		#endif
		file<<settingLine;
		delete[] ansipath;
	}
	else
	{
		wsprintf(szBuff, TEXT("Не удалось открыть файл %s"), f);
		MessageBox(NULL, f, TEXT(""), MB_OK | MB_ICONERROR);
		return false;
	}
	file.close();
	return true;
}

VOID COrganizer::ShowContextMenu(int pos)
{
	JOB jb = jList.GetJb(pos);
	switch(jb.status)
	{
	case ACTIVE:
		ModifyMenu(hListContextMenu, 1, MF_BYPOSITION | MF_STRING, IDC_LIST_CONTEXT_CANCEL, TEXT("Отменить"));
		break;
	case CANCELED:
		ModifyMenu(hListContextMenu, 1, MF_BYPOSITION | MF_STRING, IDC_LIST_CONTEXT_RETURN, TEXT("Включить"));
		break;
	default:
		ModifyMenu(hListContextMenu, 1, MF_BYPOSITION | MF_STRING | MF_GRAYED, NULL, TEXT("Отменить"));
		break;
	}
}

VOID COrganizer::SetTitle()
{
	TCHAR szBuff[STRING_LENGTH] = {0};
	wsprintf(szBuff, TEXT("Органайзер"));
	SetWindowText(hMainDlg, szBuff);
}

VOID COrganizer::Shutdown()
{
	if(SetPrivilegies())
	{
		ExitWindows(EWX_SHUTDOWN | EWX_FORCE, 0);
	}
	else
	{
		MessageBox(NULL, TEXT("Не удалось выключить компьютер"), TEXT("Ошикба"), MB_OK | MB_ICONERROR);
	}
}

VOID COrganizer::RunApplication(TCHAR* path)
{
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&si, sizeof(pi));
	
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	BOOL fbCreate = CreateProcess(
		NULL,
		path,
		&sa,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi
	);
	if(!fbCreate)
	{
		TCHAR szBuff[STRING_LENGTH] = {0};
		wsprintf(szBuff, TEXT("Невозможно запустить: %s"), path);
		MessageBox(NULL, szBuff, TEXT("Ошибка"), MB_OK | MB_ICONERROR);
	}
	else
	{
		CloseHandle(pi.hProcess);
	}
}

VOID COrganizer::ShowRemember()
{
	DialogBox((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_REMEMBERDIALOG), NULL, COrganizer::DlgRememberProc);
}

VOID COrganizer::ExecuteJob()
{
	//to-do
}

VOID COrganizer::WriteLog(TCHAR* log)
{
	//to-do
}