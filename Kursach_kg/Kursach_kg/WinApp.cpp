#include "WinApp.h"

WinApp::WinApp(LPCWSTR windowName, HINSTANCE hInstance, LPWSTR lpCmdLine, int32_t nCmdShow)
{
	_lpCmdLine = lpCmdLine;
	_nCmdShow = nCmdShow;
	_hInstance = hInstance;
	_windowName = windowName;

	registration();
}

int16_t WinApp::create()
{
	_mainWindowHwnd = CreateWindowEx(0, IDN_MAIN_WND, _windowName.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, config::mainWindowWidth, config::mainWindowHeight, nullptr, nullptr, _hInstance, this);
	return (_mainWindowHwnd) ? TRUE : FALSE;
}

WPARAM WinApp::run()
{
	ShowWindow(_mainWindowHwnd, _nCmdShow);
	UpdateWindow(_mainWindowHwnd);

	BOOL ret;
	while ((ret = GetMessage(&_msg, NULL, 0, 0)) != 0)
	{
		if (ret == -1) break;
		else
		{
			TranslateMessage(&_msg);
			DispatchMessage(&_msg);
		}
	}

	return _msg.wParam;
}

void WinApp::registration()
{
	// Создаем класс основного окна
	_wndclass.cbSize = sizeof(WNDCLASSEX);
	_wndclass.style = CS_HREDRAW | CS_VREDRAW;
	_wndclass.lpfnWndProc = static_cast<WNDPROC>(MainWindowEventHander);
	_wndclass.cbClsExtra = 0;
	_wndclass.cbWndExtra = 0;
	_wndclass.hInstance = _hInstance;
	_wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	_wndclass.lpszMenuName = NULL;
	_wndclass.lpszClassName = IDN_MAIN_WND;
	_wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Регистрируем класс основного окна
	RegisterClassEx(&_wndclass);
}

LRESULT WinApp::MainWindowEventHander(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WinApp* pThis;
	LRESULT result = 0;

	if (message == WM_NCCREATE)
	{
		pThis = static_cast<WinApp*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		SetLastError(0);
		if (!SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
			return TRUE;
		}
	}
	else
	{
		pThis = reinterpret_cast<WinApp*>(GetWindowLongPtr(hWnd, GWL_USERDATA));
		bool wasHandled = false;

		if (pThis)
		{
			switch (message)
			{
			case WM_CREATE:
			{
				RECT mainRect;
				GetClientRect(hWnd, &mainRect);
				controls::WidgetRect areaSize(mainRect.left, mainRect.top,
					mainRect.right - mainRect.left, mainRect.bottom - mainRect.top);
				controls::WidgetRect compressedAreaSize = areaSize.compress(10);

				pThis->drawingArea = std::make_unique<graphics::DrawingArea>(compressedAreaSize, config::color::areaBackground);
				
				pThis->drawingArea->addObject(graphics::type::PYRAMID3, config::objectPreset::color::standartColorObject1);
				pThis->drawingArea->addObject(graphics::type::CUBE, config::objectPreset::color::standartColorObject2);

				wasHandled = true;
				result = 0;
				break;
			}
			case WM_SIZE:
			{
				if (wParam == SIZE_MINIMIZED) break;

				RECT mainRect;
				GetClientRect(hWnd, &mainRect);
				controls::WidgetRect areaSize(mainRect.left, mainRect.top,
					mainRect.right - mainRect.left, mainRect.bottom - mainRect.top);
				controls::WidgetRect compressedAreaSize = areaSize.compress(10);

				// Изменяем размер полотна
				pThis->drawingArea->updateSize(compressedAreaSize);
				RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW);

				wasHandled = true;
				result = 0;
				break;
			}
			case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
				lpMMI->ptMinTrackSize.x = config::mainWindowWidth;
				lpMMI->ptMinTrackSize.y = config::mainWindowHeight;

				wasHandled = true;
				result = 0;
				break;
			}
			case WM_PAINT:
			{
				// Рисуем полотно
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				pThis->drawingArea->drawAreaEdge(config::color::areaBackground);
				pThis->drawingArea->drawAreaObjects(pThis->_showFrame, pThis->_showEdges);

				HDC memHdc = CreateCompatibleDC(hdc);
				HBITMAP memBM = CreateBitmap(pThis->drawingArea->getWidth(), pThis->drawingArea->getHeight(), 1,
					sizeof(uint32_t) * 8, static_cast<void*>(pThis->drawingArea->getCanvas()));

				SelectObject(memHdc, memBM);

				BitBlt(hdc, pThis->drawingArea->getX(), pThis->drawingArea->getY(), pThis->drawingArea->getWidth(), pThis->drawingArea->getHeight(), memHdc, 0, 0, SRCCOPY);
				DeleteDC(memHdc);
				DeleteObject(memBM);

				EndPaint(hWnd, &ps);

				wasHandled = true;
				result = 0;
				break;
			}
			case WM_KEYDOWN:
			{

				int PZDC_flag = 1;

				int16_t step = 4;
				switch (wParam)
				{
				case 'P':
					pThis->_showFrame = !pThis->_showFrame;
					InvalidateRect(hWnd, NULL, NULL);
					break;

				case 'O':
					pThis->_showEdges = !pThis->_showEdges;
					InvalidateRect(hWnd, NULL, NULL);
					break;
				
				// Перемещения
				case VK_UP:
				{
					if (PZDC_flag)
					{
						pThis->drawingArea->PZDZ_GLOBAL_Y -= 10;
					break;
				}
					pThis->drawingArea->moveObjects(0, -10, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case VK_DOWN:
				{
					if (PZDC_flag){
						pThis->drawingArea->PZDZ_GLOBAL_Y += 10;
					break;
				}
					pThis->drawingArea->moveObjects(0, 10, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case VK_LEFT:
				{
					if (PZDC_flag){
						pThis->drawingArea->PZDZ_GLOBAL_X -= 10;
					break;
				}
					pThis->drawingArea->moveObjects(-10, 0, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case VK_RIGHT:
				{
					if (PZDC_flag){
						pThis->drawingArea->PZDZ_GLOBAL_X += 10;
					break;
				}
					pThis->drawingArea->moveObjects(10, 0, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'R':
				{
					if (PZDC_flag)
					{ 
						pThis->drawingArea->PZDZ_GLOBAL_Z += 10;
						break;
					}
					pThis->drawingArea->moveObjects(0, 0, step);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'T':
				{
					if (PZDC_flag){
						pThis->drawingArea->PZDZ_GLOBAL_Z -= 10;
					break;
				}
					pThis->drawingArea->moveObjects(0, 0, -step);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}


				// Повороты
				case 'W':
				{
					pThis->drawingArea->rotateObjects(step, 0, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'S':
				{
					pThis->drawingArea->rotateObjects(-step, 0, 0);
					InvalidateRect(hWnd, NULL, NULL); 
					break;
				}
				case 'A':
				{
					pThis->drawingArea->rotateObjects(0, -step, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'D':
				{
					pThis->drawingArea->rotateObjects(0, step, 0);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'Q':
				{
					pThis->drawingArea->rotateObjects(0, 0, -step);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'E':
				{
					pThis->drawingArea->rotateObjects(0, 0, step);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}

				case '+':
				{
					if (PZDC_flag)
						PZDC_flag = 0;
					else
						PZDC_flag = 1;


					break;
				}


				// Масштабирование

				case 'Z':
				{
					pThis->drawingArea->scaleObjects(0.01f);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}
				case 'X':
				{
					pThis->drawingArea->scaleObjects(-0.01f);
					InvalidateRect(hWnd, NULL, NULL);
					break;
				}

				}

				wasHandled = true;
				result = 0;
				break;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				wasHandled = true;
				result = 1;
				break;
			}
			}
		}
		if (!wasHandled)
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}
