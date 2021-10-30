#include "stdafx.h"
#include "BezierCurve.h"

std::ostream& operator<<(std::ostream& os, const BezierCurve::CurveSegment& seg)
{
	os << "[ " << seg.ua << ", " << seg.ub << "]";
	return os;
}
