#pragma once
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
