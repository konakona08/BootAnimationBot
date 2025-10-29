/*
** ===========================================================================
** File: Bot_Debug.cpp
** Description: Bot custom code
** Copyright (c) 2023 raulmrio28-git and contributors.
** Licensed under the Apache License, Version 2.0. All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/08/2023	raulmrio28-git	Simplify debug print function to change the
**								type string, not printf for every type
** 03/04/2023	raulmrio28-git	Initial version
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "Bot_Debug.h"

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define  CONSTSTR_DEBUG     "[->DEBUG<-] "
#define  CONSTSTR_INFO      "[->INFO<-] "
#define  CONSTSTR_WARNING   "[->WARNING<-] "
#define  CONSTSTR_ERROR     "[->ERROR<-] "

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Global variables
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Internal variables
**---------------------------------------------------------------------------
*/

char szBuffer[1024]; /* temp buffer */
time_t rawtime_T; /* time_t */
struct tm timeinfo_T; /* time struct */
char* pszBuffer; /* temp ptr buffer */

/*
**---------------------------------------------------------------------------
**  Function(internal use only) Declarations
**---------------------------------------------------------------------------
*/

/*
** ---------------------------------------------------------------------------
**
** Function:
**     pszGetDate()
**
** Description:
**     Get date of the system to a char string
**
** Input:
**     none
**
** Output:
**     Date during execution
**
** Return value:
**     pszBuffer
** 
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/04/2023	raulmrio28-git	Initial version
** ---------------------------------------------------------------------------
*/

char* pszGetDate()
{
	time(&rawtime_T);
	localtime_s(&timeinfo_T,&rawtime_T);
	pszBuffer = (char*)malloc(11);
	if (pszBuffer != NULL)
		sprintf_s(pszBuffer, _msize(pszBuffer), "%02d/%02d/%04d", timeinfo_T.tm_mday, timeinfo_T.tm_mon + 1, timeinfo_T.tm_year + 1900);
	return pszBuffer;
}

/*
** ---------------------------------------------------------------------------
**
** Function:
**     pszGetTime()
**
** Description:
**     Get time of the system to a char string
**
** Input:
**     none
**
** Output:
**     Time during execution
**
** Return value:
**     pszBuffer
**
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/04/2023	raulmrio28-git	Initial version
** ---------------------------------------------------------------------------
*/

char* pszGetTime()
{
	time(&rawtime_T);
	localtime_s(&timeinfo_T, &rawtime_T); 
	pszBuffer = (char*)malloc(9);
	if (pszBuffer != NULL)
		sprintf_s(pszBuffer, _msize(pszBuffer), "%02d:%02d:%02d", timeinfo_T.tm_hour, timeinfo_T.tm_min, timeinfo_T.tm_sec);
	return pszBuffer;
}

/*
** ---------------------------------------------------------------------------
**
** Function:
**     Bot_Log()
**
** Description:
**     Debug logging
**
** Input:
**     nLevel - Type of information (LOG_TYPES_E)
**     pszFile - File (char*)
**     pszLine - Line (int)
**     pszFunction - Function (char*)
**     pszString - String (char*)
**     ...
**
** Output:
**     Debug information in stdout
**
** Return value:
**     none
**
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/08/2023	raulmrio28-git	Simplify debug print function to change the type string, not printf for every type
** 03/04/2023	raulmrio28-git	Initial version
** ---------------------------------------------------------------------------
*/

void Bot_Log (LOG_TYPES_E nLevel, const char *pszFile, int nLine, const char *pszFunction, const char* pszString, ...)
{
	char* pszDebugType = NULL;
	va_list argList;
	va_start(argList, pszString);
	vsprintf_s(szBuffer, sizeof(szBuffer), pszString, argList);
	va_end(argList);
	switch (nLevel)
	{
	case LOGLEVEL_DEBUG:
		pszDebugType = (char*)CONSTSTR_DEBUG;
		break;
	case LOGLEVEL_INFO:
		pszDebugType = (char*)CONSTSTR_INFO;
		break; 
	case LOGLEVEL_WARN:
		pszDebugType = (char*)CONSTSTR_WARNING;
		break;
	case LOGLEVEL_ERROR:
		pszDebugType = (char*)CONSTSTR_ERROR;
		break;
	default:
		break;
	}
	printf("%s[%s-%s] FILE:%s LINE:%d %s->%s\n", pszDebugType, pszGetDate(), pszGetTime(), pszFile, nLine, pszFunction, szBuffer);
	free(pszBuffer);
	memset(szBuffer, sizeof(szBuffer), 0);
}