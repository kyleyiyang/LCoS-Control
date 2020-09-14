
// LCoS ControlDlg.h : header file
//

#pragma once

#include <string>

// CLCoSControlDlg dialog
class CLCoSControlDlg : public CDialogEx
{
// Construction
public:
	CLCoSControlDlg(CWnd* pParent = nullptr);	// standard constructor
	~CLCoSControlDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LCOSCONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void Load_LUT_File(int n, CString lut_file);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	//UINT MyThreadProc(LPVOID Param);
	UINT Looping();
	void updateControls();
	afx_msg void OnBnClickedButton2();
	void LoadListBox();
	CStatic m_folder;
	afx_msg void OnBnClickedButtonbrowse();
	std::string CString2String(CString s);
	void ImgPreview();
	//void ImgPreview();
	CListBox m_listBox;
	CStatic m_img_preview;
	afx_msg void OnLbnSelchangeListfilelist();
	CSliderCtrl m_slider_dwell_time;
	CSpinButtonCtrl m_sel_LCoS;
	afx_msg void OnNMReleasedcaptureSpinLcosselect(NMHDR* pNMHDR, LRESULT* pResult);
	CStatic m_show_sel_LCoS;
	afx_msg void OnBnClickedButtonsettings();
	afx_msg void OnBnClickedButton1SettingOk();
	CStatic m_temp;
	afx_msg void OnDeltaposSpinLcosselect(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMReleasedcaptureSliderdwelltime(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit m_loop;
	afx_msg void OnEnChangeEditnloops();
	afx_msg void OnEnUpdateEditnloops();
	afx_msg void OnBnClickedButtonstop();
	CButton m_trigger;
	CButton m_loop_start;
};
