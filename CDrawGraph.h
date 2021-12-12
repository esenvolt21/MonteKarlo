#pragma once
#include "afxdialogex.h"
#include <vector>

using namespace std;
// Диалоговое окно CDrawGraph

class CDrawGraph : public CDialogEx
{
	DECLARE_DYNAMIC(CDrawGraph)

public:
	CDrawGraph(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~CDrawGraph();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DRAW_GRAPH };
#endif

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	// Области рисования.
	CWnd* PicWndGraphEnergy;
	CDC* PicDcGraphEnergy;
	CRect PicGraphEnergy;

	CWnd* PicWndGraphCapacity;
	CDC* PicDcGraphCapacity;
	CRect PicGraphCapacity;

	// Объекты для создания потока.
	DWORD dwThreadGraphs;
	HANDLE hThreadGraphs;
	BOOL bRunTh = false;

	CString bStartVisualization = L"Запустить процедуру вычисления";
	CString bStopVisualization = L"Остановить процедуру вычисления";

	COLORREF BUTTONS_COLOR = RGB(255, 255, 255);

	double xpGraphEnergy = 0, ypGraphEnergy = 0,			//коэфициенты пересчета
		xminGraphEnergy = -1, xmaxGraphEnergy = 1,			//максисимальное и минимальное значение х 
		yminGraphEnergy = -0.5, ymaxGraphEnergy = 5;			//максисимальное и минимальное значение y

	double xpGraphСapacity = 0, ypGraphСapacity = 0,			//коэфициенты пересчета
		xminGraphСapacity = -1, xmaxGraphСapacity = 1,			//максисимальное и минимальное значение х 
		yminGraphСapacity = -0.5, ymaxGraphСapacity = 5;			//максисимальное и минимальное значение y
public:
	int scale = 1;
	double defaultX0 = 0.0;
	double defaultY0 = 0.0;

	void DrawEnergy(vector<double> x, vector<double> y,CDC* WinDc, CRect WinPic);
	void DrawСapacity(vector<double> x, vector<double> y, CDC* WinDc, CRect WinPic);
	void CalculateGraphs();
	
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedCalcButton();
	afx_msg void OnBnClickedClearButton();
	CMFCButton button_calculate;
	CMFCButton button_clear;
	CStatic CURRENT_STEP;
	CStatic CURRENT_TEMPERATURE;
};
