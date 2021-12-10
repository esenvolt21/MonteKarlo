// CDrawGraph.cpp: файл реализации
//

#include "pch.h"
#include "IzingModel.h"
#include "afxdialogex.h"
#include "CDrawGraph.h"


// Диалоговое окно CDrawGraph

IMPLEMENT_DYNAMIC(CDrawGraph, CDialog)

CDrawGraph::CDrawGraph(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DRAW_GRAPH, pParent)
{

}

CDrawGraph::~CDrawGraph()
{
}

void CDrawGraph::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDrawGraph, CDialog)
	ON_BN_CLICKED(IDC_CALC_ENERGY, &CDrawGraph::OnBnClickedCalcEnergy)
	ON_BN_CLICKED(IDC_CLEAR_ENERGY, &CDrawGraph::OnBnClickedClearEnergy)
	ON_BN_CLICKED(IDC_CALC_CAPACITY, &CDrawGraph::OnBnClickedCalcCapacity)
	ON_BN_CLICKED(IDC_CLEAR_CAPACITY, &CDrawGraph::OnBnClickedClearCapacity)
END_MESSAGE_MAP()


// Обработчики сообщений CDrawGraph


void CDrawGraph::OnBnClickedCalcEnergy()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CDrawGraph::OnBnClickedClearEnergy()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CDrawGraph::OnBnClickedCalcCapacity()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CDrawGraph::OnBnClickedClearCapacity()
{
	// TODO: добавьте свой код обработчика уведомлений
}
