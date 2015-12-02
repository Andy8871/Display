// NewVsIndicator.cpp: implementation of the CNewVsIndicator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "display.h"
#include "NewVsIndicator.h"
#include "RunEnvironment.h"

// #include  <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const string g_strNewVsIndicatorFileName = "NewVs.ind";

CNewVsIndicator::CNewVsIndicator()
{

}

CNewVsIndicator::~CNewVsIndicator()
{

}

void CNewVsIndicator::CreateNewVSIndicator()
{
}

void CNewVsIndicator::DestoryNewVSIndicator()
{
}

bool CNewVsIndicator::IsNewVehicleSelection()
{
	string strFileName = CRunEnvironment::GetDisplayDirectory() + g_strNewVsIndicatorFileName;

// 	if( (_access(strFileName.c_str(), 0 )) != -1 )
	FILE *stream = NULL;
	stream = fopen(strFileName.c_str(),"r");
	if(stream!=NULL)
	{
		fclose(stream);
		return true;
	}
	else
	{
		return false;
	}

}
