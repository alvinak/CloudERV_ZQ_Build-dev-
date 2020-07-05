// BookingOfRISView.h : interface of the CBookingOfRISView class
#pragma once

#include <vector>

class CBookingOfRISDoc;

struct DayMetricsCallbackParamsInfo
{
	int nMaxBookingDays;
};


class CQueueGroupInfoOfRIS
{
public:
	CQueueGroupInfoOfRIS();
	~CQueueGroupInfoOfRIS();
	void ReadFromQuery(SACommand &cmd);
public:
	CString m_sQueueGroupID;
	CString m_sQueueGroupName;
	CString m_sExamType;
	CString m_sLocation;
	CString m_sMemo;

	int		m_nCapacityD1_AM;	// 周１上午
	int		m_nCapacityD1_PM;	// 周１下午
	int		m_nCapacityD2_AM;
	int		m_nCapacityD2_PM;
	int		m_nCapacityD3_AM;
	int		m_nCapacityD3_PM;
	int		m_nCapacityD4_AM;
	int		m_nCapacityD4_PM;
	int		m_nCapacityD5_AM;
	int		m_nCapacityD5_PM;
	int		m_nCapacityD6_AM;
	int		m_nCapacityD6_PM;
	int		m_nCapacityD7_AM;
	int		m_nCapacityD7_PM;
};


class CQueueGroupBookingInfoOfRIS
{
public:
	CQueueGroupBookingInfoOfRIS(const TCHAR *szQueueGroupID, const TCHAR *szQueueGroupName, const TCHAR *szExamType, const TCHAR *szLocation, const TCHAR *szMemo,
		int nCapacity_AM[7], int nCapacity_PM[7], COleDateTime dtFirstDay);
	~CQueueGroupBookingInfoOfRIS();
public:

	CString m_sQueueGroupID;
	CString m_sQueueGroupName;
	CString m_sExamType;
	CString m_sLocation;
	CString m_sMemo;

	COleDateTime m_dtFirstDay;

	int		m_nBookingAmount_AM[28];	// 最多预约28天
	int		m_nBookingAmount_PM[28];	// 最多预约28天
	int		m_nCapacity_AM[7];			// 上午可预约量
	int		m_nCapacity_PM[7];			// 下午可预约量
};



class CBookingOfRISView : public CXTPResizeFormView
{
protected: // create from serialization only
	CBookingOfRISView();
	DECLARE_DYNCREATE(CBookingOfRISView)

// Attributes
public:
	enum { IDD = IDD_BOOKINGOFRIS };

	CBookingOfRISDoc* GetDocument() const;

	int m_nMaxBookingDays;

	CXTPDatePickerControl m_ctlDatePicker;

	CXTPReportControl	m_ctlQueueGroups;
	CXTPReportControl   m_ctlQueueGroupsBookingInfo;

	CStatic	m_ctlOrderNo;
	CStatic m_ctlExamType;
	CStatic m_ctlPatientName;
	CEdit	m_ctlServiceNames;
	CStatic m_ctlQueueGroupID;
	CStatic m_ctlScheduleDate;
	CStatic m_ctlScheduleTime;

	DayMetricsCallbackParamsInfo m_DayMetricsCallbackParams;

	std::vector <CQueueGroupInfoOfRIS *> m_lstQueueGroupInfoOfRIS;
	std::vector <CQueueGroupBookingInfoOfRIS *> m_lstQueueGroupBookingInfoOfRIS;

	CString			m_sExamTypeOfMedicalOrder;
	CString			m_sScheduledDates[28];
	COleDateTime	m_dtToday;
	COleDateTime	m_dtFirstDay, m_dtLastDay;

	CImageList m_ilIcons;

private:
	void SetupScheduledDates();
	int GetIndexOfScheduledDate(const TCHAR *szScheduledDate);

	void NewBooking();

// Operations
public:

	void LoadQueueGroupsInfo();

	void LoadBookingRecords();

	/////////////////////////////////////////////////////
// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CBookingOfRISView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBookingReportSelChanged(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnBookingReportItemButtonClick(NMHDR * pNotifyStruct, LRESULT*);
	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);
	afx_msg void OnExecuteActions(UINT nCommandID);
};

#ifndef _DEBUG  // debug version in BookingRISView.cpp
inline CBookingOfRISDoc* CBookingOfRISView::GetDocument() const
   { return reinterpret_cast<CBookingOfRISDoc*>(m_pDocument); }
#endif

