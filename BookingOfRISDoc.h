// BookingOfRISDoc.h : interface of the CBookingOfRISDoc class
//

#pragma once



class CMedicalOrderOfRIS
{
public:
	CMedicalOrderOfRIS();
	~CMedicalOrderOfRIS();
public:
	CString m_sOrderNo;
	CString m_sExamType;
	CString m_sPatientName;
	CString m_sServiceNames;
	CString m_sQueueGroupID;
	CString m_sScheduledDate;
	CString m_sScheduledTime;
};


class CBookingOfRISDoc : public CDocument
{
protected: // create from serialization only
	CBookingOfRISDoc();
	DECLARE_DYNCREATE(CBookingOfRISDoc)

// Attributes
public:
	BOOL	m_bInitialized;
	CString	m_sTitle;

	CMedicalOrderOfRIS m_objMedicalOrder;
	
	CString m_sQueueGroupID;
	CString	m_sScheduledDate;
	CString m_sScheduledTime;

// Operations
public:

	const TCHAR *GetTitle() { return m_sTitle; }

	const TCHAR *GetQueueGroupID();
	const TCHAR *GetScheduledDate();
	const TCHAR *GetScheduledTime();

	void SetQueueGroupID(const TCHAR *szQueueGroupID);
	void SetScheduledDate(const TCHAR *szScheduledDate);
	void SetScheduledTime(const TCHAR *szScheduledTime);

	const TCHAR *GetExamTypeOfMedicalOrder();

	BOOL OpenOrder(const TCHAR *szOrderNo);


// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CBookingOfRISDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
