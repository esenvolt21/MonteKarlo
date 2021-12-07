#include "pch.h"
#include "framework.h"
#include "IzingModel.h"
#include "IzingModelDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <random>
#include <iterator>
#include <math.h>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Диалоговое окно CIzingModelDlg

#define DOTSIMAGE(x,y) (xpImage*((x)-xminImage)),(ypImage*((y)-ymaxImage)) 

CIzingModelDlg::CIzingModelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IZINGMODEL_DIALOG, pParent)
	, value_size(20)
	, Ecm(-1)
	, TEMPERATURE(0.5*T_CRITICAL)
	, MKSH_QOUNT(510)
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
	DDX_Control(pDX, IDC_CALCULATE, button_calculate);
	DDX_Control(pDX, IDC_PICTURE, button_picture);
	DDX_Control(pDX, IDC_DROPPING, button_dropping);
	DDX_Text(pDX, IDC_MIXING_ENERGY, Ecm);
	DDX_Text(pDX, IDC_TEMPERATURE_EDIT, TEMPERATURE);
	DDX_Text(pDX, IDC_MKSH_COUNT, MKSH_QOUNT);
	DDX_Control(pDX, IDC_CURRENT_MKSH_STEP, CURRENT_MKSH_STEP);
	DDX_Control(pDX, IDC_WITH_GRAPH, radio_with_graph);
	DDX_Control(pDX, IDC_WITHOUT_GRAPH, radio_without_graph);
	DDX_Control(pDX, IDC_CHECK1, check_GU);
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

	srand((unsigned int)time(0));

	//для  картинки
	PicWndImage = GetDlgItem(IDC_IMAGE);
	PicDcImage = PicWndImage->GetDC();
	PicWndImage->GetClientRect(&PicImage);

	this->SetBackgroundColor(RGB(147, 112, 219));

	button_calculate.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;//required for flatering and use bg color
	button_calculate.m_bTransparent = false;//reg for use bg color
	button_calculate.SetFaceColor(RGB(0, 255, 255), true);

	button_picture.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;//required for flatering and use bg color
	button_picture.m_bTransparent = false;//reg for use bg color
	button_picture.SetFaceColor(RGB(0, 255, 255), true);

	button_dropping.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;//required for flatering and use bg color
	button_dropping.m_bTransparent = false;//reg for use bg color
	button_dropping.SetFaceColor(RGB(0, 255, 255), true);

	// Значения чекпоинтов по умолчанию.
	radio_YZ.SetCheck(true);
	radio_with_graph.SetCheck(true);

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

DWORD WINAPI MyProc(PVOID pv)
{
	CIzingModelDlg* p = (CIzingModelDlg*)pv;
	p->MonteCarlo();
	return 0;
}

void CIzingModelDlg::DrawImage(vector<vector<vector<int>>> vec, CDC* WinDc, CRect WinxmaxGraphc)
{
	//ГРАФИК СИГНАЛА
	if (check_GU.GetCheck())
	{
		double GU_X = value_size + 2;
		double GU_Y = value_size + 2;

		xminImage = -GU_X * 0.01;
		xmaxImage = GU_X * 1.01;
		yminImage = -GU_Y * 0.01;			//минимальное значение y
		ymaxImage = GU_Y * 1.01;
	}
	else
	{
		double GU_X = value_size;
		double GU_Y = value_size;

		xminImage = -GU_X * 0.01;
		xmaxImage = GU_X * 1.01;
		yminImage = -GU_Y * 0.01;			//минимальное значение y
		ymaxImage = GU_Y * 1.01;
	}

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

	CPen line_pen;
	line_pen.CreatePen(		//для сетки
		PS_SOLID,					//пунктирная
		3,						//толщина 1 пиксель
		RGB(255, 0, 0));			//цвет  grey

	CBrush white_circle(RGB(255, 255, 255));			//цвет white
	CBrush black_circle(RGB(0, 0, 0));			//цвет black

	if (!vec.empty()) {
		vector<int> vecHelpMinLeft;
		vector<int> vecHelpMaxRight;
		vector<int> vecHelpMinTop;
		vector<int> vecHelpMaxBotomm;

		vector<vector<vector<int>>> IzingModel;
		int center = vec.size() / 2;
		// СЕЛЕДКА
		if (check_GU.GetCheck())
		{
			vector<int> vecHelp1(value_size + 2, 0);
			vector<vector<int>> vecHelp2(value_size + 2, vecHelp1);
			vector<vector<vector<int>>> Model(value_size + 2, vecHelp2);
			IzingModel = Model;

			if (radio_XY.GetCheck())
			{
				for (int i = 0; i < vec.size(); i++)
				{
					IzingModel[i + 1][0][center] = vec[i][vec.size() - 1][center];
					IzingModel[i + 1][IzingModel.size() - 1][center] = vec[i][0][center];

					for (int j = 0; j < vec[i].size(); j++)
					{
						IzingModel[0][j + 1][center] = vec[vec.size() - 1][j][center];
						IzingModel[IzingModel.size() - 1][j + 1][center] = vec[0][j][center];

						IzingModel[i + 1][j + 1][center] = vec[i][j][center];

					}
				}
			}
			else if (radio_YZ.GetCheck())
			{
				for (int j = 0; j < vec[0].size(); j++)
				{
					IzingModel[center][j + 1][0] = vec[center][j][vec.size() - 1];
					IzingModel[center][j + 1][IzingModel.size() - 1] = vec[center][j][0];

					for (int k = 0; k < vec[0][j].size(); k++)
					{
						IzingModel[center][0][k + 1] = vec[center][vec.size() - 1][k];
						IzingModel[center][IzingModel.size() - 1][k + 1] = vec[center][0][k];

						IzingModel[center][j + 1][k + 1] = vec[center][j][k];
					}
				}
			}
			else if (radio_ZX.GetCheck())
			{
				for (int i = 0; i < vec.size(); i++)
				{
					IzingModel[i + 1][center][0] = vec[i][center][vec.size() - 1];
					IzingModel[i + 1][center][IzingModel[i][0].size() - 1] = vec[i][center][0];

					for (int k = 0; k < vec[i][0].size(); k++)
					{
						IzingModel[0][center][k + 1] = vec[vec.size() - 1][center][k];
						IzingModel[IzingModel.size() - 1][center][k + 1] = vec[0][center][k];

						IzingModel[i + 1][center][k + 1] = vec[i][center][k];
					}
				}
			}
		}
		else
		{
			IzingModel = vec;
		}

		// РЫБА
		if (radio_XY.GetCheck())
		{
			for (int i = 0; i < IzingModel.size(); i++)
			{
				for (int j = 0; j < IzingModel[i].size(); j++)
				{
					double xxi = i;
					double yyi = j;
					int color = IzingModel[i][j][center];

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
			for (int j = 0; j < IzingModel[0].size(); j++)
			{
				for (int k = 0; k < IzingModel[0][j].size(); k++)
				{
					double xxi = j;
					double yyi = k;
					int color = IzingModel[center][j][k];

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
			for (int i = 0; i < IzingModel.size(); i++)
			{
				for (int k = 0; k < IzingModel[i][0].size(); k++)
				{
					double xxi = i;
					double yyi = k;
					int color = IzingModel[i][center][k];

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

		if (check_GU.GetCheck())
		{
			MemDc->SelectObject(&line_pen);
			MemDc->MoveTo(DOTSIMAGE(1, 1));
			MemDc->LineTo(DOTSIMAGE(1, vec.size() + 1));
			MemDc->LineTo(DOTSIMAGE(vec.size() + 1, vec.size() + 1));
			MemDc->LineTo(DOTSIMAGE(vec.size() + 1, 1));
			MemDc->LineTo(DOTSIMAGE(1, 1));
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
	DrawImage(vecIzingModel, PicDcImage, PicImage);
}

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}

vector<int> CIzingModelDlg::BorderConditions(int rand_idx) {
	vector<int> neigbours;
	// Если выбран не граничный атом.
	if (rand_idx > 0 && rand_idx < value_size - 1) {
		neigbours.push_back(rand_idx - 1);
		neigbours.push_back(rand_idx + 1);
	}
	// Если выбран атом на левой границе.
	else if (rand_idx == 0) {
		neigbours.push_back(value_size - 1);
		neigbours.push_back(rand_idx + 1);
	}
	// Если выбран атом на правой границе.
	else if (rand_idx == value_size - 1) {
		neigbours.push_back(rand_idx - 1);
		neigbours.push_back(0);
	}
	return neigbours;
}

double CIzingModelDlg::CalculateHamiltonian(int i, int j, int k, int n_i, int n_j, int n_k,
	vector<vector<vector<int>>> new_cfg) {
	if (!new_cfg.empty() && !vecIzingModel.empty()) {

		// Получение соседей.
		vector<int> neig_i = BorderConditions(i);
		vector<int> neig_j = BorderConditions(j);
		vector<int> neig_k = BorderConditions(k);

		vector<int> neig_n_i = BorderConditions(n_i);
		vector<int> neig_n_j = BorderConditions(n_j);
		vector<int> neig_n_k = BorderConditions(n_k);

		double energy_last_cfg = 0.0;
		double energy_new_cfg = 0.0;
		for (int idx = 0; idx < neig_i.size(); idx++) {
			energy_last_cfg += vecIzingModel[i][j][k] * vecIzingModel[neig_i[idx]][j][k];
			energy_last_cfg += vecIzingModel[i][j][k] * vecIzingModel[i][neig_j[idx]][k];
			energy_last_cfg += vecIzingModel[i][j][k] * vecIzingModel[i][j][neig_k[idx]];
			energy_last_cfg += vecIzingModel[n_i][n_j][n_k] * vecIzingModel[neig_n_i[idx]][n_j][n_k];
			energy_last_cfg += vecIzingModel[n_i][n_j][n_k] * vecIzingModel[n_i][neig_n_j[idx]][n_k];
			energy_last_cfg += vecIzingModel[n_i][n_j][n_k] * vecIzingModel[n_i][n_j][neig_n_k[idx]];

			energy_new_cfg += new_cfg[i][j][k] * new_cfg[neig_i[idx]][j][k];
			energy_new_cfg += new_cfg[i][j][k] * new_cfg[i][neig_j[idx]][k];
			energy_new_cfg += new_cfg[i][j][k] * new_cfg[i][j][neig_k[idx]];
			energy_new_cfg += new_cfg[n_i][n_j][n_k] * new_cfg[neig_n_i[idx]][n_j][n_k];
			energy_new_cfg += new_cfg[n_i][n_j][n_k] * new_cfg[n_i][neig_n_j[idx]][n_k];
			energy_new_cfg += new_cfg[n_i][n_j][n_k] * new_cfg[n_i][n_j][neig_n_k[idx]];
		}

		energy_last_cfg *= -Ecm;
		energy_last_cfg /= 2.;
		energy_new_cfg *= -Ecm;
		energy_new_cfg /= 2.;
		double del_energy = energy_new_cfg - energy_last_cfg;
		return del_energy;
	}
}

/**
Выполнение одного изменения конфигурации.
**/
void CIzingModelDlg::MonteCarloStep() {
	// Проверка, что начальная конфигурация существует.
	if (!vecIzingModel.empty()) {
		int counter = 0;
		// 1 МКШ.
		int iteration = value_size * value_size * value_size;
		while (counter < iteration) {
			// Выбор случайного спина.
			int rand_i = RandStaff(0, value_size - 1);
			int rand_j = RandStaff(0, value_size - 1);
			int rand_k = RandStaff(0, value_size - 1);

			// Индексы соседнего спина.
			int neig_i = rand_i;
			int neig_j = rand_j;
			int neig_k = rand_k;

			// Получение вектора соседей по случайной оси, индексы.
			vector<int> axis{ 0, 1, 2 };
			while (!axis.empty()) {
				int axis_idx = *select_randomly(axis.begin(), axis.end());
				// Обработка оси X.
				if (axis_idx == 0) {
					vector<int> neigbour_i = BorderConditions(rand_i);
					for (int i = 0; i < neigbour_i.size(); i++) {
						// Проверка на разносортность.
						if (vecIzingModel[rand_i][rand_j][rand_k] == vecIzingModel[neigbour_i[i]][neig_j][neig_k]) {
							neigbour_i.erase(neigbour_i.begin() + i);
						}
					}

					// Проверка оставшихся соседей.
					if (!neigbour_i.empty()) {
						int random_idx = rand() % neigbour_i.size();
						neig_i = neigbour_i[random_idx];
						break;
					}
					else {
						axis.erase(axis.begin() + axis_idx);
					}
				}
				// Обработка оси Y.
				else if (axis_idx == 1) {
					vector<int> neigbour_j = BorderConditions(rand_j);
					for (int i = 0; i < neigbour_j.size(); i++) {
						// Проверка на разносортность.
						if (vecIzingModel[rand_i][rand_j][rand_k] == vecIzingModel[neig_i][neigbour_j[i]][neig_k]) {
							neigbour_j.erase(neigbour_j.begin() + i);
						}
					}

					// Проверка оставшихся соседей.
					if (!neigbour_j.empty()) {
						int random_idx = rand() % neigbour_j.size();
						neig_j = neigbour_j[random_idx];
						break;
					}
					else {
						axis.erase(axis.begin() + axis_idx);
					}
				}
				// Обработка оси Z.
				else if (axis_idx == 2) {
					vector<int> neigbour_k = BorderConditions(rand_k);
					for (int i = 0; i < neigbour_k.size(); i++) {
						// Проверка на разносортность.
						if (vecIzingModel[rand_i][rand_j][rand_k] == vecIzingModel[neig_i][neig_j][neigbour_k[i]]) {
							neigbour_k.erase(neigbour_k.begin() + i);
						}
					}

					// Проверка оставшихся соседей.
					if (!neigbour_k.empty()) {
						int random_idx = rand() % neigbour_k.size();
						neig_k = neigbour_k[random_idx];
						break;
					}
					else {
						axis.erase(axis.begin() + axis_idx);
					}
				}
			}

			// Если нужный сосед найден - меняется спин, считаются энергии.
			if ((rand_i + rand_j + rand_k) != (neig_i + neig_j + neig_k)) {
				// Генерация новой конфигурации.
				vector<vector<vector<int>>> new_configuration = vecIzingModel;
				int temp = new_configuration[rand_i][rand_j][rand_k];
				new_configuration[rand_i][rand_j][rand_k] = new_configuration[neig_i][neig_j][neig_k];
				new_configuration[neig_i][neig_j][neig_k] = temp;

				// Расчет энергий, изменения энергии.
				double hamiltonian = CalculateHamiltonian(rand_i, rand_j, rand_k, neig_i, neig_j, neig_k, new_configuration);
				if (hamiltonian < 0) {
					vecIzingModel = new_configuration;
				}
				else {
					// Случайное число в диапазоне [0;1].
					double random_value = (double)(rand()) / RAND_MAX;
					double exponent = exp(-hamiltonian / (K * TEMPERATURE));
					if (random_value < exponent) {
						vecIzingModel = new_configuration;
					}
				}
			}

			if (radio_with_graph.GetCheck() == BST_CHECKED) {
				DrawImage(vecIzingModel, PicDcImage, PicImage);
			}
			counter++;
		}
	}
}

/**
Выполнение алгоритма метрополиса.
**/
void CIzingModelDlg::MonteCarlo() {
	int counter = 1;
	char step[100];
	while (true) {
		// Вывод иинформации на экран.
		sprintf_s(step, "%d", counter);
		CURRENT_MKSH_STEP.SetWindowTextW((CString)step);

		// Выполнение 1 Монте-Карло шага.
		MonteCarloStep();
		counter++;

		// Условие завершения.
		if (counter >= MKSH_QOUNT) {
			break;
		}
	}

	MessageBox(L"Визуализация успешно завершена!", L"Информация", MB_ICONINFORMATION | MB_OK);
	bRunTh = false;
	button_picture.SetWindowTextW(bStartVisualization);
	TerminateThread(hThread, 0);
	CloseHandle(hThread);
	hThread = NULL;
}

void CIzingModelDlg::OnBnClickedPicture()
{
	// Выполнение визуализации МКШ.
	UpdateData(TRUE);
	if (!bRunTh) {
		button_picture.SetWindowTextW(bStopVisualization);
		if (hThread == NULL) {
			if (vecIzingModel.empty()) {
				vecIzingModel.clear();
				OnBnClickedCalculate();
			}

			hThread = CreateThread(NULL, 0, MyProc, this, 0, &dwThread);
		}
		else {
			ResumeThread(hThread);
		}
		bRunTh = true;
	}
	else {
		button_picture.SetWindowTextW(bStartVisualization);
		bRunTh = false;
		SuspendThread(hThread);
	}
}

void CIzingModelDlg::OnBnClickedDropping()
{
	// Остановка потока.
	if (bRunTh) {
		bRunTh = false;
		SuspendThread(hThread);
		button_picture.SetWindowTextW(bStartVisualization);
	}

	// Завершение потока.
	TerminateThread(hThread, 0);
	CloseHandle(hThread);
	hThread = NULL;

	CURRENT_MKSH_STEP.SetWindowTextW(L"");

	// Очистка глобального вектора.
	vecIzingModel.clear();
	DrawImage(vecIzingModel, PicDcImage, PicImage);
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