#include "wbText.hpp"

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

// the default text collection
int curSeleTxt;
std::vector<wbText> texts;

void wbDelText(unsigned i) {
	if(i >= texts.size())
		return;
	std::swap(texts[i], *texts.rbegin());
	texts.pop_back();
}

void wbDeselectText() {
	if(curSeleTxt >= 0 && curSeleTxt < (int)texts.size() && texts[curSeleTxt].s.empty()) {
		wbDelText(curSeleTxt);
	}
	curSeleTxt = -1;
}

void wbSelectText(PointF ptMouse, HDC hdc) {
	wbDeselectText();
	Graphics g(hdc);
	for(int i = 0; i < (int)texts.size(); i++) {
		RectF rcTxt;
		Font fTxt(texts[i].fName.c_str(), texts[i].fSize, texts[i].fStyle);
		g.MeasureString(texts[i].s.c_str(), -1, &fTxt, texts[i].o, &rcTxt);
		if(rcTxt.Contains(ptMouse)) {
			curSeleTxt = i;
			return;
		}
	}
	curSeleTxt = texts.size();
	texts.push_back({ ptMouse, L"等线", 12.0f, 0, L"" });
}

void wbAddToText(wchar_t ch) {
	if(curSeleTxt < 0 || curSeleTxt >= (int)texts.size()) {
		return;
	}
	// if(ch < 32)
	// 	printf("char input: %d\n",ch);
	if(ch == 8) {
		if(texts[curSeleTxt].s.size())
			texts[curSeleTxt].s.pop_back();
	} else {
		texts[curSeleTxt].s.push_back(ch);
		if(ch == 13) {
			texts[curSeleTxt].s.push_back(L'\n');
		}
	}
}
