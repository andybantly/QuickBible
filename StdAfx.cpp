// stdafx.cpp : source file that includes just the standard includes
//	Project.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information
// Copyright (C) 2007 Andrew S. Bantly

#include "stdafx.h"
#include <vector>

// Output to the debugger window
void DebugMsg(const char * pszFormat,...)
{
	char buf[1024] = {'\0'};
	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf(&buf[0],pszFormat,arglist);
	va_end(arglist);
	OutputDebugString(&buf[0]);
}