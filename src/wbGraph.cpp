#include "wbGraph.hpp"

const int nodeSize = 25, edgeWidth = 4;

void wbGraph::createNode(PointF pt) {
	std::wstringstream _ws(L"");
	_ws << vs.size() + 1;
	vs.push_back({ pt, _ws.str() });
}

std::pair<int, int> wbGraph::select(PointF pt, bool forceCreate) {
	for(unsigned i = 0; i < vs.size(); i++) {
		if(dist(pt, vs[i].o) <= nodeSize) {
			return std::make_pair(1, i);
		}
	}
	for(unsigned i = 0; i < es.size(); i++) {
		if(distSeg(vs[es[i].u].o, vs[es[i].v].o, pt) <= 4) {
			return std::make_pair(2, i);
		}
	}
	if(forceCreate) {
		createNode(pt);
	}
	return std::make_pair(0, 0);
}

void wbGraph::moveVertex(int i, PointF pos) {
	if(i >= 0 && i < (int)vs.size()) {
		vs[i].o = pos;
	}
}

void wbGraph::createEdge(int u, int v) {
	es.push_back({ u, v, 0, L"" });
}

void wbGraph::paint(Graphics &g, std::pair<int, unsigned> sele) {
	Pen pen(Color::Black, edgeWidth);
	HatchBrush hb(HatchStyle::HatchStyle30Percent, Color::LightGray, Color::Black);
	Pen pen2(&hb, edgeWidth);
	for(unsigned i = 0; i < es.size(); i++) {
		g.DrawLine(sele.first == 2 && sele.second == i ? &pen2 : &pen, vs[es[i].u].o, vs[es[i].v].o);
	}
	SolidBrush sbBg(0xffeeeeee), sbFg(Color::Black);
	Font f1(L"Calibri", 16);
	StringFormat sfCenter;
	sfCenter.SetAlignment(StringAlignment::StringAlignmentCenter);
	sfCenter.SetLineAlignment(StringAlignment::StringAlignmentCenter);
	for(unsigned i = 0; i < vs.size(); i++) {
		RectF rc = vs[i].getDispRect();
		g.FillEllipse(&sbBg, rc);
		g.DrawEllipse(sele.first == 1 && sele.second == i ? &pen2 : &pen, rc);
		g.DrawString(vs[i].s.c_str(), -1, &f1, rc, &sfCenter, &sbFg);
	}
}