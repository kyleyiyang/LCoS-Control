
// LCoS ControlDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "LCoS Control.h"
#include "LCoS ControlDlg.h"
#include "afxdialogex.h"
#include "LCoS_Settings.h"

#include "resource.h"
#include <string>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
#include <vector>

// SDK
#include "Blink_C_wrapper.h"  // Relative path to SDK header.
#include "math.h"
#include <Windows.h>
#include <iostream>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION  // For loading images
#include "stb_image.h"  // For loading images
// End SDK

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#include <thread>
//#include <Afxwin.h>
//#define _AFXDLL


CString folder("C:\\Program Files\\Meadowlark Optics\\Blink OverDrive Plus\\Image Files\\frame rate");
CString LUT_file_1("C:\\Program Files\\Meadowlark Optics\\Blink OverDrive Plus\\LUT Files\\slm5500_at1550_amplitude.LUT");
CString LUT_file_2("C:\\Program Files\\Meadowlark Optics\\Blink OverDrive Plus\\LUT Files\\slm5495_at1550_amplitude.LUT");

int board_number, height, width, dwell_time, n_loop;
double temp1, temp2;
char* lut_file_1, lut_file_2;

bool ExternalTrigger = false;
bool OutputPulse = false;
bool is_looping;

unsigned char* Image = new unsigned char[width * height];

LCoS_Settings LCoSSettings;

//int Zeroth_Order;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLCoSControlDlg dialog



CLCoSControlDlg::CLCoSControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LCOSCONTROL_DIALOG, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(143);
}
CLCoSControlDlg::~CLCoSControlDlg(){
	//std::ofstream cFile("C:\\Program Files\\Meadowlark Optics\\Blink OverDrive Plus\\SDK\\test.txt");
	std::ofstream cFile("Preferences.ini");
	if (cFile.is_open())
	{
		cFile << "[User Preferences]" << std::endl;
		cFile << "Dwell Time=" << dwell_time << std::endl;
		cFile << "LUT_1=" << CString2String(LUT_file_1) << std::endl;
		cFile << "LUT_2=" << CString2String(LUT_file_2) << std::endl;
		cFile << "Num Loops=" << n_loop << std::endl;
		cFile << "Img Folder=" << CString2String(folder) << std::endl;
		cFile << "Input Trigger=" << ExternalTrigger << std::endl;
		
		cFile.close();
	}
	else
	{
		AfxMessageBox(L"Couldn't open config file for writing.\n");
	}
	Delete_SDK();
}
void CLCoSControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_fold_path, m_folder);
	DDX_Control(pDX, IDC_LIST_file_list, m_listBox);
	DDX_Control(pDX, IDC_STATIC_image_display, m_img_preview);
	DDX_Control(pDX, IDC_SLIDER_dwell_time, m_slider_dwell_time);
	DDX_Control(pDX, IDC_SPIN_LCoS_select, m_sel_LCoS);
	DDX_Control(pDX, IDC_STATIC_LCoS_select, m_show_sel_LCoS);
	DDX_Control(pDX, IDC_STATIC_temp, m_temp);
	DDX_Control(pDX, IDC_EDIT_n_loops, m_loop);
	DDX_Control(pDX, IDC_CHECK_trigger, m_trigger);
	DDX_Control(pDX, IDC_BUTTON_start, m_loop_start);
}

BEGIN_MESSAGE_MAP(CLCoSControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_dwell_time, &CLCoSControlDlg::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BUTTON_start, &CLCoSControlDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_browse, &CLCoSControlDlg::OnBnClickedButtonbrowse)
	ON_LBN_SELCHANGE(IDC_LIST_file_list, &CLCoSControlDlg::OnLbnSelchangeListfilelist)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SPIN_LCoS_select, &CLCoSControlDlg::OnNMReleasedcaptureSpinLcosselect)
	ON_BN_CLICKED(IDC_BUTTON_settings, &CLCoSControlDlg::OnBnClickedButtonsettings)
	//ON_BN_CLICKED(IDC_BUTTON1_Setting_OK, &CLCoSControlDlg::OnBnClickedButton1SettingOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LCoS_select, &CLCoSControlDlg::OnDeltaposSpinLcosselect)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_dwell_time, &CLCoSControlDlg::OnNMReleasedcaptureSliderdwelltime)
	ON_EN_CHANGE(IDC_EDIT_n_loops, &CLCoSControlDlg::OnEnChangeEditnloops)
	ON_EN_UPDATE(IDC_EDIT_n_loops, &CLCoSControlDlg::OnEnUpdateEditnloops)
	ON_BN_CLICKED(IDC_BUTTON_stop, &CLCoSControlDlg::OnBnClickedButtonstop)
	ON_BN_CLICKED(1026, &CLCoSControlDlg::OnBnClickedRadio1lampon)
	ON_BN_CLICKED(1027, &CLCoSControlDlg::OnBnClickedRadio2LampOff)
END_MESSAGE_MAP()


// CLCoSControlDlg message handlers

BOOL CLCoSControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// Construct a Blink_SDK instance
	unsigned int bits_per_pixel = 12U;   //12U is used for 1920x1152 SLM, 8U used for the small 512x512
	bool         is_nematic_type = true;
	bool         RAM_write_enable = true;
	bool         use_GPU_if_available = true;
	unsigned int n_boards_found = 0U;
	int         constructed_okay = true;
	Create_SDK(bits_per_pixel, &n_boards_found, &constructed_okay, is_nematic_type, RAM_write_enable, use_GPU_if_available, 10U, 0);

	// return of 0 means okay, return -1 means error
	if (constructed_okay == 0)
	{
		board_number = n_boards_found;
		height = Get_image_height(board_number);
		width = Get_image_width(board_number);
		temp1 = Read_SLM_temperature(1);
		m_sel_LCoS.SetRange(1, 1);
		//dwell_time = 333; 
		//n_loop = 100;
				
		// read variables from Preferences.ini
		// std::ifstream is RAII, i.e. no need to call close
		//std::ifstream cFile("C:\\Program Files\\Meadowlark Optics\\Blink OverDrive Plus\\SDK\\test.txt");
		std::ifstream cFile("Preferences.ini");
		if (cFile.is_open())
		{
			std::string line;
			while (std::getline(cFile, line))
			{
				//line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
				if (line[0] == '#' || line.empty()) continue;

				auto delimiterPos = line.find("=");
				auto name = line.substr(0, delimiterPos);
				auto value = line.substr(delimiterPos + 1);

				//Custom coding
				if (name == "Dwell Time") { dwell_time = std::stoi(value); }
				else if (name == "LUT_1") {LUT_file_1 = value.c_str();}
				else if (name == "LUT_2") { LUT_file_2 = value.c_str(); }
				else if (name == "Num Loops") { n_loop = std::stoi(value); }
				else if (name == "Img Folder") { folder = value.c_str(); }
				else if (name == "Input Trigger") { ExternalTrigger = std::stoi(value); }
			}
		}
		else
		{
			AfxMessageBox(L"Couldn't open config file for reading.\n");
		}

		m_sel_LCoS.SetPos(1);
		m_slider_dwell_time.SetRange(0, 1000, 0);
		m_slider_dwell_time.SetPos(dwell_time);
		SetDlgItemText(IDC_STATIC_temp, std::to_wstring(temp1).c_str());
		SetDlgItemText(IDC_STATIC_LCoS_select, std::to_wstring(board_number).c_str());
		SetDlgItemText(IDC_EDIT_n_loops, std::to_wstring(n_loop).c_str());
		m_trigger.SetCheck(ExternalTrigger);

		Load_LUT_File(1, LUT_file_1);
		if (board_number == 2) {
			temp2 = Read_SLM_temperature(2);
			m_sel_LCoS.SetRange(1, 2);
			//n_loop_2 = 100;
			Load_LUT_File(2, LUT_file_2);
		}
		LoadListBox();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLCoSControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLCoSControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLCoSControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLCoSControlDlg::Load_LUT_File(int n, CString lut_file) {
	std::string lut = CString2String(lut_file);
	std::vector<char> writable(lut.begin(), lut.end());
	writable.push_back('\0');
	// get the char* using &writable[0] or &*writable.begin()
	Load_LUT_file(n, &writable[0]);
}

void CLCoSControlDlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//int sel_LCoS = m_sel_LCoS.GetPos();
	dwell_time = m_slider_dwell_time.GetPos();
	SetDlgItemText(IDC_STATIC_dwell_time, std::to_wstring(dwell_time).c_str());
	//sel_LCoS == 1 ? dwell_time_1 = dwell_time : dwell_time_2 = dwell_time;
}

static UINT MyThreadProc(LPVOID pParam) {
	CLCoSControlDlg* pDlg = (CLCoSControlDlg*)pParam;
	return pDlg->Looping();
}
UINT CLCoSControlDlg::Looping() {
	is_looping = true;

	//int size = m_listBox.GetAccessibleChildCount();
	std::vector< unsigned char* > arr;

	std::string path;
	std::string strStd = CString2String(folder);
	int n = 1;
	for (auto& entry : fs::directory_iterator(strStd)) {
		path = entry.path().string();
		Image = stbi_load(path.c_str(), &width, &height, &n, 0);
		arr.push_back(Image);
	}
	int ImgSize = width * height;
	int arrSize = arr.size();
	for (int i = 0; i < n_loop; i++)
	{
		if (is_looping) {
			//write image returns on DMA complete, ImageWriteComplete returns when the hardware
			//image buffer is ready to receive the next image. Breaking this into two functions is 
			//useful for external triggers. It is safe to apply a trigger when Write_image is complete
			//and it is safe to write a new image when ImageWriteComplete returns
			for (int j = 0; j < arrSize; j++) {
				Write_image(1, arr[j], ImgSize, ExternalTrigger, OutputPulse, 5000);
				if (board_number == 2)
					Write_image(2, arr[j], ImgSize, ExternalTrigger, OutputPulse, 5000);
					ImageWriteComplete(2, 5000);
				ImageWriteComplete(1, 5000);
				if (!ExternalTrigger) {
					if (dwell_time < 4) {
						dwell_time = 3.5;
					}
					Sleep(dwell_time - 3.5);
				}
			}
		}
		else {
			break;
		}
	}
	for (int k = 0; k < arrSize; k++) {
		stbi_image_free(arr[k]);
	}
	AfxMessageBox(L"Sequencing is stopped.");
	//updateControls();
	return 1;
}
void CLCoSControlDlg::updateControls(){
	m_loop_start.EnableWindow(TRUE);
}
void CLCoSControlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	m_loop_start.EnableWindow(FALSE);
	ExternalTrigger = (m_trigger.GetCheck() == 1) ? true : false;
	int* bright = new int(0);
	AfxBeginThread(MyThreadProc, (LPVOID)bright); // <<== START THE THREAD
	
	/*LPDWORD W = (LPDWORD)1;
	if (GetExitCodeThread(MyThreadProc,W)) {
		m_loop_start.EnableWindow(TRUE);
	}*/
	
}

void CLCoSControlDlg::LoadListBox() {
	CString fileName;
	m_listBox.ResetContent();
	SetDlgItemText(IDC_STATIC_fold_path, folder);
	std::string strStd = CString2String(folder);
	for (auto& entry : fs::directory_iterator(strStd)) {
		fileName = entry.path().filename().c_str();
		m_listBox.AddString(fileName);
	}
	m_listBox.SetCurSel(0);
	
	ImgPreview();
}


void CLCoSControlDlg::OnBnClickedButtonbrowse()
{
	// TODO: Add your control notification handler code here

	CFolderPickerDialog dlgFolder;
	if (dlgFolder.DoModal() == IDOK) {
		folder = dlgFolder.GetFolderPath();
		//AfxMessageBox(dlgFolder.GetFolderPath());
		SetDlgItemText(IDC_STATIC_fold_path, folder);
	}
	LoadListBox();
}
std::string CLCoSControlDlg::CString2String(CString cs) {
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString(cs);
	// construct a std::string using the LPCSTR input
	std::string strStd(pszConvertedAnsiString);

	return strStd;
}

void CLCoSControlDlg::ImgPreview() {
	int sel = m_listBox.GetCurSel();
	CString file;
	m_listBox.GetText(sel, file);

	std::string strStd = CString2String(folder);
	std::string fileN = CString2String(file);
	std::string path = strStd + "\\" + fileN;
	std::wstring ws;
	ws.assign(path.begin(), path.end());
	// get temporary LPWSTR (not really safe)
	LPWSTR pwst = &ws[0];
	// get temporary LPCWSTR (pretty safe)
	LPCWSTR pcwstr = ws.c_str();

	m_img_preview.SetBitmap((HBITMAP)LoadImage(0, pcwstr, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

	
	unsigned char* Image = new unsigned char[width * height];
	int n = 1;
	Image = stbi_load(path.c_str(), &width, &height, &n, 0);

	int sel_LCoS = m_sel_LCoS.GetPos();
	/*double temp = sel_LCoS == 1 ? temp1 : temp2;
	SetDlgItemText(IDC_STATIC_temp, std::to_wstring(temp).c_str());*/
	ExternalTrigger = false;
	Write_image(sel_LCoS, Image, width * height, ExternalTrigger, OutputPulse, 5000);
	ImageWriteComplete(sel_LCoS, 5000);
}


void CLCoSControlDlg::OnLbnSelchangeListfilelist()
{
	// TODO: Add your control notification handler code here
	ImgPreview();
}


void CLCoSControlDlg::OnNMReleasedcaptureSpinLcosselect(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
}


void CLCoSControlDlg::OnBnClickedButtonsettings()
{
	// TODO: Add your control notification handler code here
	INT_PTR returnCode = -1;
	int sel_LCoS = m_sel_LCoS.GetPos();
	
	LCoSSettings.sel_LCoS = sel_LCoS;
	LCoSSettings.lut_1 = LUT_file_1;
	LCoSSettings.lut_2 = LUT_file_2;
	
	returnCode = LCoSSettings.DoModal();
	
	switch (returnCode) {
	case IDOK:
		//gather your input fields here 
		sel_LCoS == 1 ? LUT_file_1 = LCoSSettings.lut_1 : LUT_file_2 = LCoSSettings.lut_2;
		break;
	case IDCANCEL:
		//do something
		break;

	case -1:
	default:
		//error creating box
		break;
	};
}


void CLCoSControlDlg::OnBnClickedButton1SettingOk()
{
	// TODO: Add your control notification handler code here
	
}


void CLCoSControlDlg::OnDeltaposSpinLcosselect(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int sel_LCoS = m_sel_LCoS.GetPos() % 2 + 1;
	double temp = (sel_LCoS == 1) ? temp1 : temp2;
	SetDlgItemText(IDC_STATIC_temp, std::to_wstring(temp).c_str());

}


void CLCoSControlDlg::OnNMReleasedcaptureSliderdwelltime(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	//int sel_LCoS = m_sel_LCoS.GetPos();
	dwell_time = m_slider_dwell_time.GetPos();
	SetDlgItemText(IDC_STATIC_dwell_time, std::to_wstring(dwell_time).c_str());
	//sel_LCoS == 1 ? dwell_time_1 = dwell_time : dwell_time_2 = dwell_time;
}


void CLCoSControlDlg::OnEnChangeEditnloops()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	int sel_LCoS_1 = m_sel_LCoS.GetPos();
	CString nloop;
	m_loop.GetWindowTextW(nloop);
	n_loop = _wtoi(nloop);
	//(sel_LCoS_1 == 1) ? (n_loop_1 = _wtoi(n_loop)) : (n_loop_2 = _wtoi(n_loop));
	//m_listBox.AddString(n_loop);
}


void CLCoSControlDlg::OnEnUpdateEditnloops()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here

}


void CLCoSControlDlg::OnBnClickedButtonstop()
{
	// TODO: Add your control notification handler code here

	is_looping = false;
	m_loop_start.EnableWindow(TRUE);
}

// RS232
bool CLCoSControlDlg::WriteComPort(CString PortSpecifier, CString data) {
	DCB dcb;  DWORD byteswritten;

	//HANDLE hPort = CreateFile(PortSpecifier, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	HANDLE hPort = CreateFile(PortSpecifier, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	//HANDLE hPort = CreateFile(L"COM1", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (hPort == INVALID_HANDLE_VALUE) {
		AfxMessageBox(L"error opening port");
		return false;
	}

	CString strMessage("Comm State Invalid");
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);
	if (!GetCommState(hPort, &dcb))
	//if (!GetCommState(hPort, &serialParams))
		strMessage.Format(_T("Failed to Get Comm State Reason: %d", GetLastError()));
		AfxMessageBox(strMessage);
		return false;

	dcb.BaudRate = CBR_9600;  //9600 Baud  
	dcb.ByteSize = 8;   //8 data bits  
	dcb.Parity = NOPARITY;  //no parity  
	dcb.StopBits = ONESTOPBIT;  //1 stop 

	if (!SetCommState(hPort, &dcb))   
		strMessage.Format(_T("Failed to Set Comm State Reason: %d", GetLastError()));
		AfxMessageBox(strMessage);
		return false;

	bool retVal = WriteFile(hPort, data, 1, &byteswritten, NULL);  
	CloseHandle(hPort);   //close the handle  
	return retVal; 
}

int CLCoSControlDlg::ReadByte(CString PortSpecifier) {
	DCB dcb;  int retVal;  BYTE Byte;  DWORD dwBytesTransferred;  DWORD dwCommModemStatus;

	HANDLE hPort = CreateFile(PortSpecifier, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hPort == INVALID_HANDLE_VALUE) {
		AfxMessageBox(L"error opening port");
		return false;
	}

	CString strMessage("Comm State Invalid");
	if (!GetCommState(hPort, &dcb))   
		strMessage.Format(_T("Failed to Get Comm State Reason: %d", GetLastError()));
		AfxMessageBox(strMessage);
		return 0x100;

	dcb.BaudRate = CBR_9600;  //9600 Baud  
	dcb.ByteSize = 8;   //8 data bits  
	dcb.Parity = NOPARITY;  //no parity  
	dcb.StopBits = ONESTOPBIT;  //1 stop 

	if (!SetCommState(hPort, &dcb))   
		strMessage.Format(_T("Failed to Set Comm State Reason: %d", GetLastError()));
		AfxMessageBox(strMessage);
		return 0x100;

	SetCommMask(hPort, EV_RXCHAR | EV_ERR); //receive character event   
	WaitCommEvent (hPort, &dwCommModemStatus, 0); //wait for character 

	if (dwCommModemStatus & EV_RXCHAR)    
		ReadFile(hPort, &Byte, 1, &dwBytesTransferred, 0);  //read 1  
	else if (dwCommModemStatus & EV_ERR)   
		retVal = 0x101;  
	retVal = Byte;  
	CloseHandle(hPort);  
	return retVal; 
} 





void CLCoSControlDlg::OnBnClickedRadio1lampon()
{
	// TODO: Add your control notification handler code here
	WriteComPort(L"\\\\.\\COM1", L"STOP");
	//ReadByte(L"\\\\.\\COM1");
}


void CLCoSControlDlg::OnBnClickedRadio2LampOff()
{
	// TODO: Add your control notification handler code here
	WriteComPort(L"\\\\.\\COM1", L"STOP");
}
