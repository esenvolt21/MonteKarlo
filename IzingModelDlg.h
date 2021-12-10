
// IzingModelDlg.h: файл заголовка
//

#pragma once
#include <vector>
#include "CDrawGraph.h"
using namespace std;

// Диалоговое окно CIzingModelDlg
class CIzingModelDlg : public CDialogEx
{
// Создание
public:
	CIzingModelDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IZINGMODEL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// Объекты для создания потока.
	DWORD dwThread;
	HANDLE hThread;
	BOOL bRunTh = false;

	// Надписи на кнопке визуализации.
	CString bStartVisualization = L"Запустить процедуру вычисления";
	CString bStopVisualization = L"Остановить процедуру вычисления";

	// Области рисования.
	CWnd* PicWndImage;
	CDC* PicDcImage;
	CRect PicImage;

	// Диалог построения графиков.
	CDrawGraph* pGraphDialog = nullptr;

	double xpImage = 0, ypImage = 0,			//коэфициенты пересчета
		xminImage = -1, xmaxImage = 1,			//максисимальное и минимальное значение х 
		yminImage = -0.5, ymaxImage = 5;			//максисимальное и минимальное значение y

	// Критическая температура, Ecm/k.
	double T_CRITICAL = 2.2554;
	// Постоянная Больцмана, Дж/К
	double K = 1.380649e-23;

public:
	afx_msg void OnBnClickedCalculate();
	afx_msg void OnBnClickedPicture();
	afx_msg void OnBnClickedDropping();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();

	int value_size;
	CButton radio_XY;
	CButton radio_YZ;
	CButton radio_ZX;
	CButton radio_with_graph;
	CButton radio_without_graph;
	CMFCButton button_calculate;
	CMFCButton button_picture;
	CMFCButton button_dropping;
	CMFCButton button_plots;
	double Ecm;
	double TEMPERATURE;
	int MKSH_QOUNT;
	CStatic CURRENT_MKSH_STEP;
	CButton check_GU;

	vector<vector<vector<int>>> vecIzingModel;

	int RandStaff(int min, int max);
	void DrawImage(vector<vector<vector<int>>> vec, CDC* WinDc, CRect WinxmaxGraphc);
	vector<int> BorderConditions(int rand_idx);
	double CalculateHamiltonian(int i, int j, int k, int n_i, int n_j, int n_k, vector<vector<vector<int>>> new_cfg);
	void MonteCarloStep();
	void MonteCarlo();
	afx_msg void OnBnClickedOpenGraphDialog();
};
