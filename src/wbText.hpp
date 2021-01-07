#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

struct wbText {
	PointF o;
	std::wstring fName;
	float fSize;
	int fStyle;
	std::wstring s;
};

void wbDrawText(const wbText &t, bool edit, Graphics &g);

extern int curSeleTxt;
extern std::vector<wbText> texts;

void wbDeselectText();
void wbSelectText(PointF ptMouse, HDC hdc);
void wbAddToText(wchar_t ch);