#include "math/Quadrilateral.cpp"

#ifndef SQUARE_FINDER_CPP
#define SQUARE_FINDER_CPP

using namespace cv;
using namespace std;

class SquareFinder
{
	private:
		//Square detection configuration
		static const int canny_upper_thresh = 30;
		static const int thresh_it = 30;

	public:
		//Returns sequence of squares detected on the image the sequence is stored in the specified memory storage
		static vector<Quadrilateral> findSquares(Mat image)
		{
			//Squares found
			vector<Quadrilateral> squares = vector<Quadrilateral>();

			//Matrix
			Mat img, gray(image.size(), CV_8U);

			//Use full image
			img = image.clone();

			vector<vector<Point> > contours;

			//Find squares in every color plane of the image
			for(int c = 0; c < 3; c++)
			{
				int ch[] = {c, 0};
				mixChannels(&img, 1, &gray, 1, ch, 1);

				//Try several threshold levels
				for(int l = 0; l < thresh_it; l++ )
				{
					//Canny helps to catch squares with gradient shading
					if(l == 0)
					{
						//Apply Canny lower threshold zero
						Canny(gray, gray, 0, canny_upper_thresh, 5);

						//Dilate canny output to remove potential holes between edge segments
						dilate(gray, gray, Mat(), Point(-1,-1));
					}
					//Apply threshold if l!=0:
					else
					{
						gray >= (l+1)*255/thresh_it;
					}

					//Find contours and store them all as a list
					findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
					vector<Point> approx;

					for(unsigned int i = 0; i < contours.size(); i++)
					{
						//Approximate contour with accuracy proportional to the contour perimeter
						approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

						//Square contours should have 4 vertices after approximation relatively large area (to filter out noisy contours)and be convex.
						if(approx.size() == 4 && fabs(contourArea(Mat(approx))) > 1000 && isContourConvex(Mat(approx)))
						{
							float maxCosine = 0;

							//Find the maximum cosine of the angle between joint edges
							for(int j = 2; j < 5; j++)
							{
								float cosine = fabs(angleCornerPointsCos(approx[j%4], approx[j-2], approx[j-1]));
								maxCosine = MAX(maxCosine, cosine);
							}

							//Check if all angle corner close to 90 (more than 60)
							if(maxCosine < 0.5)
							{
								Quadrilateral quad = Quadrilateral();
								int j = 0;
								while(!approx.empty() && j < 4)
								{
									quad.points[j] = approx.back();
									approx.pop_back();
									j++;
								}
								squares.push_back(quad);
							}
						}
					}
				}
			}
			return squares;
		}

		//Finds a cosine of angle between vectors from pt0->pt1 and from pt0->pt2
		static float angleCornerPointsCos(Point b, Point c, Point a)
		{
			float dx1 = b.x - a.x;
			float dy1 = b.y - a.y;
			float dx2 = c.x - a.x;
			float dy2 = c.y - a.y;
			return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
		}
};

#endif
