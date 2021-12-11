// CDrawGraph.cpp: файл реализации
//

#include "pch.h"
#include "IzingModel.h"
#include "afxdialogex.h"
#include "CDrawGraph.h"
#include "framework.h"
#include "IzingModelDlg.h"
#include <fstream>
#include <random>
#include <iterator>
#include <math.h>
#include <iostream>


// Диалоговое окно CDrawGraph
#define DOTSGRAPHENERGYTEMP(x,y) (xpGraphEnergy*((x)-xminGraphEnergy)),(ypGraphEnergy*((y)-ymaxGraphEnergy))
#define DOTSGRAPHCAPACITYTEMP(x,y) (xpGraphСapacity*((x)-xminGraphСapacity)),(ypGraphСapacity*((y)-ymaxGraphСapacity))

IMPLEMENT_DYNAMIC(CDrawGraph, CDialog)

CDrawGraph::CDrawGraph(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRAW_GRAPH, pParent)
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
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BOOL CDrawGraph::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	//для  картинки
	PicWndGraphEnergy = GetDlgItem(IDC_DRAW_ENERGY_TEMP);
	PicDcGraphEnergy = PicWndGraphEnergy->GetDC();
	PicWndGraphEnergy->GetClientRect(&PicGraphEnergy);

	PicWndGraphСapacity = GetDlgItem(IDC_DRAW_CAPACITY_TEMP);
	PicDcGraphСapacity = PicWndGraphСapacity->GetDC();
	PicWndGraphСapacity->GetClientRect(&PicGraphСapacity);

	this->SetBackgroundColor(RGB(147, 112, 219));

	// TODO:  Добавить дополнительную инициализацию

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}

void CDrawGraph::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
		// TODO: добавьте свой код обработчика сообщений
		// Не вызывать CDialog::OnPaint() для сообщений рисования
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CDrawGraph::OnQueryDragIcon()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	return CDialogEx::OnQueryDragIcon();
}

void CDrawGraph::DrawEnergy(vector<double> x, vector<double> y, CDC* WinDc, CRect WinPic)
{
	//ГРАФИК СИГНАЛА

	//область построения
	xminGraphEnergy = - 5;			//минимальное значение х
	xmaxGraphEnergy =  10;			//максимальное значение х
	yminGraphEnergy =  -5;			//минимальное значение y
	ymaxGraphEnergy =  10;		//максимальное значение y

	// создание контекста устройства
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);

	double widthX = WinPic.Width() * scale;
	double heightY = WinPic.Height() * scale;
	xpGraphEnergy = (widthX / (xmaxGraphEnergy - xminGraphEnergy));			//Коэффициенты пересчёта координат по Х
	ypGraphEnergy = -(heightY / (ymaxGraphEnergy - yminGraphEnergy));			//Коэффициенты пересчёта координат по У

	bmp.CreateCompatibleBitmap(WinDc, widthX, heightY);
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);

	// заливка фона графика белым цветом
	MemDc->FillSolidRect(WinPic, RGB(0, 0, 0));

	CPen setka_pen;
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(71, 74, 81));			//цвет  grey

	CPen osi_pen;
	osi_pen.CreatePen(		//для сетки
		PS_SOLID,				//сплошная линия
		3,						//толщина 3 пикселя
		RGB(255, 255, 255));			//цвет white

	MemDc->SelectObject(&osi_pen);

	//создаём Ось Y
	MemDc->MoveTo(DOTSGRAPHENERGYTEMP(0, ymaxGraphEnergy));
	MemDc->LineTo(DOTSGRAPHENERGYTEMP(0, yminGraphEnergy));
	//создаём Ось Х
	MemDc->MoveTo(DOTSGRAPHENERGYTEMP(xminGraphEnergy, 0));
	MemDc->LineTo(DOTSGRAPHENERGYTEMP(xmaxGraphEnergy, 0));

	MemDc->SelectObject(&setka_pen);

	//отрисовка сетки по y
	for (double x = xminGraphEnergy; x <= xmaxGraphEnergy; x += xmaxGraphEnergy / scale / 8)
	{
		if (x != 0)
		{
			MemDc->MoveTo(DOTSGRAPHENERGYTEMP(x, ymaxGraphEnergy));
			MemDc->LineTo(DOTSGRAPHENERGYTEMP(x, yminGraphEnergy));
		}
	}
	//отрисовка сетки по x
	for (double y = yminGraphEnergy; y <= ymaxGraphEnergy; y += ymaxGraphEnergy / scale / 4)
	{
		if (y != 0)
		{
			MemDc->MoveTo(DOTSGRAPHENERGYTEMP(xminGraphEnergy, y));
			MemDc->LineTo(DOTSGRAPHENERGYTEMP(xmaxGraphEnergy, y));
		}
	}

	// установка прозрачного фона текста
	MemDc->SetBkMode(TRANSPARENT);
	MemDc->SetTextColor(RGB(255, 255, 255));

	// установка шрифта
	CFont font;
	font.CreateFontW(16, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Century Gothic"));
	MemDc->SelectObject(&font);

	//подпись осей
	MemDc->TextOutW(DOTSGRAPHENERGYTEMP(5, ymaxGraphEnergy - 0.03), _T("φ"));	//Y
	MemDc->TextOutW(DOTSGRAPHENERGYTEMP(xmaxGraphEnergy - 15, 0.1), _T("х"));		//X

	//по Y с шагом 5
	for (double i = 0; i <= ymaxGraphEnergy; i += ymaxGraphEnergy / scale / 8)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.2f"), i / scale + defaultY0 / scale);
			MemDc->TextOutW(DOTSGRAPHENERGYTEMP(defaultX0 + xminGraphEnergy / 2, i + 0.03 * ymaxGraphEnergy), str);
		}
	}
	//по X с шагом 0.5
	for (double j = xminGraphEnergy; j <= xmaxGraphEnergy; j += xmaxGraphEnergy / scale / 8)
	{
		double o = 0.0 / scale;
		CString str;
		if (j != o)
		{
			str.Format(_T("%.0f"), j / scale - defaultX0 / scale);
			MemDc->TextOutW(DOTSGRAPHENERGYTEMP(j - xmaxGraphEnergy / 100, -defaultY0 - 0.02), str);
		}
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

void CDrawGraph::DrawСapacity(vector<double> x, vector<double> y, CDC* WinDc, CRect WinPic)
{
	//ГРАФИК СИГНАЛА

	//область построения
	xminGraphСapacity = - 5;			//минимальное значение х
	xmaxGraphСapacity =  10;			//максимальное значение х
	yminGraphСapacity = -5;			//минимальное значение y
	ymaxGraphСapacity = 10;		//максимальное значение y

	// создание контекста устройства
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);

	double widthX = WinPic.Width() * scale;
	double heightY = WinPic.Height() * scale;
	xpGraphСapacity = (widthX / (xmaxGraphСapacity - xminGraphСapacity));			//Коэффициенты пересчёта координат по Х
	ypGraphСapacity = -(heightY / (ymaxGraphСapacity - yminGraphСapacity));			//Коэффициенты пересчёта координат по У

	bmp.CreateCompatibleBitmap(WinDc, widthX, heightY);
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);

	// заливка фона графика белым цветом
	MemDc->FillSolidRect(WinPic, RGB(0, 0, 0));

	CPen setka_pen;
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(71, 74, 81));			//цвет  grey

	CPen osi_pen;
	osi_pen.CreatePen(		//для сетки
		PS_SOLID,				//сплошная линия
		3,						//толщина 3 пикселя
		RGB(255, 255, 255));			//цвет white

	MemDc->SelectObject(&osi_pen);

	//создаём Ось Y
	MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(defaultX0, ymaxGraphСapacity));
	MemDc->LineTo(DOTSGRAPHCAPACITYTEMP(defaultX0, yminGraphСapacity));
	//создаём Ось Х
	MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(xminGraphСapacity, -defaultY0));
	MemDc->LineTo(DOTSGRAPHCAPACITYTEMP(xmaxGraphСapacity, -defaultY0));

	MemDc->SelectObject(&setka_pen);

	//отрисовка сетки по y
	for (double x = xminGraphСapacity; x <= xmaxGraphСapacity; x += xmaxGraphСapacity / scale / 8)
	{
		if (x != 0)
		{
			MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(x, ymaxGraphСapacity));
			MemDc->LineTo(DOTSGRAPHCAPACITYTEMP(x, yminGraphСapacity));
		}
	}
	//отрисовка сетки по x
	for (double y = yminGraphСapacity; y <= ymaxGraphСapacity; y += ymaxGraphСapacity / scale / 4)
	{
		if (y != 0)
		{
			MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(xminGraphСapacity, y));
			MemDc->LineTo(DOTSGRAPHCAPACITYTEMP(xmaxGraphСapacity, y));
		}
	}

	// установка прозрачного фона текста
	MemDc->SetBkMode(TRANSPARENT);
	MemDc->SetTextColor(RGB(255, 255, 255));

	// установка шрифта
	CFont font;
	font.CreateFontW(16, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Century Gothic"));
	MemDc->SelectObject(&font);

	//подпись осей
	MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(5, ymaxGraphСapacity - 0.03), _T("φ"));	//Y
	MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(xmaxGraphСapacity - 15, 0.1), _T("х"));		//X

	//по Y с шагом 5
	for (double i = 0; i <= ymaxGraphСapacity; i += ymaxGraphСapacity / scale / 8)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.2f"), i / scale + defaultY0 / scale);
			MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(defaultX0 + xminGraphСapacity / 2, i + 0.03 * ymaxGraphСapacity), str);
		}
	}
	//по X с шагом 0.5
	for (double j = xminGraphСapacity; j <= xmaxGraphСapacity; j += xmaxGraphСapacity / scale / 8)
	{
		double o = 0.0 / scale;
		CString str;
		if (j != o)
		{
			str.Format(_T("%.0f"), j / scale - defaultX0 / scale);
			MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(j - xmaxGraphСapacity / 100, -defaultY0 - 0.02), str);
		}
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

// Обработчики сообщений CDrawGraph
void CDrawGraph::OnBnClickedCalcEnergy()
{
	vector<double> y;
	vector<double> x;
	DrawEnergy(x, y, PicDcGraphEnergy, PicGraphEnergy);
	// TODO: добавьте свой код обработчика уведомлений
}

void CDrawGraph::OnBnClickedClearEnergy()
{
	// TODO: добавьте свой код обработчика уведомлений
}

void CDrawGraph::OnBnClickedCalcCapacity()
{
	vector<double> y;
	vector<double> x;
	DrawEnergy(x, y, PicDcGraphСapacity, PicGraphСapacity);
	// TODO: добавьте свой код обработчика уведомлений
}

void CDrawGraph::OnBnClickedClearCapacity()
{
	// TODO: добавьте свой код обработчика уведомлений
}






