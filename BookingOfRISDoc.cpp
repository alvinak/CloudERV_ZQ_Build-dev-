// BookingOfRISDoc.cpp : implementation of the CBookingOfRISDoc class
//

#include "stdafx.h"
#include "BookingOfRISDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CMedicalOrderOfRIS::CMedicalOrderOfRIS()
{
	m_sOrderNo		= _T("");
	m_sExamType		= _T("");
	m_sPatientName	= _T("");
	m_sServiceNames	= _T("");
	m_sQueueGroupID	= _T("");
	m_sScheduledDate= _T("");
	m_sScheduledTime= _T("");
}

CMedicalOrderOfRIS::~CMedicalOrderOfRIS()
{

}


// CBookingOfRISDoc

IMPLEMENT_DYNCREATE(CBookingOfRISDoc, CDocument)

BEGIN_MESSAGE_MAP(CBookingOfRISDoc, CDocument)
END_MESSAGE_MAP()


// CBookingOfRISDoc construction/destruction

CBookingOfRISDoc::CBookingOfRISDoc()
{
	m_sTitle			= _T("");
	m_bInitialized		= FALSE;

	m_sQueueGroupID		= _T("");
	m_sScheduledDate	= _T("");
	m_sScheduledTime	= _T("");

}

CBookingOfRISDoc::~CBookingOfRISDoc()
{

}

BOOL CBookingOfRISDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle(_T("放射检查预约"));

	return TRUE;
}

// CBookingOfRISDoc serialization

void CBookingOfRISDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CBookingOfRISDoc diagnostics

#ifdef _DEBUG
void CBookingOfRISDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBookingOfRISDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CBookingOfRISDoc commands
const TCHAR * CBookingOfRISDoc::GetQueueGroupID()
{
	return m_sQueueGroupID;
}

const TCHAR * CBookingOfRISDoc::GetScheduledDate()
{
	return m_sScheduledDate;
}

const TCHAR * CBookingOfRISDoc::GetScheduledTime()
{
	return m_sScheduledTime; 
}

void CBookingOfRISDoc::SetQueueGroupID(const TCHAR *szQueueGroupID)
{
	m_sQueueGroupID = szQueueGroupID;
}

void CBookingOfRISDoc::SetScheduledDate(const TCHAR *szScheduledDate)
{
	m_sScheduledDate = szScheduledDate;
}

void CBookingOfRISDoc::SetScheduledTime(const TCHAR *szScheduledTime)
{
	m_sScheduledTime = szScheduledTime;
}

const TCHAR * CBookingOfRISDoc::GetExamTypeOfMedicalOrder()
{
	return m_objMedicalOrder.m_sExamType;
}

BOOL CBookingOfRISDoc::OpenOrder(const TCHAR *szOrderNo)
{

	m_objMedicalOrder.m_sExamType = _T("MRI");
	m_objMedicalOrder.m_sOrderNo = _T("XXXXXXXXXXXXXXXXXXXX");
	m_objMedicalOrder.m_sPatientName = _T("张三丰");
	m_objMedicalOrder.m_sServiceNames = _T("上腹部平扫");


	return TRUE;
}