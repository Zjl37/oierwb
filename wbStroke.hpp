#pragma once
#include <vector>
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

inline double sqr(double x) {
	return x * x;
}

struct vect {
	double x, y;
	vect(): x(0), y(0) {}
	vect(Point p): x(p.X), y(p.Y) {}
	vect(PointF p): x(p.X), y(p.Y) {}
	vect(double _x, double _y): x(_x), y(_y) {}
	vect operator+(const vect &b) const {
		return vect(x + b.x, y + b.y);
	}
	vect operator-(const vect &b) const {
		return vect(x - b.x, y - b.y);
	}
	double operator*(const vect &b) const {
		return x * b.x + y * b.y;
	}
	vect operator*(double fac) const {
		return vect(x * fac, y * fac);
	}
};

inline double dist(const vect &a, const vect &b) {
	return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

inline double distSeg(const vect &a, const vect &b, const vect &p) {
	if((p - a) * (b - a) < 0)
		return dist(p, a);
	double pro = (p - b) * (a - b);
	if(pro < 0)
		return dist(p, b);
	return dist(p, b + (a - b) * pro * sqr(1 / dist(a, b)));
}

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
