#include <cstdio>
#include <iomanip>

#include "wbStroke.hpp"
#include "wbText.hpp"
#include "wbGraph.hpp"
using namespace Gdiplus;

#define VK_ALPHA(ch) (0x41 + (ch) - 'A')

const int ttsDrag = 128;

// API variables
MSG msg;
HWND hwnd;
ULONG_PTR m_gdiplusToken;
WNDCLASSEX wc;
PAINTSTRUCT ps;
Color bkgColor = Color::White;
FontFamily ff(L"等线");
Font f(&ff, 16, FontStyleRegular, UnitPoint);
GdiplusStartupInput StartupInput;
const wchar_t wndClassName[] = L"Oierwb Window Class", wndName[] = L"OIer Whiteboard";
const wchar_t *strMode[] = {
	L"Select",
	L"Pen",
	L"Erase",
	L"Text",
	L"Graph",
};

// Profile variables
int mx, my;
int mbState, curFn = 1;
clock_t tMbDown;
int pnState, pnX = 25, pnY = 30;

std::vector<int> candidateColor{ 0x000000, 0xff0000, 0xffff00, 0x0000ff };

wbGraph gr;
std::pair<int,int> grSele1, grSele2;

void wbPaintPanel(Graphics &g) {
	Font f1(L"等线", 12);
	SolidBrush sbFg(Color::Black);
	SolidBrush sbBg(0xb0ffffff);
	Pen pFg(Color::Black);
	if(pnState < 1) return;
	PointF ptMode(pnX, pnY);
	RectF rcMode;
	g.MeasureString(strMode[curFn], -1, &f1, ptMode, &rcMode);
	g.FillRectangle(&sbBg, rcMode);
	g.DrawString(strMode[curFn], -1, &f1, ptMode, &sbFg);
	g.DrawRectangle(&pFg, rcMode);
	if(pnState < 2) return;
	if(curFn == 1) {
		PointF ptPenWidth(rcMode.GetRight(), rcMode.GetTop());
		RectF rcPenWidth;
		std::wstringstream ss(L"");
		ss << L"Width: " << std::fixed << std::setprecision(0) << curStroke.width;
		g.MeasureString(ss.str().c_str(), -1, &f1, ptPenWidth, &rcPenWidth);
		g.FillRectangle(&sbBg, rcPenWidth);
		g.DrawString(ss.str().c_str(), -1, &f1, ptPenWidth, &sbFg);
		g.DrawRectangle(&pFg, rcPenWidth);

		PointF ptAlpha(rcPenWidth.GetRight(), rcPenWidth.GetTop());
		RectF rcAlpha;
		ss.str(L"");
		ss << L"Alpha: " << (curStroke.color.GetValue() >> 24);
		g.MeasureString(ss.str().c_str(), -1, &f1, ptAlpha, &rcAlpha);
		g.FillRectangle(&sbBg, rcAlpha);
		g.DrawString(ss.str().c_str(), -1, &f1, ptAlpha, &sbFg);
		g.DrawRectangle(&pFg, rcAlpha);

		PointF ptColor1(rcAlpha.GetRight(), rcAlpha.GetTop());
		RectF rcColor1, rcColor2;
		g.MeasureString(L"Color ", -1, &f1, ptColor1, &rcColor1);
		REAL coBtnSize = rcColor1.Height;
		PointF ptColor2(rcColor1.GetRight() + candidateColor.size() * coBtnSize, rcAlpha.GetTop());
		g.MeasureString(L"…", -1, &f1, ptColor2, &rcColor2);
		RectF rcColor(ptColor1, { rcColor2.GetRight() - ptColor1.X, coBtnSize });
		g.FillRectangle(&sbBg, rcColor);
		g.DrawRectangle(&pFg, rcColor);
		g.DrawString(L"Color ", -1, &f1, ptColor1, &sbFg);
		g.DrawString(L"…", -1, &f1, ptColor2, &sbFg);
		for(unsigned i=0; i<candidateColor.size(); ++i) {
			SolidBrush sbCo(0xff000000 | candidateColor[i]);
			g.FillEllipse(&sbCo, rcColor1.GetRight() + i * coBtnSize + coBtnSize * 0.2f, rcColor1.GetTop() + coBtnSize * 0.2f, coBtnSize * 0.6f, coBtnSize * 0.6f);
		}
	}
}

void wbPaint(HDC hdc) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	Bitmap bmp(rc.right - rc.left, rc.bottom - rc.top);
	Graphics g(&bmp), gDest(hdc);

	g.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	g.Clear(bkgColor);
	for(auto s : strokes) {
		wbDrawStroke(s, g);
	}

	for(int i = 0; i < (int)texts.size(); i++) {
		wbDrawText(texts[i], curFn == 3 && i == curSeleTxt, g);
	}

	gr.paint(g, grSele2);

	wbPaintPanel(g);

	CachedBitmap cb(&bmp, &gDest);
	gDest.DrawCachedBitmap(&cb, 0, 0);
	gDest.ReleaseHDC(hdc);
}

void fnChangeFn(int x) {
	if(curFn != x) {
		pnState = 1;
		if(curFn == 3) {
			wbDeselectText();
		}
	} else {
		++pnState;
	}
	curFn = x;
}

bool LBtnDownPanel(HDC hdc) {
	if(pnState < 1) return false;
	PointF ptMouse(mx, my);
	Graphics g(hdc);

	Font f1(L"等线", 12);
	Pen pFg(Color::Black);
	PointF ptMode(pnX, pnY);
	RectF rcMode;
	g.MeasureString(strMode[curFn], -1, &f1, ptMode, &rcMode);
	if(rcMode.Contains(ptMouse)) {
		pnState = 0;
		wbPaint(hdc);
		return true;
	} else if(my < rcMode.GetTop() || my > rcMode.GetBottom()) {
		return false;
	}
	if(pnState < 2) return false;
	if(curFn == 1) {
		PointF ptPenWidth(rcMode.GetRight(), rcMode.GetTop());
		RectF rcPenWidth;
		std::wstringstream ss(L"");
		ss << L"Width: " << std::fixed << std::setprecision(0) << curStroke.width;
		g.MeasureString(ss.str().c_str(), -1, &f1, ptPenWidth, &rcPenWidth);

		PointF ptAlpha(rcPenWidth.GetRight(), rcPenWidth.GetTop());
		RectF rcAlpha;
		ss.str(L"");
		ss << L"Alpha: " << (curStroke.color.GetValue() >> 24);
		g.MeasureString(ss.str().c_str(), -1, &f1, ptAlpha, &rcAlpha);

		if(rcPenWidth.Contains(ptMouse) || rcAlpha.Contains(ptMouse)) {
			return true;
		}

		PointF ptColor1(rcAlpha.GetRight(), rcAlpha.GetTop());
		RectF rcColor1, rcColor2;
		g.MeasureString(L"Color ", -1, &f1, ptColor1, &rcColor1);
		REAL coBtnSize = rcColor1.Height;
		PointF ptColor2(rcColor1.GetRight() + candidateColor.size() * coBtnSize, rcAlpha.GetTop());
		g.MeasureString(L"…", -1, &f1, ptColor2, &rcColor2);
		RectF rcColor(ptColor1, { rcColor2.GetRight() - ptColor1.X, coBtnSize });

		if(rcColor1.Contains(ptMouse)) {
			return true;
		} else if(rcColor2.Contains(ptMouse)) {
			return true;
		} else if(rcColor.Contains(ptMouse)) {
			int i = (mx - rcColor1.GetRight()) / coBtnSize;
			if(i >= 0 && (unsigned)i < candidateColor.size()) {
				curStroke.color = (curStroke.color.GetValue() & 0xff000000) | candidateColor[i];
				wbPaint(hdc);
			}
			return true;
		}
	}
	return false;
}

bool wheelPanel(short delta, HDC hdc) {
	if(pnState < 1) return false;
	PointF ptMouse(mx, my);
	Graphics g(hdc);

	Font f1(L"等线", 12);
	Pen pFg(Color::Black);
	PointF ptMode(pnX, pnY);
	RectF rcMode;
	g.MeasureString(strMode[curFn], -1, &f1, ptMode, &rcMode);
	if(rcMode.Contains(ptMouse)) {
		return true;
	} else if(my < rcMode.GetTop() || my > rcMode.GetBottom()) {
		return false;
	}
	if(pnState < 2) return false;
	if(curFn == 1) {
		PointF ptPenWidth(rcMode.GetRight(), rcMode.GetTop());
		RectF rcPenWidth;
		std::wstringstream ss(L"");
		ss << L"Width: " << std::fixed << std::setprecision(0) << curStroke.width;
		g.MeasureString(ss.str().c_str(), -1, &f1, ptPenWidth, &rcPenWidth);

		PointF ptAlpha(rcPenWidth.GetRight(), rcPenWidth.GetTop());
		RectF rcAlpha;
		ss.str(L"");
		ss << L"Alpha: " << (curStroke.color.GetValue() >> 24);
		g.MeasureString(ss.str().c_str(), -1, &f1, ptAlpha, &rcAlpha);

		if(rcPenWidth.Contains(ptMouse)) {
			curStroke.width += delta / 60.0;
			if(curStroke.width < 1)
				curStroke.width = 1;
			wbPaint(hdc);
			return true;
		} else if(rcAlpha.Contains(ptMouse)) {
			int curAlpha = curStroke.color.GetValue() >> 24;
			curAlpha += 0.21 * delta;
			curAlpha = std::min(std::max(curAlpha, 37), 255);
			curStroke.color.SetValue(curAlpha << 24 | (curStroke.color.GetValue() & 0xffffff));
			wbPaint(hdc);
			return true;
		}
	}
	return false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msgVal, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	SolidBrush sb(Color::Red);
	switch(msgVal) {
		case WM_CREATE: {
			GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
			curStroke.color = Color::Black;
			curStroke.width = 5.0f;
			break;
		}
		case WM_LBUTTONDOWN: {
			mx = LOWORD(lParam), my = HIWORD(lParam);
			bool isToPaint = 0;
			hdc = GetDC(hwnd);
			if(LBtnDownPanel(hdc)) {
				ReleaseDC(hwnd, hdc);
				break;
			}
			mbState |= 1;
			tMbDown = clock();
			if(curFn == 1) {
				wbStrokeNewPt(curStroke, PointF(mx, my));
				curStroke.pts.push_back(PointF(mx, my));
			} else if(curFn == 3) {
				wbSelectText(PointF(mx, my), hdc);
				isToPaint = 1;
			} else if(curFn == 4) {
				grSele1 = gr.select(PointF(mx, my), false);
				if(grSele1.first == 1 && grSele2.first == 1) {
					gr.createEdge(grSele1.second, grSele2.second);
					mbState ^= 1;
					grSele2 = { 0, 0 };
					isToPaint = 1;
				}
			}
			if(isToPaint) {
				wbPaint(hdc);
			}
			ReleaseDC(hwnd, hdc);
			break;
		}
		case WM_MOUSEMOVE: {
			mx = LOWORD(lParam), my = HIWORD(lParam);
			bool isToPaint = 0;
			if(mbState & 1) {
				if(curFn == 1) {
					hdc = GetDC(hwnd);
					Graphics g(hdc);
					g.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

					wbStrokeNewPt(curStroke, PointF(mx, my), g);

					g.ReleaseHDC(hdc);
					ReleaseDC(hwnd, hdc);
				} else if(curFn == 2) {
					if(wbEraseStroke(mx, my)) {
						isToPaint = 1;
					}
				} else if(curFn == 4) {
					if(clock() - tMbDown > ttsDrag) {
						gr.moveVertex(grSele1.second, PointF(mx, my));
						isToPaint = 1;
					}
				}
			}
			if(isToPaint) {
				hdc = GetDC(hwnd);
				wbPaint(hdc);
				ReleaseDC(hwnd, hdc);
			}
			break;
		}
		case WM_LBUTTONUP: {
			mx = lParam & 0xffff, my = lParam >> 16;
			bool isToPaint = 0;
			if(mbState & 1) {
				mbState ^= 1;
				if(curFn == 1) {
					isToPaint = 1;
					hdc = GetDC(hwnd);
					Graphics g(hdc);
					g.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

					wbStrokeNewPt(curStroke, PointF(mx, my), g);

					strokes.push_back(curStroke);
					curStroke.pts.clear();

					g.ReleaseHDC(hdc);
					ReleaseDC(hwnd, hdc);
				} else if(curFn == 4) {
					if(clock() - tMbDown <= ttsDrag) {
						grSele2 = gr.select(PointF(mx, my), true);
						isToPaint = 1;
					}
				}
			}
			if(isToPaint) {
				hdc = GetDC(hwnd);
				wbPaint(hdc);
				ReleaseDC(hwnd, hdc);
			}
			break;
		}
		case WM_MOUSEWHEEL: {
			hdc = GetDC(hwnd);
			wheelPanel(GET_WHEEL_DELTA_WPARAM(wParam), hdc);
			ReleaseDC(hwnd, hdc);
			break;
		}
		case WM_PAINT: {
			hdc = BeginPaint(hwnd, &ps);

			wbPaint(hdc);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_KEYDOWN: {
			bool isToPaint = 0;
			if(!(lParam & KF_REPEAT)) {
				if(GetKeyState(VK_CONTROL) & 0x8000) {
					if(wParam == VK_ALPHA('D')) {
						if(MessageBox(hwnd, L"Are you sure to clear all the strokes?", wndName, MB_YESNO) != IDNO)
							fnClearStroke(), isToPaint = 1;
					} else if(wParam == VK_ALPHA('Z')) {
						fnPopStroke(), isToPaint = 1;
					}
				} else {
					if(wParam >= VK_F1 && wParam <= VK_F4) {
						fnChangeFn(wParam - VK_F1 + 1);
						isToPaint = 1;
					}
				}
			}
			if(isToPaint) {
				hdc = GetDC(hwnd);
				wbPaint(hdc);
				ReleaseDC(hwnd, hdc);
			}
			break;
		}
		case WM_CHAR: {
			hdc = GetDC(hwnd);
			if(curFn == 3) {
				if(wParam == 27) { // esc
					wbDeselectText();
				}
				wbAddToText(wParam);
				wbPaint(hdc);
			} else if(curFn == 4) {
				if(wParam == 27) {
					grSele2 = std::make_pair(0, 0);
				}
			}
			ReleaseDC(hwnd, hdc);
			break;
		}
		case WM_DESTROY: {
			// GdiplusShutdown(m_gdiplusToken);
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, msgVal, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hIns, HINSTANCE /*hPreIns*/, LPSTR /*lpCmdLn*/, int /*nCmdShow*/) {
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hIns;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = wndClassName;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Failed to regiter window class!", L"Error", MB_ICONERROR | MB_OK);
		return 1;
	}

	hwnd = CreateWindowEx(
	WS_EX_CLIENTEDGE,
	wndClassName,
	wndName,
	WS_VISIBLE | WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, CW_USEDEFAULT,
	1024, 768,
	NULL,
	NULL,
	hIns,
	NULL);
	if(!hwnd) {
		MessageBox(NULL, L"Failed to create window!", L"Error", MB_ICONERROR | MB_OK);
		return 2;
	}

	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}