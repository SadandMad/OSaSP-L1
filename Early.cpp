#include <windows.h>

int leftPos = 220, topPos = 140;
int width, height;
HANDLE hBitmap;
bool flDrag = false;
int mX, mY;

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HDC hDC, hCompatibleDC;
	PAINTSTRUCT PaintStruct;
	HANDLE hOldBitmap;
	RECT Rect;

	switch (Message) {

		/* Upon destruction, tell the main thread to stop */
	case WM_DESTROY: {
		/* удалить загруженный битмап */
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT: {
		GetClientRect(hwnd, &Rect);
		if (leftPos < 0) {
			leftPos = 0;
		}
		if (topPos < 0) {
			topPos = 0;
		}
		if (leftPos + width > Rect.right) {
			leftPos = Rect.right - width;
		}
		if (topPos + height > Rect.bottom) {
			topPos = Rect.bottom - height;
		}
		/* получить контекст устройства */
		hDC = BeginPaint(hwnd, &PaintStruct);
		/* создать совместимый с контекстом окна контекст в памяти */
		hCompatibleDC = CreateCompatibleDC(hDC);
		/* делаем загруженный битмап текущим в совместимом контексте */
		hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
		StretchBlt(hDC, leftPos, topPos, width, height, hCompatibleDC, 0, 0, width, height, SRCAND);
		/* вновь делаем старый битмап текущим */
		SelectObject(hCompatibleDC, hOldBitmap);
		/* удалить совместимый контекст */
		DeleteDC(hCompatibleDC);
		/* освободить основной контекст, завершая перерисовку рабочей области окна */
		EndPaint(hwnd, &PaintStruct);
		return 0;
	}
	case WM_KEYDOWN: {
		if (wParam == VK_LEFT) {
			leftPos -= 5;
		}
		if (wParam == VK_RIGHT) {
			leftPos += 5;
		}
		if (wParam == VK_UP) {
			topPos -= 5;
		}
		if (wParam == VK_DOWN) {
			topPos += 5;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	}
	case WM_MOUSEWHEEL: {
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
			leftPos += GET_WHEEL_DELTA_WPARAM(wParam) / 60;
		}
		else {
			topPos -= GET_WHEEL_DELTA_WPARAM(wParam) / 60;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	}
	case WM_SIZE: {
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}
	case WM_LBUTTONDOWN: {
		if ((LOWORD(lParam) >= leftPos) and (LOWORD(lParam) <= leftPos + width) and (HIWORD(lParam) >= topPos) and (HIWORD(lParam) <= topPos + height)) {
			flDrag = true;
			mX = LOWORD(lParam) - leftPos;
			mY = HIWORD(lParam) - topPos;
		}
		break;
	}
	case WM_MOUSEMOVE: {
		if (flDrag) {
			leftPos = LOWORD(lParam) - mX;
			topPos = HIWORD(lParam) - mY;
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	}
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP: {
		flDrag = false;
		break;
	}
	/* All other messages (a lot of them) are processed using default procedures */
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG Msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc; /* This is where we will send messages to */
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);

	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = L"MyCoolWinow";
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Ошибка при регистрации окна!", L"Проблемка!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"MyCoolWinow", L"Labka 1", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		640, /* width */
		480, /* height */
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, L"Ошибка при создании окна!", L"Проблемка!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/* загрузить bitmap который будет отображаться в окне из файла */
	hBitmap = LoadImage(NULL, L"sprite.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBitmap) {
		MessageBox(NULL, L"Спрайт не найден!", L"Проблемка!", MB_OK);
		return 0;
	}
	BITMAP Bitmap;
	/* получть размерность изображения */
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	width = Bitmap.bmWidth;
	height = Bitmap.bmHeight;

	/*
		This is the heart of our program where all input is processed and
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while (GetMessage(&Msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&Msg); /* Translate key codes to chars if present */
		DispatchMessage(&Msg); /* Send it to WndProc */
	}
	return Msg.wParam;
}