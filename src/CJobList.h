#pragma once
#include "mystdafx.h"

enum JOBTYPE	{ ERR = -1, REMEMBER, RUNAPPLICATION, SHUTDOWN };
enum JOBSTATUS	{ ACTIVE, DEACTIVE, DONE, CANCELED };

struct JOB
{
	JOBTYPE		type:3;
	JOBSTATUS	status:3;
	SYSTEMTIME  time;
	TCHAR		description[STRING_LENGTH];
	TCHAR		path[STRING_LENGTH];
};

typedef std::multimap<ULONGLONG,JOB> JOBS;
typedef std::multimap<ULONGLONG,JOB>::iterator JOBITER;
typedef std::pair<ULONGLONG,JOB> JOBPAIR;
 
class JobList
{
	JOBS jobs;
	ULONGLONG  SysTimeToInt(SYSTEMTIME sTime);
	SYSTEMTIME IntToSysTime(ULONGLONG iTime);
public:
	JobList();
	bool IsEmpty() const;
	int  GetCount() const;
	void Clear();
	void AddJb(const JOB& job);
	void RemoveJb(SYSTEMTIME sTime);
	void RemoveJb(int index);
	JOB& GetJb(SYSTEMTIME sTime);
	JOB& GetJb(int index);
	int  OpenFile(TCHAR* path);
	int  SaveFile(TCHAR* path);
	void SetStatus();
};