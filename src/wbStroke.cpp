#include "wbStroke.hpp"

void wbDrawStroke(const wbStroke &s, Graphics &g) {
	if(!s.pts.size())
		return;
	Pen p(s.color, s.width);
	g.DrawLines(&p, &s.pts[0], s.pts.size());
}

void wbStrokeNewPt(wbStroke &s, PointF pt) {
	s.pts.push_back(pt);
}

void wbStrokeNewPt(wbStroke &s, PointF pt, Graphics &g) {
	Pen p(s.color, s.width);
	if(s.pts.size())
		g.DrawLine(&p, s.pts[s.pts.size() - 1], pt);
	s.pts.push_back(pt);
}

// the default stroke collection
wbStroke curStroke;
std::vector<wbStroke> strokes;

void fnClearStroke() {
	strokes.clear();
}

void fnPopStroke() {
	if(!strokes.size())
		return;
	strokes.pop_back();
}

bool wbEraseStroke(int x, int y) {
	for(int i = 0; i < (int)strokes.size(); i++)
		if(strokes[i].isNear(PointF(x, y)))
			return strokes.erase(strokes.begin() + i), true;
	return false;
}
