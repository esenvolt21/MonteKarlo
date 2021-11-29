
// IzingModelDlg.h: файл заголовка
//

#pragma once
#include <vector>
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

	//области рисования
	CWnd* PicWndImage;
	CDC* PicDcImage;
	CRect PicImage;

	double xpImage = 0, ypImage = 0,			//коэфициенты пересчета
		xminImage = -1, xmaxImage = 1,			//максисимальное и минимальное значение х 
		yminImage = -0.5, ymaxImage = 5;			//максисимальное и минимальное значение y

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

	vector<vector<vector<int>>> vecIzingModel;

	int RandStaff(int min, int max);
	void DrawImage(vector<vector<vector<int>>> vec, CDC* WinDc, CRect WinxmaxGraphc);
};
