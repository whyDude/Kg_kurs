#ifndef _WINAPP_H_
#define _WINAPP_H_

#include <Windows.h>
#include <cstdint>
#include <string>
#include <memory>

#include "config.h"
#include "DrawingArea.h"

#define IDN_MAIN_WND TEXT("MAIN_WINDOW")

class WinApp
{
public:
	WinApp() = delete;
	WinApp(LPCWSTR windowName, HINSTANCE hInstance, LPWSTR lpCmdLine, int32_t nCmdShow);
	~WinApp() {};

	int16_t create();
	WPARAM run();

private:
	void registration();
	static LRESULT CALLBACK MainWindowEventHander(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	WNDCLASSEX _wndclass;
	LPWSTR _lpCmdLine;
	int32_t _nCmdShow;
	HINSTANCE _hInstance;
	MSG _msg;

	std::wstring _windowName;
	HWND _mainWindowHwnd;

private:
	std::unique_ptr<graphics::DrawingArea> drawingArea;
	bool _showFrame = false;
	bool _showEdges = false;
};

#endif