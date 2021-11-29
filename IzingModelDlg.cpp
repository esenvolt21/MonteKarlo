
// IzingModelDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "IzingModel.h"
#include "IzingModelDlg.h"
#include "afxdialogex.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Диалоговое окно CIzingModelDlg

#define DOTSIMAGE(x,y) (xpImage*((x)-xminImage)),(ypImage*((y)-ymaxImage)) 

CIzingModelDlg::CIzingModelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IZINGMODEL_DIALOG, pParent)
	, value_size(30)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIzingModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SIZE, value_size);
	DDX_Control(pDX, IDC_RADIO1, radio_XY);
	DDX_Control(pDX, IDC_RADIO2, radio_YZ);
	DDX_Control(pDX, IDC_RADIO3, radio_ZX);
}

BEGIN_MESSAGE_MAP(CIzingModelDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CALCULATE, &CIzingModelDlg::OnBnClickedCalculate)
	ON_BN_CLICKED(IDC_PICTURE, &CIzingModelDlg::OnBnClickedPicture)
	ON_BN_CLICKED(IDC_DROPPING, &CIzingModelDlg::OnBnClickedDropping)
	ON_BN_CLICKED(IDC_RADIO1, &CIzingModelDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CIzingModelDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CIzingModelDlg::OnBnClickedRadio3)
END_MESSAGE_MAP()


// Обработчики сообщений CIzingModelDlg

BOOL CIzingModelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	//для  картинки
	PicWndImage = GetDlgItem(IDC_IMAGE);
	PicDcImage = PicWndImage->GetDC();
	PicWndImage->GetClientRect(&PicImage);

	this->SetBackgroundColor(RGB(147, 112, 219));

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CIzingModelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

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
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CIzingModelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIzingModelDlg::DrawImage(vector<vector<vector<int>>> vec, CDC* WinDc, CRect WinxmaxGraphc)
{
	//ГРАФИК СИГНАЛА
	double GU_X = value_size;
	double GU_Y = value_size;

	xminImage = -GU_X * 0.04;
	xmaxImage = GU_X * 1.01;
	yminImage = -GU_Y * 0.04;			//минимальное значение y
	ymaxImage = GU_Y * 1.01;

	// создание контекста устройства
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);

	double widthX = WinxmaxGraphc.Width();
	double heightY = WinxmaxGraphc.Height();
	xpImage = (widthX / (xmaxImage - xminImage));			//Коэффициенты пересчёта координат по Х
	ypImage = -(heightY / (ymaxImage - yminImage));			//Коэффициенты пересчёта координат по У

	bmp.CreateCompatibleBitmap(WinDc, widthX, heightY);
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);

	// заливка фона графика
	MemDc->FillSolidRect(WinxmaxGraphc, RGB(147, 112, 219));

	CPen setka_pen;
	setka_pen.CreatePen(		//для сетки
		PS_SOLID,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(0, 0, 128));			//цвет  grey

	CBrush white_circle(RGB(255, 255, 255));			//цвет white

	CBrush black_circle(RGB(0, 0, 0));			//цвет white

	MemDc->SelectObject(&setka_pen);

	if (radio_XY.GetCheck())
	{
		for (int i = 0; i < vec.size(); i++)
		{
			for (int j = 0; j < vec[i].size(); j++)
			{
				double xxi = i;
				double yyi = j;
				int color = vec[i][j][0];

				if (color == -1)
				{
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					CRect rect(DOTSIMAGE(xxi, yyi + 1), DOTSIMAGE(xxi + 1, yyi));
					LPCRECT lpRect = rect;
					MemDc->FillRect(lpRect, &white_circle);
				}
				else if (color == 1)
				{
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					CRect rect(DOTSIMAGE(xxi, yyi + 1), DOTSIMAGE(xxi + 1, yyi));
					LPCRECT lpRect = rect;
					MemDc->FillRect(lpRect, &black_circle);
				}
			}
		}
	}
	else if (radio_YZ.GetCheck())
	{
		for (int j = 0; j < vec[0].size(); j++)
		{
			for (int k = 0; k < vec[0][j].size(); k++)
			{
				double xxi = j;
				double yyi = k;
				int color = vec[0][j][k];

				if (color == -1)
				{
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					CRect rect(DOTSIMAGE(xxi, yyi + 1), DOTSIMAGE(xxi + 1, yyi));
					LPCRECT lpRect = rect;
					MemDc->FillRect(lpRect, &white_circle);
				}
				else if (color == 1)
				{
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					CRect rect(DOTSIMAGE(xxi, yyi + 1), DOTSIMAGE(xxi + 1, yyi));
					LPCRECT lpRect = rect;
					MemDc->FillRect(lpRect, &black_circle);
				}
			}
		}
	}
	else if (radio_ZX.GetCheck())
	{
		for (int i = 0; i < vec.size(); i++)
		{
			for (int k = 0; k < vec[i][0].size(); k++)
			{
				double xxi = i;
				double yyi = k;
				int color = vec[i][0][k];

				if (color == -1)
				{
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					CRect rect(DOTSIMAGE(xxi, yyi + 1), DOTSIMAGE(xxi + 1, yyi));
					LPCRECT lpRect = rect;
					MemDc->FillRect(lpRect, &white_circle);
				}
				else if (color == 1)
				{
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					CRect rect(DOTSIMAGE(xxi, yyi + 1), DOTSIMAGE(xxi + 1, yyi));
					LPCRECT lpRect = rect;
					MemDc->FillRect(lpRect, &black_circle);
				}
			}
		}
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

//Функция выдающая рандомное число в диапазоне от мин до макс
int CIzingModelDlg::RandStaff(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	// Равномерно распределяем рандомное число в нашем диапазоне
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

void CIzingModelDlg::OnBnClickedCalculate()
{
	UpdateData(TRUE);

	// Инициализация 3-х мерного вектора значением -1 (сорт А).
	vector<int> vecHelp1(value_size, -1);
	vector<vector<int>> vecHelp2(value_size, vecHelp1);
	vector<vector<vector<int>>> IzingModel(value_size, vecHelp2);

	// Расчет количества элементов сорта Б.
	int size = (value_size * value_size * value_size) / 2;
	int c = 0;
	while (c < size)
	{
		// Рандомно выбираем место в векторе.
		int i = RandStaff(0, value_size - 1);
		int j = RandStaff(0, value_size - 1);
		int k = RandStaff(0, value_size - 1);

		// Меняем сорт А на сорт Б, пока кол-во А != колву Б.
		if (IzingModel[i][j][k] == -1)
		{
			IzingModel[i][j][k] = 1;
			c++;
		}
	}

	// Заполнение глобального вектора.
	vecIzingModel = IzingModel;

	// Очистка локальных векторов.
	vecHelp1.clear();
	vecHelp2.clear();
	IzingModel.clear();
}

void CIzingModelDlg::OnBnClickedPicture()
{
	//Отрисовка начальной конфигурации.
	DrawImage(vecIzingModel, PicDcImage, PicImage);
}

void CIzingModelDlg::OnBnClickedDropping()
{
	// Очистка глобального вектора.
	vecIzingModel.clear();
	// Перерисовка.
	OnBnClickedPicture();
}

void CIzingModelDlg::OnBnClickedRadio1()
{
	if (!vecIzingModel.empty())
	{
		DrawImage(vecIzingModel, PicDcImage, PicImage);
	}
}

void CIzingModelDlg::OnBnClickedRadio2()
{
	if (!vecIzingModel.empty())
	{
		DrawImage(vecIzingModel, PicDcImage, PicImage);
	}
}

void CIzingModelDlg::OnBnClickedRadio3()
{
	if (!vecIzingModel.empty())
	{
		DrawImage(vecIzingModel, PicDcImage, PicImage);
	}
}