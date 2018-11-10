#include <iostream>
#include <math.h>

#include "opencv2/core/core.hpp"

#ifndef MATHUTILS_CPP
#define MATHUTILS_CPP

using namespace cv;

class MathUtils
{
	public:
		//Check if value in range
		static bool inRange(float value, float min, float max)
		{
			return value >= min && value <= max;
		}

		//Truncate value to range
		static int truncate(int value, int min, int max)
		{
			if(value < min)
			{
				return min;
			}
			if(value > max)
			{
				return max;
			}
			return value;
		}

		//Value module
		static int mod(float value)
		{
			if(value <  0)
			{
				return -value;
			}
			return value;
		}

		//Distance between points
		static float distanceBetweenPoints(Point a, Point b)
		{
			return norm(a-b);
		}

		//Angle Betweeen point (degrees)
		static float angleBetweenPoints(Point a, Point b)
		{
			return atan2(b.y-a.y, b.x-a.x) * 180 / 3.14159265359;
		}
};

#endif
