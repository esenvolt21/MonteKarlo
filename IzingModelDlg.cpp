
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

	CPen white_circle;
	white_circle.CreatePen(		//для сетки
		PS_SOLID,				//сплошная линия
		10,						//толщина 3 пикселя
		RGB(255, 255, 255));			//цвет white

	CPen black_circle;
	black_circle.CreatePen(		//для сетки
		PS_SOLID,				//сплошная линия
		10,						//толщина 3 пикселя
		RGB(0, 0, 0));			//цвет white

	MemDc->SelectObject(&setka_pen);
	//отрисовка сетки по y
	for (double x = 0; x < vec.size(); x++)
	{
		MemDc->MoveTo(DOTSIMAGE(x, value_size - 1));
		MemDc->LineTo(DOTSIMAGE(x, 0));
	}
	//отрисовка сетки по x
	for (double y = 0; y < vec.size(); y++)
	{
		MemDc->MoveTo(DOTSIMAGE(0, y));
		MemDc->LineTo(DOTSIMAGE(value_size - 1, y));
	}

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
					MemDc->SelectObject(&white_circle);
					MemDc->Ellipse(DOTSIMAGE(xxi - 0.2, yyi - 0.2), DOTSIMAGE(xxi + 0.2, yyi + 0.2));
				}
				else if (color == 1)
				{
					MemDc->SelectObject(&black_circle);
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					MemDc->Ellipse(DOTSIMAGE(xxi - 0.2, yyi - 0.2), DOTSIMAGE(xxi + 0.2, yyi + 0.2));
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

				if (color < 0)
				{
					MemDc->SelectObject(&white_circle);
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					MemDc->Ellipse(DOTSIMAGE(xxi - 0.2, yyi - 0.2), DOTSIMAGE(xxi + 0.2, yyi + 0.2));
				}
				else if (color > 0)
				{
					MemDc->SelectObject(&black_circle);
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					MemDc->Ellipse(DOTSIMAGE(xxi - 0.2, yyi - 0.2), DOTSIMAGE(xxi + 0.2, yyi + 0.2));
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
					MemDc->SelectObject(&white_circle);
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					MemDc->Ellipse(DOTSIMAGE(xxi - 0.2, yyi - 0.2), DOTSIMAGE(xxi + 0.2, yyi + 0.2));
				}
				else if (color == 1)
				{
					MemDc->SelectObject(&black_circle);
					MemDc->MoveTo(DOTSIMAGE(xxi, yyi));
					MemDc->Ellipse(DOTSIMAGE(xxi - 0.2, yyi - 0.2), DOTSIMAGE(xxi + 0.2, yyi + 0.2));
				}
			}
		}
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

int CIzingModelDlg::RandStaff(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	// Равномерно распределяем рандомное число в нашем диапазоне
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

void CIzingModelDlg::OnBnClickedCalculate()
{
	UpdateData(TRUE);

	vector<int> vecHelp1(value_size, -1);
	vector<vector<int>> vecHelp2(value_size, vecHelp1);
	vector<vector<vector<int>>> IzingModel(value_size, vecHelp2);

	int size = (value_size * value_size * value_size) / 2;
	int c = 0;
	while (c < size)
	{
		int i = RandStaff(0, value_size - 1);
		int j = RandStaff(0, value_size - 1);
		int k = RandStaff(0, value_size - 1);

		if (IzingModel[i][j][k] == -1)
		{
			IzingModel[i][j][k] = 1;
			c++;
		}
	}

	ofstream out("ПомощникВектор.txt");
	for (int k = 0; k < IzingModel[0][0].size(); k++)
	{
		for (int i = 0; i < IzingModel.size(); i++)
		{
			for (int j = 0; j < IzingModel[i].size(); j++)
			{
				out << IzingModel[i][j][k];
			}
			out << endl;
		}
		out << endl << endl;
	}

	out.close();

	/*int sum1 = 0;
	int sum2 = 0;
	for (int i = 0; i < IzingModel.size(); i++)
	{
		for (int j = 0; j < IzingModel[i].size(); j++)
		{
			for (int k = 0; k < IzingModel[i][j].size(); k++)
			{
				if (IzingModel[i][j][k] == -1)
				{
					sum1 += IzingModel[i][j][k];
				}
				else if (IzingModel[i][j][k] == 1)
				{
					sum2 += IzingModel[i][j][k];
				}
			}
		}
	}*/

	vecIzingModel = IzingModel;

	vecHelp1.clear();
	vecHelp2.clear();
	IzingModel.clear();
}


void CIzingModelDlg::OnBnClickedPicture()
{
	DrawImage(vecIzingModel, PicDcImage, PicImage);
}


void CIzingModelDlg::OnBnClickedDropping()
{
	// TODO: добавьте свой код обработчика уведомлений
}
