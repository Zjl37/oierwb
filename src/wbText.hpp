#pragma once
#include <string>
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

void wbDrawText(const wbText &t, bool edit, Graphics &g) {
	Font font(t.fName.c_str(), t.fSize, t.fStyle);
	SolidBrush sb1(Color::Black);
	if(edit) {
		g.DrawString((t.s + L"|").c_str(), -1, &font, t.o, &sb1);
		RectF rc;
		g.MeasureString((t.s + L"\u200b").c_str(), -1, &font, t.o, &rc);
		Pen pen(Color::Black);
		g.DrawRectangle(&pen, rc);
	} else {
		g.DrawString(t.s.c_str(), -1, &font, t.o, &sb1);
	}
}