#include <windows.h>

int leftPos = 220, topPos = 140;
int hSpeed = 0, vSpeed = 0;
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
		/* ������� ����������� ������ */
		DeleteObject(hBitmap);
		KillTimer(hwnd, 1);
		KillTimer(hwnd, 2);
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT: {
		GetClientRect(hwnd, &Rect);
		if (leftPos < 0) {
			leftPos = 0;
			hSpeed = -hSpeed;
		}
		if (topPos < 0) {
			topPos = 0;
			vSpeed = -vSpeed;
		}
		if (leftPos + width > Rect.right) {
			leftPos = Rect.right - width;
			hSpeed = -hSpeed;
		}
		if (topPos + height > Rect.bottom) {
			topPos = Rect.bottom - height;
			vSpeed = -vSpeed;
		}
		/* �������� �������� ���������� */
		hDC = BeginPaint(hwnd, &PaintStruct);
		/* ������� ����������� � ���������� ���� �������� � ������ */
		hCompatibleDC = CreateCompatibleDC(hDC);
		/* ������ ����������� ������ ������� � ����������� ��������� */
		hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
		StretchBlt(hDC, leftPos, topPos, width, height, hCompatibleDC, 0, 0, width, height, SRCAND);
		/* ����� ������ ������ ������ ������� */
		SelectObject(hCompatibleDC, hOldBitmap);
		/* ������� ����������� �������� */
		DeleteDC(hCompatibleDC);
		/* ���������� �������� ��������, �������� ����������� ������� ������� ���� */
		EndPaint(hwnd, &PaintStruct);
		return 0;
	}
	case WM_KEYDOWN: {
		if (!(lParam & 1073741824)) {
			if (wParam == VK_LEFT) {
				hSpeed -= 5;
			}
			if (wParam == VK_RIGHT) {
				hSpeed += 5;
			}
			if (wParam == VK_UP) {
				vSpeed -= 5;
			}
			if (wParam == VK_DOWN) {
				vSpeed += 5;
			}
			if (wParam == VK_SPACE) {
				hSpeed = 0;
				vSpeed = 0;
			}
		}
		return 0;
	}
	case WM_MOUSEWHEEL: {
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
			hSpeed += GET_WHEEL_DELTA_WPARAM(wParam) / 60;
		}
		else {
			vSpeed -= GET_WHEEL_DELTA_WPARAM(wParam) / 60;
		}
		return 0;
	}
	case WM_SIZE: {
		GetWindowRect(hwnd, &Rect);
		if (LOWORD(lParam) < width) {
			MoveWindow(hwnd, Rect.left, Rect.top, width, Rect.bottom - Rect.top, TRUE);
		}
		if (HIWORD(lParam) < height + 45) {
			MoveWindow(hwnd, Rect.left, Rect.top, Rect.right - Rect.left, height + 45, TRUE);
		}
		break;
	}
	case WM_LBUTTONDOWN: {
		if ((LOWORD(lParam) >= leftPos) and (LOWORD(lParam) <= leftPos + width) and (HIWORD(lParam) >= topPos) and (HIWORD(lParam) <= topPos + height)) {
			flDrag = true;
			hSpeed = 0;
			vSpeed = 0;
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
	case WM_TIMER: {
		switch (wParam)
		{
		case 1: {
			leftPos += hSpeed;
			topPos += vSpeed;
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		case 2: {
			if (hSpeed) {
				hSpeed -= hSpeed / abs(hSpeed);
			}
			if (vSpeed) {
				vSpeed -= vSpeed / abs(vSpeed);
			}
			break;
		}
		default:
			break;
		}
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
		MessageBox(NULL, L"������ ��� ����������� ����!", L"���������!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"MyCoolWinow", L"L1", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		640, /* width */
		480, /* height */
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, L"������ ��� �������� ����!", L"���������!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/* ��������� bitmap ������� ����� ������������ � ���� �� ����� */
	hBitmap = LoadImage(NULL, L"sprite.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBitmap) {
		MessageBox(NULL, L"������ �� ������!", L"���������!", MB_OK);
		return 0;
	}
	BITMAP Bitmap;
	/* ������� ����������� ����������� */
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	width = Bitmap.bmWidth;
	height = Bitmap.bmHeight;

	SetTimer(hwnd, 1, 20, NULL);
	SetTimer(hwnd, 2, 500, NULL);

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