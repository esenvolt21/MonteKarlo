#pragma once
#include <vector>
using namespace std;

class Energy
{
public:
	double energy = 0.0;
	double pow_energy = 0.0;
	bool is_need_calc = false;
};

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
	DWORD dwThreadGraphs;
	HANDLE hThreadGraphs;
	BOOL bRunTh = false;
	BOOL bRunThGraph = false;

	// Надписи на кнопке визуализации.
	CString bStartVisualization = L"Запустить процедуру вычисления";
	CString bStopVisualization = L"Остановить процедуру вычисления";

	// Области рисования.
	CWnd* PicWndImage;
	CDC* PicDcImage;
	CRect PicImage;

	COLORREF BUTTONS_COLOR = RGB(255, 255, 255);

	double xpImage = 0, ypImage = 0,			//коэфициенты пересчета
		xminImage = -1, xmaxImage = 1,			//максисимальное и минимальное значение х 
		yminImage = -0.5, ymaxImage = 5;			//максисимальное и минимальное значение y
public:
	// Критическая температура, Ecm/k.
	double T_CRITICAL = 2.2554;
	// Постоянная Больцмана, Дж/К
	double K = 1.380649e-23;
	// Порог МКШ, с которого начинается расчет энергии.
	int THRESHOLD_MKSH = 500;

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
	vector<vector<vector<int>>> GenerateConfiguration(int size);
	void DrawImage(vector<vector<vector<int>>> vec, CDC* WinDc, CRect WinxmaxGraphc);
	vector<int> BorderConditions(int size, int rand_idx);
	double CalculateHamiltonian(vector<vector<vector<int>>> last_cfg, vector<vector<vector<int>>> new_cfg);
	void MonteCarloStep(vector<vector<vector<int>>> &configuration, int step_count);
	void MonteCarlo();
	void CalculateGraphs();
	afx_msg void OnBnClickedOpenGraphDialog();
};
