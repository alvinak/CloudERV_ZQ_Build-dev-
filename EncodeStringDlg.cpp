// EncodeStringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "EncodeStringDlg.h"
#include "Aescrypt.h"
#include "resource.h"

// CEncodeStringDlg dialog

CEncodeStringDlg::CEncodeStringDlg(CWnd* pParent /*=NULL*/)
	: CXTPDialog(CEncodeStringDlg::IDD, pParent)
{

}

CEncodeStringDlg::~CEncodeStringDlg()
{
}

void CEncodeStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEncodeStringDlg, CXTPDialog)
	ON_BN_CLICKED(IDC_ENCODE, &CEncodeStringDlg::OnBnClickedEncode)
END_MESSAGE_MAP()


// CEncodeStringDlg message handlers


BOOL CEncodeStringDlg::OnInitDialog()
{
	CXTPDialog::OnInitDialog();



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CEncodeStringDlg::OnBnClickedEncode()
{
	CString sText(""), sEncodeStr("");
	std::string strIn(""), strAes(""), strOut("");
	CWnd *pWndIn, *pWndOut;

	pWndIn = GetDlgItem(IDC_EDITSTR);
	pWndOut = GetDlgItem(IDC_ENCODESTR);

	if (pWndIn) {
		pWndIn->GetWindowText(sText);
		if (sText.GetLength() > 0) {
			strIn = CStringA(sText);
			strAes = ::aes_encrypt(strIn, "");
			strOut = ::base64_encode(strAes);
		}
	}

	if (pWndOut)
		pWndOut->SetWindowText(CString(strOut.c_str()));

}
