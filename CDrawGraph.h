#pragma once
#include "afxdialogex.h"


// Диалоговое окно CDrawGraph

class CDrawGraph : public CDialog
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
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCalcEnergy();
	afx_msg void OnBnClickedClearEnergy();
	afx_msg void OnBnClickedCalcCapacity();
	afx_msg void OnBnClickedClearCapacity();
};
