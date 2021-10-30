


#pragma once
#include "BidirectionalMap.h"
#include <set>

template<class Container>
auto my_equal_range(Container&& container, double target, double epsilon = 0.00001)
-> decltype(container.equal_range(target))
{
	auto lower = container.lower_bound(target - epsilon);
	auto upper = container.upper_bound(target + epsilon);
	return std::make_pair(lower, upper);
}

template<typename A, typename B>
std::pair<B, A> flip_pair(const std::pair<A, B>& p)
{
	return std::pair<B, A>(p.second, p.first);
}

template<typename A, typename B>
std::map<B, A> flip_map(const std::map<A, B>& src)
{
	std::map<B, A> dst;
	std::transform(src.begin(), src.end(), inserter(dst, dst.begin()),
		flip_pair<A, B>);
	return dst;
}

class BezierCurve {
public:
	using Point = glm::vec3;
	using Points = std::vector<Point>;
	using ParameterValue = float;
	using ArcLength = float;

	static void GetControlPoints(const Points& knots, Points& outFirstControlPoints, Points& outSecondControlPoints)
	{
		if (knots.size() <= 2)
		{
			DebugBreak();
			return;
		}

		size_t n = knots.size();
		std::vector<double> a(n, 0);
		std::vector<double> b(n, 0);
		std::vector<double> c(n, 0);
		std::vector<Point> rhs(n, glm::vec3(0));

		for (int i = 1; i < n - 1; ++i)
		{
			a[i] = 1;
			b[i] = 4;
			c[i] = 1;
			rhs[i] = 4 * knots[i] + 2 * knots[i + 1];
		}

		a[0] = 0;
		b[0] = 2;
		c[0] = 1;
		rhs[0] = knots[0] + 2 * knots[0];

		a[n - 1] = 2;
		b[n - 1] = 7;
		c[n - 1] = 0;
		rhs[n - 1] = (8 * knots[n - 1] + knots[n]) / 2.0f;

		for (int i = 1; i < n; i++)
		{
			double m = a[i] / b[i - 1];
			b[i] = b[i] - m * c[i - 1];
			rhs[i] = rhs[i] - m * rhs[i - 1];
		}


		outFirstControlPoints.resize(n);
		outFirstControlPoints[n - 1] = rhs[n - 1] / b[n - 1];

		for (int i = n - 2; i >= 0; --i)
		{
			outFirstControlPoints[i] = (rhs[i] - c[i] * outFirstControlPoints[i + 1]) / b[i];
		}

		outSecondControlPoints.resize(n);
		/*we have p1, now compute p2*/
		for (int i = 0; i < n - 1; ++i)
			outSecondControlPoints[i] = 2 * knots[i + 1] - outFirstControlPoints[i + 1];

		outSecondControlPoints[n - 1] = 0.5 * (knots[n] + outFirstControlPoints[n - 1]);

	}
	static glm::vec3 Interpolate(double t, const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3)
	{
		glm::vec3 point;
		t = std::clamp<double>(t, 0, 1);
		double u = 1 - t;
		double w1 = u * u * u;
		double w2 = 3 * u * u * t;
		double w3 = 3 * u * t * t;
		double w4 = t * t * t;

		return glm::vec3(
			w1 * P0.x + w2 * P1.x + w3 * P2.x + w4 * P3.x,
			w1 * P0.y + w2 * P1.y + w3 * P2.y + w4 * P3.y,
			w1 * P0.z + w2 * P1.z + w3 * P2.z + w4 * P3.z);
	}

	



public:
	BezierCurve() = default;

	void AddPoint(Point point)
	{
		points.emplace_back(point);
	}
	void AddPoints(const Points& input_points)
	{
		// knots.insert(points.begin(), points.begin(), points.end());

		for (auto& point : input_points)
		{
			points.emplace_back(point);
		}
	}

	void CaclulateResultantPoints()
	{
		generatedControlPoints.clear();
		

		auto& localKnots = points;

		auto n = localKnots.size();
		std::vector<glm::dvec3> a(n, Point());
		a.resize(n);
		std::vector<glm::dvec3> b(n, Point());
		b.resize(n);
		for (int i = 1; i < n - 1; ++i)
		{
			a[i] = localKnots[i] + (localKnots[i + 1] - localKnots[i - 1]) / 2.0;
			b[i] = localKnots[i] - (localKnots[i + 1] - localKnots[i - 1]) / 2.0;
		}

		a[0] = (b[1] + localKnots[0]) / 2.0;
		b[n - 1] = (localKnots[n - 1] + a[n - 2]) / 2.0;

		for (int i = 0; i < points.size() - 1; ++i)
		{
			generatedControlPoints.emplace_back(localKnots[i]);
			generatedControlPoints.emplace_back(a[i]);
			generatedControlPoints.emplace_back(b[i + 1]); 
			generatedControlPoints.emplace_back(localKnots[i + 1]);
		}


	}

	std::vector<glm::dvec3>& GetPoints()
	{
		return points;
	}

	const Points& GetGeneratedControlPoints() const
	{
		return generatedControlPoints;
	}

	struct CurveSegment
	{
		CurveSegment() = default;
		CurveSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d):
			p0(a), p1(b), p2(c), p3(d){}

		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;

		// Values that are compared to the entire bezier curve
		float ua = 0;
		float ub = 1;
		float startArcLength = 0;
		float endArcLength = 0;

		std::map<ParameterValue, ArcLength> distanceToParameterValueLookup;

		friend std::ostream& operator<< (std::ostream& os, const CurveSegment& seg);

		// Input: Distance to travel on this curve segment
		glm::vec3 Interpolate(float distance)
		{
			// Get distance of this curve segment
			const float totalCurveLength = distanceToParameterValueLookup.rbegin()->second;

			// Error checking
			if(distance < 0 || distance > totalCurveLength)
				DebugBreak();

			// Find the t value 
			const float t = distance / totalCurveLength;

			// Look for the t in the lookup table
			auto range = my_equal_range(distanceToParameterValueLookup, t, 0.01);

			// If second is end
			if (range.second == distanceToParameterValueLookup.end())
			{
				// Use the first
				return BezierCurve::Interpolate(range.first->first, *this);
			}
			// Its in between first and second
			else
			{
				// Linear map
				float t_prime = (t - range.first->first) / (range.second->first - range.first->first);
				return BezierCurve::Interpolate(t_prime, *this);
			}
		}
	};
	
	static std::map<ParameterValue, ArcLength> GenerateTable(const CurveSegment& curveSegment)
	{
		using ArcLengthSegment = float;
		struct T_Pair
		{
			T_Pair(ParameterValue a, ParameterValue b) : ua(a), ub(b){}
			ParameterValue ua = 0;
			ParameterValue ub = 1;
			bool operator== (T_Pair& other)
			{
				return ua == other.ua && ub == other.ub;
			}
		};
		auto cmp = [](const T_Pair& a, const T_Pair& b) {
			return a.ua < b.ua;
		};
		std::set<T_Pair, decltype(cmp)> openList(cmp);
		std::map<ParameterValue, ArcLengthSegment> intermediateTable;
		std::map<ParameterValue, ArcLength> finalTable;

		openList.insert(T_Pair(0, 1));
		
		while (!openList.empty())
		{
			// Copy the top, remove it
			const T_Pair top = *openList.begin();
			openList.erase(top);
			const Point& left = Interpolate(top.ua, curveSegment);
			const Point& right = Interpolate(top.ub, curveSegment);

			// Compute midpoint of curve
			ParameterValue um = (top.ua + top.ub) / 2;
			Point midPoint = Interpolate(um, curveSegment);

			float lengthA = glm::distance(left, midPoint);
			float lengthB = glm::distance(right, midPoint);
			float lengthC = glm::distance(left, right);

			float d = lengthA + lengthB - lengthC;

			if (d > epislon)
			{
				// Greater then, both halves in
				openList.insert(T_Pair(top.ua, um));
				openList.insert(T_Pair(um, top.ub));
			}
			else
			{
				// Record paramter midpoint
				intermediateTable.insert({um, lengthA});
				// Record end point
				intermediateTable.insert({top.ub, lengthB});
			}


		}

		// The intermediate table should have a mapping of t -> segmented length. We need to make sure the length is the total distance
		float currentdistance = 0;
		for (const auto& segment : intermediateTable)
		{
			currentdistance += segment.second;
			finalTable.insert({segment.first, currentdistance});
		}



		return finalTable;
	}

	void GenerateDistanceTable()
	{
		parameterValueToCurveSegmentLookup.clear();
		curveSegments.clear();

		std::vector<std::map<ParameterValue, ArcLength>> segmentedTables;
		segmentedTables.reserve(generatedControlPoints.size() / 4);

		// For each curve segment, we generate a table
		for (int i = 0; i < generatedControlPoints.size(); i += 4)
		{
			CurveSegment segment = {
				generatedControlPoints[i],
				generatedControlPoints[i + 1],
				generatedControlPoints[i + 2],
				generatedControlPoints[i + 3]
			};
			
			segmentedTables.emplace_back(GenerateTable(segment));
			segment.distanceToParameterValueLookup = flip_map(segmentedTables.back());
			curveSegments.emplace_back(segment);
		}

		// Accumulate all the ends of each curve segment lookup to find total value
		float l_totalDistance = 0;
		std::vector<float> arcLengthDistances;
		arcLengthDistances.emplace_back(0);
		for (int i = 0; i < segmentedTables.size(); ++i)
		{
			l_totalDistance += segmentedTables[i].rbegin()->second;
			// Also cache the total distance of each arcsegment
			arcLengthDistances.emplace_back(l_totalDistance);
		}
		totalDistance = l_totalDistance;
		std::cout << "Calculated total distance: " << totalDistance << std::endl;

		// Construct a lookup table that given a t value, returns us the curve segment
		for (int i = 0; i < segmentedTables.size(); ++i)
		{
			// At i = 0, no need to fill this. Aftewards, fill with the begining value of the last value (aka ua from last segment)
			if(!parameterValueToCurveSegmentLookup.empty())
			{ 
				curveSegments[i].ua = parameterValueToCurveSegmentLookup.rbegin()->first;
			}
			for (auto& iter = segmentedTables[i].begin(); iter != segmentedTables[i].end(); ++iter)
			{
				const auto& val = *iter;
				float dist = arcLengthDistances[i] + val.second;

				parameterValueToCurveSegmentLookup.insert({dist / totalDistance, &curveSegments[i]});
			}
			// At the end of the curve segment, we also record the t value
			curveSegments[i].ub = parameterValueToCurveSegmentLookup.rbegin()->first;
		}

		//std::cout << "Final Table" << std::endl;
		//for (const auto& segment : parameterDistanceLookup)
		//{
		//	std::cout << "\t " << segment.first << " : " << *segment.second << std::endl;
		//}
	}

	inline static float epislon = 0.005f;

	Point Interpolate(double t)
	{
		auto range = my_equal_range(parameterValueToCurveSegmentLookup, t, 0.01);
		
		// If the two iteraters refer to the same curve, use the same curveSegment
		if (range.first == range.second)
		{
			const CurveSegment& curveSegment = *range.first->second;
			// Verify
			if (curveSegment.ua <= t && t <= curveSegment.ub)
			{
				// Remap t
				float t_prime = (t - curveSegment.ua) / (curveSegment.ub - curveSegment.ua);
				return Interpolate(t_prime, curveSegment);
			}
			else
			{
				DebugBreak();
			}
		}
		else
		{
			// Different curve segments. 
			// If one is end then use the other
			if (range.first == parameterValueToCurveSegmentLookup.end())
			{
				const CurveSegment& curveSegment = *range.second->second;
				// Remap t
				float t_prime = (t - curveSegment.ua) / (curveSegment.ub - curveSegment.ua);
				return Interpolate(t_prime, curveSegment);
			}
			else if (range.second == parameterValueToCurveSegmentLookup.end())
			{
				// Remap t
				const CurveSegment& curveSegment = *range.first->second;
				float t_prime = (t - curveSegment.ua) / (curveSegment.ub - curveSegment.ua);
				return Interpolate(t_prime, curveSegment);
			}

			// Neither are end. Compare
			const CurveSegment& curveSegment = *range.first->second;
			const CurveSegment& curveSegment2 = *range.second->second;
			if (curveSegment.ua <= t && t <= curveSegment.ub)
			{
				// Remap t
				float t_prime = (t - curveSegment.ua) / (curveSegment.ub - curveSegment.ua);
				return Interpolate(t_prime, curveSegment);
			}
			else if (curveSegment2.ua <= t && t <= curveSegment2.ub)
			{
				// Remap t
				float t_prime = (t - curveSegment2.ua) / (curveSegment2.ub - curveSegment2.ua);
				return Interpolate(t_prime, curveSegment2);
			}
			else
			{
				DebugBreak();
			}
		}
	}

private:
	static glm::vec3 Interpolate(double t, const CurveSegment& segment)
	{
		return Interpolate(t, segment.p0, segment.p1, segment.p2, segment.p3);
	}
private:
	std::vector<glm::dvec3> points;
	Points generatedControlPoints;
	float totalDistance = 0;
	std::vector<CurveSegment> curveSegments;
	std::map<ParameterValue, CurveSegment*> parameterValueToCurveSegmentLookup;
};



