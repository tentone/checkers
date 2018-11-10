#include "math/MathUtils.cpp"

#ifndef COLOR_RGB_CPP
#define COLOR_RGB_CPP

using namespace std;

class ColorRGB
{
	public:
		int r,g,b;

		ColorRGB()
		{
			this->r = 0;
			this->g = 0;
			this->b = 0;
		}

		ColorRGB(int r, int g, int b)
		{
			this->r = r;
			this->g = g;
			this->b = b;
		}

		ColorRGB(int c)
		{
			this->r = c;
			this->g = c;
			this->b = c;
		}

		void set(int r, int g, int b)
		{
			this->r = r;
			this->g = g;
			this->b = b;
		}

		bool similarTo(ColorRGB color, ColorRGB tolerance)
		{
			return MathUtils::inRange(this->b,color.b-tolerance.b,color.b+tolerance.b)&& MathUtils::inRange(this->g,color.g-tolerance.g,color.g+tolerance.g) && MathUtils::inRange(this->r,color.r-tolerance.r,color.r+tolerance.r);
		}

		bool equals(ColorRGB val)
		{
			return this->r == val.r && this->g == val.g && this->b == val.b;
		}

		void print()
		{
			cout << "[R:" << this->r << " G:" << this->g << " B:" << this->b << "]" << endl;
		}
};

const ColorRGB WHITE = ColorRGB(255);
const ColorRGB GREY = ColorRGB(120);
const ColorRGB GREY_DARK = ColorRGB(40);
const ColorRGB BLACK = ColorRGB(0);
const ColorRGB RED = ColorRGB(255,0,0);
const ColorRGB BLUE_DARK = ColorRGB(0,51,204);
const ColorRGB BLUE = ColorRGB(0,0,255);
const ColorRGB LIME = ColorRGB(204,255,153);
const ColorRGB GREEN = ColorRGB(0,255,0);
const ColorRGB GREEN_DARK = ColorRGB(20,220,0);
const ColorRGB YELLOW = ColorRGB(255,255,0);
const ColorRGB YELLOW_DARK = ColorRGB(255,204,30);
const ColorRGB VIOLET = ColorRGB(255,0,255);

#endif
