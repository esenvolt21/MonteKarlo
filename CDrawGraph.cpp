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
#include <algorithm>s


// Диалоговое окно CDrawGraph
#define DOTSGRAPHENERGYTEMP(x,y) (xpGraphEnergy*((x)-xminGraphEnergy)),(ypGraphEnergy*((y)-ymaxGraphEnergy))
#define DOTSGRAPHCAPACITYTEMP(x,y) (xpGraphСapacity*((x)-xminGraphСapacity)),(ypGraphСapacity*((y)-ymaxGraphСapacity))

IMPLEMENT_DYNAMIC(CDrawGraph, CDialog)

CDrawGraph::CDrawGraph(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRAW_GRAPH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDrawGraph::~CDrawGraph()
{
}

void CDrawGraph::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALC_BUTTON, button_calculate);
	DDX_Control(pDX, IDC_CLEAR_BUTTON, button_clear);
	DDX_Control(pDX, IDC_CURRENT_STEP, CURRENT_STEP);
	DDX_Control(pDX, IDC_CURRENT_TEMP, CURRENT_TEMPERATURE);
}

BEGIN_MESSAGE_MAP(CDrawGraph, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CALC_BUTTON, &CDrawGraph::OnBnClickedCalcButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CDrawGraph::OnBnClickedClearButton)
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

	PicWndGraphCapacity = GetDlgItem(IDC_DRAW_CAPACITY_TEMP);
	PicDcGraphCapacity = PicWndGraphCapacity->GetDC();
	PicWndGraphCapacity->GetClientRect(&PicGraphCapacity);

	button_calculate.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;//required for flatering and use bg color
	button_calculate.m_bTransparent = false;//reg for use bg color
	button_calculate.SetFaceColor(BUTTONS_COLOR, true);

	button_clear.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;//required for flatering and use bg color
	button_clear.m_bTransparent = false;//reg for use bg color
	button_clear.SetFaceColor(BUTTONS_COLOR, true);

	hThreadGraphs = NULL;

	// TODO:  Добавить дополнительную инициализацию

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}

void CDrawGraph::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CBrush brush;
	brush.CreateSolidBrush(RGB(153, 204, 255));
	dc.FillRect(&rect, &brush);
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

void CDrawGraph::DrawEnergy(vector<double> vec_x, vector<double> vec_y, CDC* WinDc, CRect WinPic)
{
	//ГРАФИК СИГНАЛА
	x_max = 1.0;
	y_max1 = 1.0;
	x_min = 0.0;
	y_min1 = 0.0;
	if (!vec_x.empty() && !vec_y.empty()) {
		x_max = *max_element(vec_x.begin(), vec_x.end());
		y_max1 = *max_element(vec_y.begin(), vec_y.end());
		x_min = *min_element(vec_x.begin(), vec_x.end());
		y_min1 = *min_element(vec_y.begin(), vec_y.end());
	}

	//область построения
	xminGraphEnergy = x_min-x_max * 0.1;			//минимальное значение х
	xmaxGraphEnergy = x_max * 1.1;			//максимальное значение х
	yminGraphEnergy = y_min1-y_max1 * 0.1;			//минимальное значение y
	ymaxGraphEnergy = y_max1 * 1.1;		//максимальное значение y

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
	MemDc->FillSolidRect(WinPic, RGB(255, 255, 255));

	CPen setka_pen;
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(71, 74, 81));			//цвет  grey

	CPen osi_pen;
	osi_pen.CreatePen(		//для сетки
		PS_SOLID,				//сплошная линия
		3,						//толщина 3 пикселя
		RGB(0, 0, 0));			//цвет white

	CPen ellips;
	ellips.CreatePen(
		PS_SOLID,				//сплошная линия
		5,						//толщина 2 пикселя
		RGB(0, 0, 255));			//цвет blue

	MemDc->SelectObject(&osi_pen);

	//создаём Ось Y
	MemDc->MoveTo(DOTSGRAPHENERGYTEMP(0, ymaxGraphEnergy));
	MemDc->LineTo(DOTSGRAPHENERGYTEMP(0, yminGraphEnergy));
	//создаём Ось Х
	MemDc->MoveTo(DOTSGRAPHENERGYTEMP(xminGraphEnergy, 0));
	MemDc->LineTo(DOTSGRAPHENERGYTEMP(xmaxGraphEnergy, 0));

	MemDc->SelectObject(&setka_pen);

	//отрисовка сетки по y
	for (double x = xminGraphEnergy; x <= xmaxGraphEnergy; x += xmaxGraphEnergy / scale / 4)
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
	MemDc->SetTextColor(RGB(0, 0, 0));

	// установка шрифта
	CFont font;
	font.CreateFontW(10, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Century Gothic"));
	MemDc->SelectObject(&font);

	//подпись осей
	MemDc->TextOutW(DOTSGRAPHENERGYTEMP(5, ymaxGraphEnergy - 0.03), _T("φ"));	//Y
	MemDc->TextOutW(DOTSGRAPHENERGYTEMP(xmaxGraphEnergy - 15, 0.1), _T("х"));		//X

	//по Y с шагом 5
	for (double i = 0; i <= ymaxGraphEnergy; i += ymaxGraphEnergy / scale / 4)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.1f"), i / scale + defaultY0 / scale);
			MemDc->TextOutW(DOTSGRAPHENERGYTEMP(defaultX0 + xminGraphEnergy / 2, i + 0.001 * ymaxGraphEnergy), str);
		}
	}
	//по X с шагом 0.5
	for (double j = xminGraphEnergy; j <= xmaxGraphEnergy; j += xmaxGraphEnergy / scale / 4)
	{
		CString str;
		if (j != 0)
		{
			str.Format(_T("%.1f"), j / scale - defaultX0 / scale);
			MemDc->TextOutW(DOTSGRAPHENERGYTEMP(j, -defaultY0 - 0.02), str);
		}
	}

	if (vec_x.size() == vec_y.size() && !vec_y.empty())
	{
		// отрисовка
		MemDc->MoveTo(DOTSGRAPHENERGYTEMP(vec_x[0], vec_y[0]));
		for (int i = 0; i < vec_x.size(); i++)
		{
			MemDc->SelectObject(&ellips);
			MemDc->Ellipse(DOTSGRAPHENERGYTEMP(vec_x[i] - 0.005, vec_y[i] - 0.001),
				DOTSGRAPHENERGYTEMP(vec_x[i] + 0.005, vec_y[i] + 0.001));
		}
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

void CDrawGraph::DrawCapacity(vector<double> vec_x, vector<double> vec_y, CDC* WinDc, CRect WinPic)
{
	//ГРАФИК СИГНАЛА
	x_max = 1.0;
	y_max2 = 1.0;
	x_min = 0.0;
	y_min2 = 0.0;
	if (!vec_x.empty() && !vec_y.empty()) {
		x_max = *max_element(vec_x.begin(), vec_x.end());
		y_max2 = *max_element(vec_y.begin(), vec_y.end());
		x_min = *min_element(vec_x.begin(), vec_x.end());
		y_min2 = *min_element(vec_y.begin(), vec_y.end());
	}

	//область построения
	xminGraphСapacity = x_min - x_max * 0.1;			//минимальное значение х
	xmaxGraphСapacity = x_max * 1.1;			//максимальное значение х
	yminGraphСapacity = y_min2 - y_max2 * 0.1;			//минимальное значение y
	ymaxGraphСapacity = y_max2 * 1.1;		//максимальное значение y

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
	MemDc->FillSolidRect(WinPic, RGB(255, 255, 255));

	CPen setka_pen;
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(71, 74, 81));			//цвет  grey

	CPen osi_pen;
	osi_pen.CreatePen(		//для сетки
		PS_SOLID,				//сплошная линия
		3,						//толщина 3 пикселя
		RGB(0, 0, 0));			//цвет white

	CPen ellips;
	ellips.CreatePen(
		PS_SOLID,				//сплошная линия
		5,						//толщина 2 пикселя
		RGB(255, 255, 0));			//цвет blue

	MemDc->SelectObject(&osi_pen);

	//создаём Ось Y
	MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(defaultX0, ymaxGraphСapacity));
	MemDc->LineTo(DOTSGRAPHCAPACITYTEMP(defaultX0, yminGraphСapacity));
	//создаём Ось Х
	MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(xminGraphСapacity, -defaultY0));
	MemDc->LineTo(DOTSGRAPHCAPACITYTEMP(xmaxGraphСapacity, -defaultY0));

	MemDc->SelectObject(&setka_pen);

	//отрисовка сетки по y
	for (double x = xminGraphСapacity; x <= xmaxGraphСapacity; x += xmaxGraphСapacity / scale / 4)
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
	MemDc->SetTextColor(RGB(0, 0, 0));

	// установка шрифта
	CFont font;
	font.CreateFontW(10, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Century Gothic"));
	MemDc->SelectObject(&font);

	//подпись осей
	MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(5, ymaxGraphСapacity - 0.03), _T("φ"));	//Y
	MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(xmaxGraphСapacity - 15, 0.1), _T("х"));		//X

	//по Y с шагом 5
	for (double i = 0; i <= ymaxGraphСapacity; i += ymaxGraphСapacity / scale / 4)
	{
		CString str;
		if (i != 0)
		{
			str.Format(_T("%.1f"), i / scale + defaultY0 / scale);
			MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(defaultX0 + xminGraphСapacity / 2, i + 0.001 * ymaxGraphСapacity), str);
		}
	}
	//по X с шагом 0.5
	for (double j = xminGraphСapacity; j <= xmaxGraphСapacity; j += xmaxGraphСapacity / scale / 4)
	{
		CString str;
		if (j != 0)
		{
			str.Format(_T("%.1f"), j / scale - defaultX0 / scale);
			MemDc->TextOutW(DOTSGRAPHCAPACITYTEMP(j, -defaultY0 - 0.02), str);
		}
	}

	if (vec_x.size() == vec_y.size() && !vec_y.empty())
	{
		// отрисовка
		MemDc->MoveTo(DOTSGRAPHCAPACITYTEMP(vec_x[0], vec_y[0]));
		for (int i = 0; i < vec_x.size(); i++)
		{
			MemDc->SelectObject(&ellips);
			MemDc->Ellipse(DOTSGRAPHCAPACITYTEMP(vec_x[i] - 0.005, vec_y[i] - 0.005),
				DOTSGRAPHCAPACITYTEMP(vec_x[i] + 0.005, vec_y[i] + 0.005));
		}
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

void CDrawGraph::Mashtab(vector<double>& solve_buff, double* mmin, double* mmax)
{
	*mmin = *mmax = solve_buff[0];

	for (int i = 0; i < solve_buff.size(); i++)
	{
		if (*mmin > solve_buff[i]) *mmin = solve_buff[i];
		if (*mmax < solve_buff[i]) *mmax = solve_buff[i];
	}
}

void CDrawGraph::CalculateGraphs() {
	CIzingModelDlg* pParent = (CIzingModelDlg*)GetParent();

	// Параметры.
	double T_CRITICAL = 2.2554;
	// Постоянная Больцмана, Дж/К.
	double K = 1.380649e-23;

	double start_temp = 0.3 * T_CRITICAL;
	double stop_temp = 1.5 * T_CRITICAL;
	double dots_count = 25;
	int mksh_count = 50;
	double step = (stop_temp - start_temp) / dots_count;
	int size = 20;
	char current_step[100];
	char current_temp[100];
	int THRESHOLD_MKSH = 20;
	pParent->Ecm = 1;

	// Энергия.
	Energy enrg;
	vector<double> y_energy;
	vector<double> y_capacity;
	vector<double> x;

	for (double t = start_temp; t < stop_temp; t += step) {
		pParent->TEMPERATURE = t;

		vector<vector<vector<int>>> conf = pParent->GenerateConfiguration(size);

		for (int mksh = 0; mksh < mksh_count; mksh++) {
			// Вывод иинформации на экран.
			sprintf_s(current_step, "%d", mksh);
			CURRENT_STEP.SetWindowTextW((CString)current_step);
			sprintf_s(current_temp, "%.3f", t);
			CURRENT_TEMPERATURE.SetWindowTextW((CString)current_temp);

			pParent->MonteCarloStep(conf, size * size * size, false);

			if (mksh > THRESHOLD_MKSH) {
				for (int i = 0; i < conf.size(); i++) {
					for (int j = 0; j < conf[i].size(); j++) {
						for (int k = 0; k < conf[i][j].size(); k++) {
							vector<int> nei_i = pParent->BorderConditions(conf.size(), i);
							vector<int> nei_j = pParent->BorderConditions(conf[i].size(), j);
							vector<int> nei_k = pParent->BorderConditions(conf[i][j].size(), k);
							double sum = 0.0;
							for (int m = 0; m < nei_i.size(); m++) {
								sum += nei_i[m] + nei_j[m] + nei_k[m];
							}
							enrg.energy += conf[i][j][k] * sum;
							enrg.pow_energy += conf[i][j][k] * sum * conf[i][j][k] * sum;
						}
					}
				}
			}
		}

		// Усреднение.
		enrg.energy /= ((mksh_count - THRESHOLD_MKSH) * size * size * size);
		enrg.pow_energy /= ((mksh_count - THRESHOLD_MKSH) * size * size * size);

		// Теплоемкость.
		double capacity = (enrg.pow_energy - enrg.energy * enrg.energy) / (K * K * t * t * size * size * size);

		x.push_back(t);
		y_energy.push_back(enrg.energy);
		y_capacity.push_back(capacity);
		Mashtab(x, &x_min, &x_max);
		Mashtab(y_energy, &y_min1, &y_max1);
		Mashtab(y_capacity, &y_min2, &y_max2);
		DrawEnergy(x, y_energy, PicDcGraphEnergy, PicGraphEnergy);
		DrawCapacity(x, y_capacity, PicDcGraphCapacity, PicGraphCapacity);
	}

	MessageBox(L"Визуализация успешно завершена!", L"Информация", MB_ICONINFORMATION | MB_OK);
	bRunTh = false;
	button_calculate.SetWindowTextW(bStartVisualization);
	TerminateThread(hThreadGraphs, 0);
	CloseHandle(hThreadGraphs);
	hThreadGraphs = NULL;
}

DWORD WINAPI CalcGraph(PVOID pv)
{
	CDrawGraph* p = (CDrawGraph*)pv;
	p->CalculateGraphs();
	return 0;
}

void CDrawGraph::OnBnClickedCalcButton()
{
	UpdateData(TRUE);
	if (!bRunTh) {
		button_calculate.SetWindowTextW(bStopVisualization);
		if (hThreadGraphs == NULL) {
			hThreadGraphs = CreateThread(NULL, 0, CalcGraph, this, 0, &dwThreadGraphs);
		}
		else {
			ResumeThread(hThreadGraphs);
		}
		bRunTh = true;
	}
	else {
		button_calculate.SetWindowTextW(bStartVisualization);
		bRunTh = false;
		SuspendThread(hThreadGraphs);
	}
}


void CDrawGraph::OnBnClickedClearButton()
{
	// TODO: добавьте свой код обработчика уведомлений
	// Остановка потока.
	if (bRunTh) {
		bRunTh = false;
		SuspendThread(hThreadGraphs);
		button_calculate.SetWindowTextW(bStartVisualization);
	}

	// Завершение потока.
	TerminateThread(hThreadGraphs, 0);
	CloseHandle(hThreadGraphs);
	hThreadGraphs = NULL;

	CURRENT_STEP.SetWindowTextW(L"");
	CURRENT_TEMPERATURE.SetWindowTextW(L"");

	// Очистка глобального вектора.

	vector<double> x;
	vector<double> y;
	DrawEnergy(x, y, PicDcGraphEnergy, PicGraphEnergy);
	DrawCapacity(x, y, PicDcGraphCapacity, PicGraphCapacity);
}
