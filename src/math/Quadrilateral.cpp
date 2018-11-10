#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Triangle.cpp"
#include "../ColorRGB.cpp"

#ifndef QUAD_CPP
#define QUAD_CPP

using namespace cv;
using namespace std;

class Quadrilateral
{
	public:
		vector<Point2f> points;

		Quadrilateral()
		{
			for(int i=0; i < 4; i++)
			{
				points.push_back(Point2f(0.0, 0.0));
			}
		}

		//Calculate Area
		float area()
		{
			return contourArea(points);
		}

		//Check if point is inside quad
		bool pointInside(Point point)
		{
			float sum = 0;
			for(int j = 0; j < 3; j++)
			{
				sum += Triangle(points[j], points[j+1],point).area();
			}
			sum += Triangle(points[3], points[0],point).area();

			float area = this->area();
			return MathUtils::inRange(sum, area-0.3, area+0.3);
		}

		//Draw quadrilateral to image
		void draw(Mat image, ColorRGB color, int weigth = 1)
		{
			for(int j = 0; j < 3; j++)
			{
				line(image, points[j], points[j+1], Scalar(color.b,color.g,color.r), weigth, 8);
			}
			line(image, points[3], points[0], Scalar(color.b,color.g,color.r), weigth, 8);
		}

		//Print quadrilateral info to cout
		void print()
		{
			cout << "[" << this->points[0] << ", " << this->points[1] << ", " << this->points[2] << ", " << this->points[3] << "]" << endl;
		}

		//Get bigger square on a vector (caller have to check vector size)
		static Quadrilateral biggerQuadrilateral(vector<Quadrilateral> quads)
		{
			Quadrilateral max = quads[0];
			float max_area = quads[0].area();

			//Search for bigger quad
			for(unsigned int i = 1; i < quads.size(); i++)
			{
				float area = quads[i].area();
				if(area > max_area)
				{
					max = quads[i];
					max_area = area;
				}
			}
			return max;
		}

		//Draw only the bigger quad from a vector
		static void drawBigger(Mat image, vector<Quadrilateral> quads, ColorRGB color)
		{
			if(quads.size() > 0)
			{
				Quadrilateral::biggerQuadrilateral(quads).draw(image, color);
			}
		}

		//Draw all squares from vector to image
		static void drawVector(Mat image, vector<Quadrilateral> quads, ColorRGB color)
		{
			for(unsigned int i = 0; i < quads.size(); i++)
			{
				quads[i].draw(image, color);
			}
		}
};

#endif
