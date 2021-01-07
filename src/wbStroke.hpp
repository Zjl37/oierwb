#pragma once
#include <vector>
#include "wbGeo.hpp"
using namespace Gdiplus;

struct wbStroke {
	std::vector<PointF> pts;
	double width;
	Color color;
	bool isNear(PointF pt) {
		if(pts.size() == 1 && dist(pt, pts[0]) < 4)
			return true;
		for(int i = 1; i < (int)pts.size(); i++) {
			if(distSeg(pts[i - 1], pts[i], pt) < 4)
				return true;
		}
		return false;
	}
};

void wbDrawStroke(const wbStroke &s, Graphics &g);

void wbStrokeNewPt(wbStroke &s, PointF pt);
void wbStrokeNewPt(wbStroke &s, PointF pt, Graphics &g);

extern wbStroke curStroke;
extern std::vector<wbStroke> strokes;

void fnClearStroke();
void fnPopStroke();
bool wbEraseStroke(int x, int y);