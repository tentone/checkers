#include <iostream>

#include "MathUtils.cpp"
#include "opencv2/core/core.hpp"

#ifndef TRIANGLE_CPP
#define TRIANGLE_CPP

using namespace cv;

class Triangle
{
	public:
		Point2f points[3];

		Triangle()
		{
			for(int i=0; i < 3; i++)
			{
				this->points[i] = Point2f(0.0, 0.0);
			}
		}

		Triangle(Point a, Point b, Point c)
		{
			this->points[0] = a;
			this->points[1] = b;
			this->points[2] = c;
		}

		float area()
		{
			return MathUtils::mod(points[0].x*(points[1].y-points[2].y) + points[1].x*(points[2].y-points[0].y) + points[2].x*(points[0].y-points[1].y)) / 2.0;
		}
};

#endif
