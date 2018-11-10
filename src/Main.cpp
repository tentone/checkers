#include <iostream>
#include <string>

#include "opencv2/videoio.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo/photo.hpp"

#include "CheckersGame.cpp"
#include "SquareFinder.cpp"

#define USE_CAMERA 1

#define STATE_CALIB 0
#define STATE_RUNNING 1

using namespace cv;
using namespace std;

//Function Prototypes
void mouseEvents(int event, int x, int y, int flags, void *param);
void calibrateColors();
void processImage();
Mat drawCheckersGame(Point2i size, CheckersGame game);
Mat joinImages(Mat a, Mat b, ColorRGB exclude);
Mat filterColor(Mat image, ColorRGB color, ColorRGB tolerance, ColorRGB mask);
Mat cropImage(Mat image, Quadrilateral quad);
Mat deformQuad(Mat image, Point2i size_out, vector<Point2f> quad);
void getCheckerData(Mat image);
int countByColor(Mat image, ColorRGB color);
Point colorConcentrationPoint(Mat image, ColorRGB color);

//State
int state = STATE_RUNNING;

//Calibrated Colors
int calibColor = 0;
ColorRGB yellow_calib = YELLOW_DARK, blue_calib = BLUE_DARK, green_calib = GREEN_DARK, black_calib = BLACK, white_calib = WHITE;
ColorRGB yellow_tolerance = ColorRGB(130), blue_tolerance = ColorRGB(130), green_tolerance = ColorRGB(100), black_tolerance = ColorRGB(190), white_tolerance = ColorRGB(150);

//Noise remove
bool filter_noise = false;

//Rotate board
int rotate_board = 1;

//Global variables
VideoCapture cap;
CheckersGame game;
Mat frame;

//Main function
int main()
{
    Mat image;

    //Open the default camera
    if(USE_CAMERA && !cap.open(0))
	{
    	cout << "Failed to open video device" << endl;
    	return 0;
	}
    else
    {
    	frame = imread("data/board.jpg", CV_LOAD_IMAGE_UNCHANGED);
    }

	//Create window
    namedWindow("Checkers", WINDOW_AUTOSIZE);
    setMouseCallback("Checkers", mouseEvents, NULL);

    //Running state
    while(true)
    {
    	//Get frame
    	if(USE_CAMERA)
    	{
    		cap >> frame;
    		if(filter_noise)
    		{
    			fastNlMeansDenoisingColored(frame, frame, 3, 7, 5, 21);
    		}
    	}

    	if(state == STATE_RUNNING)
    	{
    		processImage();
    	}
    	else if(state == STATE_CALIB)
    	{
    		calibrateColors();
    	}

		//Get Keyboard input
		char key = (char)waitKey(1);

		//Escape pressed
		if(key == (char)27)
		{
			destroyAllWindows();
			return 0;
		}
		//Enter calibration state
		else if(key == 'c')
		{
			state = STATE_CALIB;
			calibColor = 0;
		}
		//Toogle noise filter
		else if(key == 'n')
		{
			filter_noise = !filter_noise;
		}
    }
    destroyAllWindows();
    return 0;
}

//Update calibrate color state
void calibrateColors()
{
	Mat img = frame.clone();

	putText(img, "Calibrating Colors", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255), 1);
	if(calibColor == 0)
	{
		putText(img, "Yellow", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,255), 2);
	}
	else if(calibColor == 1)
	{
		putText(img, "Blue", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,0,0), 2);
	}
	else if(calibColor == 2)
	{
		putText(img, "Black", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2);
	}
	else if(calibColor == 3)
	{
		putText(img, "White", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255), 2);
	}
	else if(calibColor == 4)
	{
		putText(img, "Green", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,0), 2);
	}

	imshow("Checkers", img);

	if(state == STATE_RUNNING)
	{
		return;
	}
}

//Process image to identify checkers board
void processImage()
{
	//Color filtering
	Mat sum = filterColor(frame, white_calib, white_tolerance, WHITE); //White
	sum = joinImages(sum, filterColor(frame, yellow_calib, yellow_tolerance, YELLOW_DARK), BLACK); //Yellow
	sum = joinImages(sum, filterColor(frame, blue_calib, blue_tolerance, BLUE_DARK), BLACK); //Blue
	sum = joinImages(sum, filterColor(frame, green_calib, green_tolerance, GREEN_DARK), BLACK); //Green
	sum = joinImages(sum, filterColor(frame, black_calib, black_tolerance, BLACK), BLACK); //Black

	//Isolate white and look for bigger quad
	Mat white = filterColor(sum, WHITE, BLACK, WHITE);
	vector<Quadrilateral> quads = SquareFinder::findSquares(white);

	if(quads.size() > 0)
	{
		Quadrilateral page_quad = Quadrilateral::biggerQuadrilateral(quads);

		//Crop Image to quad found
		sum = cropImage(sum, page_quad);
		page_quad.draw(sum, WHITE, 2);

		//Filter colors
		Mat white = filterColor(sum, WHITE, BLACK, WHITE);
		Mat yellow = filterColor(sum, YELLOW_DARK, BLACK, YELLOW_DARK);
		Mat blue = filterColor(sum, BLUE_DARK, BLACK, BLUE_DARK);
		Mat green = filterColor(sum, GREEN_DARK, BLACK, GREEN_DARK);
		Mat yellow_blue = joinImages(blue, yellow, BLACK);

		//Find Chess square
		quads = SquareFinder::findSquares(yellow_blue);
		if(quads.size() > 0)
		{
			Quadrilateral board_quad = Quadrilateral::biggerQuadrilateral(quads);

			//Get point for green color concentration
			Point reference_point = colorConcentrationPoint(green, GREEN_DARK);
			circle(green, reference_point, 5, Scalar(255,0,255), -1);

			//Board Quad Mid Point
			Point mid_board_point = Point(0,0);
			for(int y = 0; y < 4; y++)
			{
				mid_board_point.x += board_quad.points[y].x;
				mid_board_point.y += board_quad.points[y].y;
			}
			mid_board_point.x /= 4;
			mid_board_point.y /= 4;
			circle(green, mid_board_point, 5, Scalar(0,255,255), -1);

			//Correct board rotation
			float angle = MathUtils::angleBetweenPoints(reference_point, mid_board_point);
			if(angle > 0 && angle < 90)
			{
				rotate_board = 0;
			}
			else if(angle > 90 && angle < 180)
			{
				rotate_board = 1;
			}
			else if(angle > -180 && angle < -90)
			{
				rotate_board = 2;
			}
			else if(angle > -90 && angle < 0)
			{
				rotate_board = 3;
			}

			//Rotate board quad points
			for(int y = 0; y < rotate_board; y++)
			{
				Point2f a = board_quad.points[0];
				board_quad.points[0] = board_quad.points[1];
				board_quad.points[1] = board_quad.points[2];
				board_quad.points[2] = board_quad.points[3];
				board_quad.points[3] = a;
			}

			//Deform game board
			Mat board = deformQuad(sum, Point2i(160,160), board_quad.points);
			imshow("Board", board);

			//Display all colors individually
			/*imshow("Channel YELLOW-BLUE", yellow_blue);
			imshow("Channel WHITE", white);
			imshow("Channel BLUE", blue);
			imshow("Channel YELLOW", yellow);
			imshow("Channel GREEN", green);*/

			//Get Game
			getCheckerData(board);

			//Draw on top of original frame
			sum = joinImages(frame, sum, ColorRGB(0));

			//Draw Quads
			page_quad.draw(sum, VIOLET, 3);
			board_quad.draw(sum, LIME, 3);

			//Draw Text
			putText(sum, "C -> Calibrate Color", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1);
			putText(sum, "N -> Toogle Noise Filter", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1);
			putText(sum, "ESC -> Exit", Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1);

			//Display Image
			imshow("Checkers", sum);
			imshow("CheckersGameBoard", drawCheckersGame(Point2i(500,500), game));
			return;
		}
	}

	//Draw Text
	putText(frame, "C -> Calibrate Color", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1);
	putText(frame, "N -> Toogle Noise Filter", Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1);
	putText(frame, "ESC -> Exit", Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1);
	putText(frame, "No board found!", Point(frame.cols/2-100, frame.rows/2), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255), 2);

	//Display Image
	imshow("Checkers", frame);
}

//Join b on top on a excluding exclude color
Mat joinImages(Mat a, Mat b, ColorRGB exclude)
{
	Mat out = a.clone();
	int i, j, t;

	for(i = 0; i < out.rows; i++)
	{
		for(j = 0; j < out.cols; j++)
		{
			t = (i*out.cols+j)*3;
			if(!(b.data[t] == exclude.b && b.data[t+1] == exclude.g && b.data[t+2] == exclude.r))
			{
				out.data[t] = b.data[t];
				out.data[t+1] = b.data[t+1];
				out.data[t+2] = b.data[t+2];
			}
		}
	}

	return out;
}

//Filter Image by color and create mask using mask color
Mat filterColor(Mat image, ColorRGB color, ColorRGB tolerance, ColorRGB mask)
{
	Mat out = image.clone();
	int i, j, t;

	for(i = 0; i < out.rows; i++)
	{
		for(j = 0; j < out.cols; j++)
		{
			t = (i*out.cols+j)*3;
			if(MathUtils::inRange(out.data[t],color.b-tolerance.b,color.b+tolerance.b)&& MathUtils::inRange(out.data[t+1],color.g-tolerance.g,color.g+tolerance.g) && MathUtils::inRange(out.data[t+2],color.r-tolerance.r,color.r+tolerance.r))
			{
				out.data[t] = mask.b;
				out.data[t+1] = mask.g;
				out.data[t+2] = mask.r;
			}
			else
			{
				out.data[t] = 0;
				out.data[t+1] = 0;
				out.data[t+2] = 0;
			}
		}
	}

	return out;
}

//Crop image to quad
Mat cropImage(Mat image, Quadrilateral quad)
{
	Mat out = Mat::zeros(image.rows, image.cols, CV_8UC3);
	Point p[1][4];
	p[0][0] = quad.points[0];
	p[0][1] = quad.points[1];
	p[0][2] = quad.points[2];
	p[0][3] = quad.points[3];
	const Point* points[1] = {p[0]};
	int points_count[] = {4};

	//Create Mask
	fillPoly(out, points, points_count, 1, Scalar(1, 1, 1));

	//Apply mask to image
	int i, j, t;
	for(i = 0; i < out.rows; i++)
	{
		for(j = 0; j < out.cols; j++)
		{
			t = (i*out.cols+j)*3;
			out.data[t] *= image.data[t];
			out.data[t+1] *= image.data[t+1];
			out.data[t+2] *= image.data[t+2];
		}
	}

	return out;
}

//Get quad zone and deform
Mat deformQuad(Mat image, Point2i size_out, vector<Point2f> quad)
{
	Mat out = Mat::zeros(size_out.x, size_out.y, CV_8UC3);

	//Create list of points that fit out image
	vector<Point2f> points;
	points.push_back(Point2f(0, 0));
	points.push_back(Point2f(0, out.rows));
	points.push_back(Point2f(out.cols, out.rows));
	points.push_back(Point2f(out.cols, 0));

	//Create transformation matrix
	Mat transf = getPerspectiveTransform(quad, points);

	//Apply perspective transformation
	warpPerspective(image, out, transf, out.size(), INTER_NEAREST);

	return out;
}

//Median point of color concentration
Point colorConcentrationPoint(Mat image, ColorRGB color)
{
	int x = 0, y = 0;
	int samples = 0;
	int i, j, t;

	for(i = 0; i < image.rows; i++)
	{
		for(j = 0; j < image.cols; j++)
		{
			t = (i*image.cols+j)*3;
			if(image.data[t] == color.b && image.data[t+1] == color.g && image.data[t+2] == color.r)
			{
				x += j;
				y += i;
				samples ++;
			}
		}
	}

	if(samples != 0)
	{
		return Point(x/samples, y/samples);
	}
	return Point(-1, -1);
}

//Get chessboard data
void getCheckerData(Mat image)
{
	int i, j;
	Point2i step = Point2i(image.cols/8, image.rows/8);
	int pixel_count = step.x * step.y * 0.4;

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			Mat section = image(Range(i*step.x,(i+1)*step.x), Range(j*step.y,(j+1)*step.y)).clone();
			int black_pixel_count = countByColor(section, BLACK);
			int white_pixel_count = countByColor(section, WHITE);

			if(black_pixel_count > pixel_count)
			{
				game.board[j][i] = 1;
			}
			else if(white_pixel_count > pixel_count)
			{
				game.board[j][i] = 2;
			}
			else
			{
				game.board[j][i] = 0;
			}

			//Show info of some cells
			/*if(i == 3 && j == 3)
			{
				char buf[10];
				imshow(string(itoa(i,buf,10))+","+string(itoa(j,buf,10)), section);
				cout << pixel_count << "," << black_pixel_count << "," << white_pixel_count << endl;
			}*/
		}
	}
}

//Count number of colors in image
int countByColor(Mat image, ColorRGB color)
{
	int count = 0;
	unsigned int size = image.cols * image.rows * 3;

	for(unsigned int i = 0; i < size; i += 3)
	{
		if(image.data[i] == color.b &&  image.data[i+1] == color.g && image.data[i+2] == color.r)
		{
			count++;
		}
	}

	return count;
}

//Draw checker game to image
Mat drawCheckersGame(Point2i size, CheckersGame game)
{
	Mat out = Mat::zeros(size.y, size.x, CV_8UC3);
	int size_8_x = size.x/8, size_8_y = size.y/8, size_16_x = size.x/16, size_16_y = size.y/16;

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8 ;j++)
		{
			if((i+j)%2 == 0)
			{
				rectangle(out, Point(j*size_8_x,i*size_8_y), Point((j+1)*size_8_x,(i+1)*size_8_y), Scalar(YELLOW_DARK.b, YELLOW_DARK.g, YELLOW_DARK.r), -1);
			}
			else
			{
				rectangle(out, Point(j*size_8_x,i*size_8_y), Point((j+1)*size_8_x,(i+1)*size_8_y), Scalar(BLUE_DARK.b, BLUE_DARK.g, BLUE_DARK.r), -1);
			}

			if(game.board[i][j] != 0)
			{
				if(game.board[i][j] == 1)
				{
					circle(out, Point(j*size_8_x+size_16_x,i*size_8_y + size_16_y), size_16_y, Scalar(0,0,0), -1);
				}
				else
				{
					circle(out, Point(j*size_8_x+size_16_x,i*size_8_y + size_16_y), size_16_y, Scalar(255,255,255), -1);
				}
			}
		}
	}

	return out;
}

//Called on every mouse action
void mouseEvents(int event, int x, int y, int flags, void *param)
{
	//Get calibration color
	if(state == STATE_CALIB)
	{
		if(event == EVENT_LBUTTONDOWN)
		{
			int t = (y*frame.cols+x)*3;
			if(calibColor == 0)
			{
				yellow_calib.set(frame.data[t+2], frame.data[t+1], frame.data[t]);
				yellow_tolerance.set(50, 50, 50);
			}
			else if(calibColor == 1)
			{
				blue_calib.set(frame.data[t+2], frame.data[t+1], frame.data[t]);
				blue_tolerance.set(50, 50, 50);
			}
			else if(calibColor == 2)
			{
				black_calib.set(frame.data[t+2], frame.data[t+1], frame.data[t]);
				black_tolerance.set(75, 75, 75);
			}
			else if(calibColor == 3)
			{
				white_calib.set(frame.data[t+2], frame.data[t+1], frame.data[t]);
				white_tolerance.set(75, 75, 75);
			}
			else if(calibColor == 4)
			{
				green_calib.set(frame.data[t+2], frame.data[t+1], frame.data[t]);
				green_tolerance.set(40, 40, 40);
			}

			calibColor++;
			if(calibColor > 4)
			{
				state = STATE_RUNNING;
			}
		}
	}

	/*
	if(event == EVENT_LBUTTONDOWN)
	else if(event == EVENT_RBUTTONDOWN)
	else if(event == EVENT_MBUTTONDOWN)
	else if(event == EVENT_MOUSEMOVE)
	{
		cout << "(" << x << ", " << y << ")" << endl;
	}
	*/
}
