#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "wbGeo.hpp"
using namespace Gdiplus;

extern const int nodeSize, edgeWidth;

struct wbVertex {
	PointF o;
	std::wstring s;
	RectF getDispRect() {
		RectF ret(o, SizeF(0, 0));
		ret.Inflate(nodeSize, nodeSize);
		return ret;
	}
};

struct wbEdge {
	unsigned u, v;
	char dir;
	std::wstring s;
};
    
struct wbGraph {
	std::vector<wbVertex> vs;
	std::vector<wbEdge> es;

	std::pair<int, int> select(PointF pt, bool);
	void createNode(PointF pt);
	void createEdge(unsigned u, unsigned v);
	void moveVertex(int i, PointF pos);
	void delVertex(unsigned i);
	void _delEdge(unsigned i);
	void delEdge(unsigned i);
	void paint(Graphics &g, std::pair<int, unsigned> sele);
};
