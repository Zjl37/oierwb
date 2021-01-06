#pragma once
#include <vector>
#include "wbGeo.hpp"
using namespace Gdiplus;

struct wbStroke {
	std::vector<Point> pts;
	double width;
	Color color;
	bool isNear(int x, int y) {
		if(pts.size() == 1 && dist(vect(x, y), pts[0]) < 4)
			return true;
		for(int i = 1; i < (int)pts.size(); i++) {
			if(distSeg(pts[i - 1], pts[i], vect(x, y)) < 4)
				return true;
		}
		return false;
	}
};

void wbDrawStroke(const wbStroke &s, Graphics &g) {
	if(!s.pts.size())
		return;
	Pen p(s.color, s.width);
	g.DrawLines(&p, &s.pts[0], s.pts.size());
}

void wbStrokeNewPt(wbStroke &s, Point pt, Graphics &g) {
	Pen p(s.color, s.width);
	g.DrawLine(&p, s.pts[s.pts.size() - 1], pt);
	s.pts.push_back(pt);
}
