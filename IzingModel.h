﻿
// IzingModel.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CIzingModelApp:
// Сведения о реализации этого класса: IzingModel.cpp
//

class CIzingModelApp : public CWinApp
{
public:
	CIzingModelApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CIzingModelApp theApp;
