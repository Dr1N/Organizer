#include "CJobList.h"

JobList::JobList(){}

bool JobList::IsEmpty() const 
{ 
	return jobs.empty(); 
}

int JobList::GetCount() const 
{ 
	return jobs.size(); 
}

void JobList::Clear()
{
	if(!jobs.empty())
		jobs.clear();
}

void JobList::AddJb(const JOB& job)	 
{ 
	ULONGLONG time = SysTimeToInt(job.time);
	if(time)
	{
		jobs.insert(std::make_pair(time, job));
	}
}

void JobList::RemoveJb(SYSTEMTIME sTime)
{
	if(jobs.empty())
	{
		return;
	}
	
	ULONGLONG time = SysTimeToInt(sTime);
	if(time)
	{
		JOBITER iter = jobs.find(time);
		if(iter != jobs.end())
		{
			jobs.erase(iter);
		}
	}
}

void JobList::RemoveJb(int index)
{
	if( index >= 0 && index < (int)jobs.size() && !jobs.empty() )
	{
		JOBITER iter = jobs.begin();
		int i = 0;
		while(i < index)
		{
			iter++;
			i++;
		}
		jobs.erase(iter);
	}
}

JOB& JobList::GetJb(SYSTEMTIME sTime)
{
	ULONGLONG time = SysTimeToInt(sTime);
	if(time)
	{
		JOBITER iter = jobs.find(time);
		if(iter != jobs.end())
		{
			return (*iter).second;
		}
	}
}

JOB& JobList::GetJb(int index)
{
	if( index >= 0 && index < (int)jobs.size() && !jobs.empty() )
	{
		JOBITER iter = jobs.begin();
		int i = 0;
		while(i < index)
		{
			iter++;
			i++;
		}
		return (*iter).second;
	}
}

int JobList::OpenFile(TCHAR* fpath)
{
	TCHAR szBuff[STRING_LENGTH] = {0};
	
	HANDLE hFile = CreateFile(
		fpath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wsprintf(szBuff,TEXT("Не удалось открыть файл %s. Файл будет создан"), fpath);
		MessageBox(NULL, szBuff, TEXT("Ошибка"), MB_OK | MB_ICONINFORMATION);
		hFile = CreateFile(
			fpath,
			GENERIC_WRITE,
			NULL,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			wsprintf(szBuff,TEXT("Не удалось создать файл %s."), fpath);
			MessageBox(NULL, szBuff, TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			return GetLastError();
		}
		CloseHandle(hFile);
		return 0;
	}
	
	jobs.clear();
	int bResult = 0;
	DWORD dwReadByte = 0;
	JOB jb = {0};
	ULONGLONG iTime = 0;
	
	while(true)
	{
		bResult = ReadFile(hFile, &jb, sizeof(JOB), &dwReadByte, NULL);
		if(bResult && dwReadByte)
		{
			iTime = SysTimeToInt(jb.time);
			if(iTime)
				jobs.insert(std::make_pair(iTime, jb));
		}
		else
			break;
	}
	CloseHandle(hFile);
	return 0;
}

int JobList::SaveFile(TCHAR* fpath)
{
	TCHAR szBuff[STRING_LENGTH] = {0};
	HANDLE hFile = CreateFile(
		fpath,
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wsprintf(szBuff,TEXT("Не удалось создать файл %s."), fpath);
		MessageBox(NULL, szBuff, TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return GetLastError();
	}
	
	if(!jobs.empty())
	{
		JOBITER iter = jobs.begin();
		DWORD dwWriteByte = 0;
		while(iter != jobs.end())
		{
			JOB jb = (*iter).second;
			if(!WriteFile(hFile, (LPVOID)&jb, sizeof(JOB), &dwWriteByte, NULL))
			{
				MessageBox(NULL, TEXT("Не удалось записать файл"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
				return GetLastError();
			}
			iter++;
		}
	}
	CloseHandle(hFile);
	return 0;
}

ULONGLONG JobList::SysTimeToInt(SYSTEMTIME sTime)
{
	FILETIME fTime = {0};
	if(!SystemTimeToFileTime(&sTime, &fTime))
	{
		return 0;
	}
	return (((ULONGLONG) fTime.dwHighDateTime) << 32) + fTime.dwLowDateTime;
}

SYSTEMTIME JobList::IntToSysTime(ULONGLONG iTime)
{
	SYSTEMTIME sTime = {0};
	FILETIME fTime = {0};
	fTime.dwLowDateTime  = (DWORD) (iTime & 0xFFFFFFFF );
	fTime.dwHighDateTime = (DWORD) (iTime >> 32 );
	FileTimeToSystemTime(&fTime, &sTime);
	return sTime;
}

void JobList::SetStatus()
{
	if(jobs.empty())
	{
		return;
	}

	SYSTEMTIME currentTime = {0};
	ULONGLONG iCurrentTime = 0;
	GetLocalTime(&currentTime);
	iCurrentTime = SysTimeToInt(currentTime);
	if(iCurrentTime)
	{
		JOBITER jIter = jobs.begin();
		while(jIter != jobs.end())
		{
			if( (*jIter).first < iCurrentTime )
			{
				(*jIter).second.status = DEACTIVE;
			}
			else
			{
				(*jIter).second.status = ACTIVE;
			}
			jIter++;
		}
	}
}