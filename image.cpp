#include "image.h"
#include "bmp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>


/**
 * Image
 **/
Image::Image (int width_, int height_)
{
    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    pixels          = new Pixel[num_pixels];
    sampling_method = IMAGE_SAMPLING_POINT;

    assert(pixels != NULL);
}


Image::Image (const Image& src)
{
    width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    pixels          = new Pixel[num_pixels];
    sampling_method = IMAGE_SAMPLING_POINT;

	
    assert(pixels != NULL);
    memcpy(pixels, src.pixels, src.width * src.height * sizeof(Pixel));
}


Image::~Image ()
{
	if (pixels != NULL)
	{
	
		delete[] pixels;
		pixels = NULL;
	}
}

/*
void Image::AddNoise (double factor)
{

}
*/
float helpP(float val)
{
	return (unsigned char)(abs((int)val) < 255) ? (abs((int)val)) : (255);

}
void Image::Brighten (double factor)
{
  
	//Brightening is simply scaling the RGB values
	//by factor

	// Component-wise multiplication of pixel by scalar.
	//Pixel operator* (const Pixel& p, double f);
	//Pixel& GetPixel(int x, int y) { assert(ValidCoord(x, y)); return pixels[y*width + x]; }
	
	for (int i = 0; i < this->width;i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			Pixel &p = GetPixel(i, j);
			//p = 0;// p*factor;
			p = p*factor;
			//p.Set(0, 0, 0);
		}
	}

}


void Image::ChangeContrast (double factor)
{
 
	float averageLum = 0;

	//for each pixel get luminance and add to averageum
	for (int i = 0; i < width;i++)
	{
		for (int j = 0; j < height;j++)
		{
			Pixel p = GetPixel(i, j);
			Component c = p.Luminance();
			averageLum += (int)c;
		}
	}

	//each pixel's color channel is equal to averageLum/w*h
	//float temp = averageLum / (width*height);
	averageLum = averageLum / (width*height);
	Pixel* p2;
	for (int i = 0; i < width;i++)
	{
		for (int j = 0; j < height;j++)
		{
			p2 = new Pixel(averageLum, averageLum, averageLum);
			Pixel& p = GetPixel(i, j);
			
			//p.Set(p2->r, p2->g, p2->b);
			p = PixelLerp(*p2, p,factor);
			delete p2;
		}
	}

}


void Image::ChangeSaturation(double factor)
{
  
	//Gray = (Red + Green + Blue) / 3
	//Pixel PixelLerp (const Pixel& p, const Pixel& q, double t);
	Pixel* p2;
	for (int i = 0; i < width;i++)
	{
		for (int j = 0; j < height;j++)
		{

			//grayscale first
			Pixel& p = GetPixel(i, j);
			//Pixel p2 = p;

			//value of grayscale part
			//float average = (p.r + p.g + p.b) / 3;
			float average = p.Luminance();
			 p2 = new Pixel(average, average, average);
			//p.Set(average, average, average);
			
			//Pixel PixelLerp(const Pixel& p, const Pixel& q, double t);

		p = PixelLerp(*p2, p, factor);

		//delete so there is no mem leaks
		delete p2;
		}


	}


}

void Image::ChangeGamma(double factor)
{
  
 //new_pixel_value = old_pixel_value ^ (1.0 / gamma).
	
	for (int i = 0; i < this->width;i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			Pixel& newPixel = GetPixel(i, j);//old

		newPixel.Set(pow(newPixel.r/255.0, 1.0 / factor)*255.0, pow(newPixel.g / 255.0, 1.0 / factor)*255.0, pow(newPixel.b / 255.0, 1.0 / factor)*255.0);//new
			
		}
	}
}

Image* Image::Crop(int x, int y, int w, int h)
{
  

	assert(x >= 0 && x <= width - 1);
	assert(y >= 0 && y <= height - 1);
	Image* newImg = new Image(w,h);
	//Pixel* newPixels = new Pixel[w*h];
	/*
	for (int m = x; m < x+h;m++)
	{
		for (int n = y; n< y+w; n++)
		{
			Pixel p = GetPixel(n, m);
			
			newImg->pixels[(m - y)*w + (n - x)] = p;// GetPixel(i, j);
		
		}

	}
	*/
	for (int m = x; m < x + w;m++)
	{
		for (int n = y; n< y + h; n++)
		{
			Pixel p = GetPixel(m,n);// n*width + m

			newImg->pixels[(n - y)*w + (m - x)] = p;// GetPixel(i, j);

		}

	}
	
//	return this;
 // return &(*newImg) ;
	return newImg;
}

/*
void Image::ExtractChannel(int channel)
{
  // For extracting a channel (R,G,B) of image.  
  // Not required for the assignment
}
*/



//Author of fRand is @rep_movsd :http://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c

double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}
void Image::QuantizeNoise(int nbits,bool withNoise)
{	

	float noise = 0.0;
	srand(563);

	float b = pow(2, nbits);
	for (int i = 0; i < this->width;i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			Pixel& pix = GetPixel(i, j);

			if (withNoise)
			{
				//noise is random
				noise = fRand(-.5,.5);
			}
			
			float qRed = floor( (pix.r*b) / 256.0 + noise);
			float qGreen = floor((pix.g*b) / 256.0 + noise);
			float qBlue = floor((pix.b*b) / 256.0 + noise);

		//	float qGreen = floor(((float)pix.g / 256.0)*b);
			//float qBlue = floor(((float)pix.b / 256.0)*b);

			//pix.Set( floor(255.0* qRed/(b-1))  , floor(255.0 * qGreen / (b - 1)), floor(255.0 * qBlue / (b - 1)));
			pix.Set(helpP((255.0*qRed) / (b - 1)),helpP( (255.0* qGreen) / (b - 1)), helpP((255.0* qBlue) / (b - 1)));
		}

	}


}
void Image::Quantize (int nbits)
{
	
	assert(nbits > 0 && nbits < 9);
	QuantizeNoise(nbits, false);

}



void Image::RandomDither (int nbits)
{
 
	assert(nbits > 0 && nbits < 9);
	QuantizeNoise(nbits, true);
}


/* Matrix for Bayer's 4x4 pattern dither. */
/* uncomment its definition if you need it */

/*
static int Bayer4[4][4] =
{
    {15, 7, 13, 5},
    {3, 11, 1, 9},
    {12, 4, 14, 6},
    {0, 8, 2, 10}
};


void Image::OrderedDither(int nbits)
{
  // For ordered dithering
  // Not required for the assignment
}

*/

/* Error-diffusion parameters for Floyd-Steinberg*/
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;
float helpP(float val);
void Image::FloydSteinbergDither(int nbits)
{
  /* Your Work Here (Section 3.3.3) */


	float b = pow(2, nbits);
	for (int i = 0; i < this->width;i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			Pixel& pix = GetPixel(i, j);

			Pixel p2 = pix;

			float qRed = floor((pix.r*b) / 256.0);
			float qGreen = floor((pix.g*b) / 256.0);
			float qBlue = floor((pix.b*b) / 256.0);

			pix.Set((255.0*qRed) / (b - 1), (255.0* qGreen) / (b - 1), (255.0* qBlue) / (b - 1));

			float pErrorRed =p2.r - pix.r;
			float pErrorGreen = p2.g - pix.g;
			float pErrorBlue = p2.b - pix.b;

			float red = 0.0;
			float green = 0.0;
			float blue = 0.0;


			Pixel& p = GetPixel((i+1 + width)%width , (j+height)%height);
			red = p.r - ALPHA*pErrorRed;
			green = p.g - ALPHA*pErrorGreen;
			blue = p.b - ALPHA*pErrorBlue;

			

			p.r = helpP(red);
			p.g = helpP(green);
			p.b = helpP(blue);


			p = GetPixel((i - 1 + width) % width, (j+1 + height) % height);
			red = p.r - BETA*pErrorRed;
			green = p.g - BETA*pErrorGreen;
			blue= p.b - BETA*pErrorBlue;

			p.r = helpP(red);
			p.g = helpP(green);
			p.b = helpP(blue);

			p = GetPixel((i + width) % width, (j + 1 + height) % height);

			red = p.r - GAMMA*pErrorRed;
			green = p.g - GAMMA*pErrorGreen;
			blue = p.b - GAMMA*pErrorBlue;

			p.r = helpP(red);
			p.g = helpP(green);
			p.b = helpP(blue);
			
			p = GetPixel((i+1 + width) % width, (j + 1 + height) % height);
			red = p.r - DELTA*pErrorRed;
			green = p.g - DELTA*pErrorGreen;
			blue = p.b - DELTA*pErrorBlue;
			
			p.r = helpP(red);
			p.g = helpP(green);
			p.b = helpP(blue);
		


		}

	}

	

}

void ImageComposite(Image *bottom, Image *top, Image *result)
{
  
}

void Image::Convolve(int *filter, int n, int normalization, int absval) {
 
}

float gaussFunction(int x, int y,int n)
{
	float sigmaSq = pow((n/2)/2.0,2);
	
	return 1 / (2 * 3.14*sigmaSq) * exp(-(pow(x,2) +pow( y,2)) / (2 * sigmaSq));
}



void Image::Blur(int n)
{
 
	int filterSize = n;
	float ** guass = new float*[filterSize];
	float sum = 0;
	//make gaussian matrix
	for (int i = 0; i < filterSize; i++)
		guass[i] = new float[filterSize];

	for (int i = -filterSize / 2; i <= filterSize / 2; i++)
	{
		for (int j = -filterSize / 2; j <= filterSize / 2; j++)
		{

			guass[i + filterSize /2][j + filterSize /2] = gaussFunction(i, j, filterSize);
			sum += guass[i + filterSize / 2][j + filterSize / 2];//gaussFunction(i, j, n);
		}
	}
	
	//add all values in guass area and divide by that number
	//normalization

	for (int i = 0; i < filterSize; i++)
	{
		for (int j = 0; j < filterSize; j++)
		{
			guass[i][j] /= sum;
		}
	}
	//TODO: Divide by smallest element, round up and normalize so
	//guass is int array

	//now pass the filter over all pixels
	for (int x = 0; x < width; x++)
	{
		for (int y = 0;y < height;y++)
		{
			
			double red = 0;
			double blue = 0;
			double green = 0;

			for (int filterY = 0; filterY < filterSize; filterY++)
			{
				for (int filterX = 0; filterX < filterSize; filterX++)
				{
					int imageX = (x - filterSize / 2 + filterX + width) % width;
					int imageY = (y - filterSize / 2 + filterY + height) % height;

					Pixel p = GetPixel(imageX, imageY);

					red += p.r * guass[filterX][filterY];
					green += p.g * guass[filterX][filterY];
					blue += p.b * guass[filterX][filterY];

				}
			}
			Pixel& p = GetPixel(x, y);
			p.r = (unsigned char)(abs((int)red) < 255) ? (abs((int)red)) : (255);
			p.g = (unsigned char)(abs((int)green) < 255) ? (abs((int)green)) : (255);
			p.b = (unsigned char)(abs((int)blue) < 255) ? (abs((int)blue)) : (255);

			

		}
	}

	//delete guass array
	for (int i = 0; i < n; i++)
		delete[] guass[i];
	delete[] guass;

}

Pixel& Image::CheckIfRange(int x, int y)
{
	if (x <= width - 1 && y <= height-1 && y>= 0 && x>=0)
	{
		
		return GetPixel(x,y);
	}
	else 
	{ 
		return *(new Pixel(0,0,0)); //0
	}

}

float vals[3][3] = { { -1.0 / 7, -2.0 / 7, -1.0 / 7 },{ -2.0 / 7, 19.0 / 7,-2.0 / 7 },{ -1.0 / 7,-2.0 / 7,-1.0 / 7 } };
int Image::HelpSharpen(int x, int y)
{
	
	double red = 0;
	double blue = 0;
	double green = 0;

	for (int filterY = 0; filterY < 3; filterY++)
	{
		for (int filterX = 0; filterX < 3; filterX++)
		{
			int imageX = (x - 1 + filterX + width) % width;
			int imageY = (y - 1 + filterY + height) % height;

			Pixel p = GetPixel(imageX, imageY);

			red += p.r * vals[filterX][filterY];
			green += p.g * vals[filterX][filterY];
			blue += p.b * vals[filterX][filterY];

		}
	}
	Pixel& p = GetPixel(x, y);
	//p.r = red;p.g = green;p.b = blue;
	p.r = (unsigned char)(abs((int)red) < 255)? (abs((int)red)):(255);
	p.g = (unsigned char)(abs((int)green) < 255) ? (abs((int)green)) : (255);
	p.b = (unsigned char)(abs((int)blue) < 255) ? (abs((int)blue)) : (255);
	


	return 0;

}
void Image::Sharpen() 
{
  /* Your Work Here (Section 3.4.2) */

	for (int i = 0; i < width; i++)
	{
		for (int j = 0;j < height;j++)
		{
			HelpSharpen(i, j);
		}
	}


}

float gradientX[3][3] = { {-1.0,0.0,1.0},{-2.0,0.0,2.0},{-1.0,0.0,1.0} };
float gradientY[3][3] = { {1.0,2.0,1.0},{0.0,0.0,0.0},{-1.0,-2.0,-1.0} };
struct vec2
{
	float x, y;
};


void Image::EdgeDetect(int threshold)
{
	
  /* Your Work Here (Section 3.4.3) */
	int filterSize = 3;
	Image* xGrad = new Image(this->width,this->height);
	Image* yGrad = new Image(this->width, this->height);
		
  //now pass the filter over all pixels
	for (int x = 1; x <= width-2; x++)
	{
		for (int y = 1;y <= height-2;y++)
		{

			float tempXG = 0;
			float tempXB = 0;
			float tempXR = 0;
			float tempYG = 0;
			float tempYB = 0;
			float tempYR = 0;
			for (int filterY = 0; filterY < filterSize; filterY++)
			{
				for (int filterX = 0; filterX < filterSize; filterX++)
				{
					int imageX = ((x - filterSize / 2 + filterX));// +width) % width;
					int imageY = ((y - filterSize / 2 + filterY));// +height) % height;

					Pixel p = GetPixel(imageX, imageY);			
					
					tempXR += p.r* gradientX[filterX][filterY];
					tempXG += p.g* gradientX[filterX][filterY];
					tempXB += p.b* gradientX[filterX][filterY];

					tempYR += p.r * gradientY[filterX][filterY];
					tempYG += p.g* gradientY[filterX][filterY] ;
					tempYB += p.b * gradientY[filterX][filterY];
					
				}
			}
		
			
			tempXB = helpP(tempXB);
			tempXR = helpP(tempXR);
			tempXG = helpP(tempXG);
			tempYB = helpP(tempYB);
			tempYR = helpP(tempYR);
			tempYG = helpP(tempYG);
	
			xGrad->GetPixel(x, y).Set(tempXR, tempXG, tempXB);
			yGrad->GetPixel(x, y).Set(tempYR, tempYG, tempYB);
					
		}
	}
	//draw edges
	for (int x = 1; x <= width-2; x++)
	{
		for (int y = 1;y <= height-2;y++)
		{		
			float g = sqrt(pow(float(xGrad->GetPixel(x, y).Luminance()),2) + pow(float(yGrad->GetPixel(x, y).Luminance()),2));		
			(g > threshold) ? (this->GetPixel(x, y).Set(0, 255, 255)) : (this->GetPixel(x, y).Set(0, 0, 0));
		//	(g > threshold) ? (this->GetPixel(x, y).Set(-1, -1, 255)) : (this->GetPixel(x, y).Set(-1, -1, -1));
		}
	}

	


}
float MitchellNetravali(float x) {
	float ax = fabs(x);
	if (ax < 1) {
		return (7 * ax * ax * ax -
			12 * ax * ax + (5.3)) / 6;
		
	}
	else if ((ax >= 1) && (ax < 2)) {
		return (-2.3 * ax * ax * ax +
			12 * ax * ax -20 *
			             ax + 10.7) / 6;
		
	}
	else {
		return 0;
		
	}
	
}

Image* Image::Scale(int sizex, int sizey)
{
	//make sure sizex/y are not zero
	assert(sizex!=0 && sizey!=0);

	float xRatio = width / (double)sizex;
	float yRatio = height / (double)sizey;
	
	Image* newImg = new Image(sizex, sizey);
	

  

	
	if (sampling_method == 0)
	{
		float px = 0, py = 0;
		//use nearest neighbor 
		for (int i = 0; i < sizey; i++)
		{
			for (int j = 0; j < sizex; j++)
			{
				px = floor(j*xRatio);
				py = floor(i*yRatio);
				Pixel& p1 = newImg->GetPixel(j,i);
				Pixel p2 = this->GetPixel(px, py);

				p1 = p2;
			}


		}
		return newImg;
	}
	else if (sampling_method == 1)
	{
		//use hat filter
		float px = 0, py = 0;
		
		for (int i = 0; i < sizey; i++)
		{
			for (int j = 0; j < sizex; j++)
			{
				//px = floor(j*xRatio);
				//py = floor(i*yRatio);
				px = j*xRatio;
				py = i*yRatio;
				
				int x = floor(px);
				int y = floor(py);

				float pxFracPart = px - x;
				float pyFracPart = py - y;
				Pixel& p1 = newImg->GetPixel(j, i);


				Pixel p2 = PixelLerp(PixelLerp(CheckIfRange(x,y), CheckIfRange(x+1, y), pxFracPart),
						  PixelLerp(CheckIfRange(x, y+1), CheckIfRange(x + 1, y+1), pxFracPart), pyFracPart);

				//Pixel p2 = this->GetPixel(px, py);

				p1 = p2;
			}


		}
		return newImg;
	}
	else if (sampling_method == 2)
	{
		//use mitchell cubic filter

		//use hat filter
		float px = 0, py = 0;

		for (int i = 0; i < sizex; i++)
		{
			for (int j = 0; j < sizey; j++)
			{
				//px = floor(j*xRatio);
				//py = floor(i*yRatio);
				px = i*xRatio;
				py = j*yRatio;

				int x = floor(px);
				int y = floor(py);

				float dx = px - x;
				float dy = py - y;
				Pixel& p1 = newImg->GetPixel(i,j);

				float red = 0;
				float green = 0;
				float blue = 0;

				for (int m = -1; m <= 2;m++)
				{
					for (int n = -1; n <= 2;n++)
					{
						//Pixel p = CheckIfRange(i + m, j + n);
						Pixel p = CheckIfRange(x + m, y + n);
						red += float(p.r)*MitchellNetravali(m-dx)*MitchellNetravali(dy-n);
						blue += float(p.b)*MitchellNetravali(m - dx)*MitchellNetravali(dy - n);
						green += float(p.g)*MitchellNetravali(m - dx)*MitchellNetravali(dy - n);

					}

				}
				
				red = (red <= 0) ? (0) : ((red >= 255) ? (255) : (red));
				blue = (blue <= 0) ? (0) : ((blue >= 255) ? (255) : (blue));
				green = (green <= 0) ? (0) : ((green >= 255) ? (255) : (green));
				//red = (int(red)+255) % 255;
				//blue = (int(blue)+255) % 255;
				//green = (int(green)+255) % 255;
			//	red = unsigned char(red);
				//green = unsigned char(green);
				//blue = unsigned char(blue);
				Pixel p2(red, green, blue);

				p1 = p2;
			////	Pixel p2 = PixelLerp(PixelLerp(CheckIfRange(x, y), CheckIfRange(x + 1, y), pxFracPart),
					//PixelLerp(CheckIfRange(x, y + 1), CheckIfRange(x + 1, y + 1), pxFracPart), pyFracPart);

				//Pixel p2 = this->GetPixel(px, py);

				//p1 = p2;








			}


		}
		return newImg;









	}

  return NULL ;
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}
void Image::Shift(double sx, double sy)
{
	/* Your Work Here (Section 3.5.2) */

	Image* newImg = new Image(width, height);

	//if sx is int
	if (ceil(sx) == sx && ceil(sy) == sy)
	{

		//both are ints
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				newImg->GetPixel(i, j) = CheckIfRange(i - sx, j - sy);

			}


		}

	}
	else //double/int combo
	{

		if (sampling_method == IMAGE_SAMPLING_POINT) {
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					newImg->GetPixel(i, j) = CheckIfRange(floor(i - sx), floor(j - sy));

				}


			}

		}

		else if (sampling_method == IMAGE_SAMPLING_HAT) {
			// Your work here

			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{

					int x = floor(sx);
					int y = floor(sy);

					float xFracPart = x - sx;
					float yFracPart = y - sy;

					if (xFracPart < 0)
					{
						xFracPart += 1;
						x = x - 1;
					}
					if (yFracPart < 0)
					{
						yFracPart += 1;
						y = y - 1;
					}

					int xShift = i - x;
					int yShift = j - y;

					//shift first by integer part
					Pixel& p1 = newImg->GetPixel(i, j) = CheckIfRange(xShift, yShift);



					//hat filter bilinear interpolation shift	
					p1 = PixelLerp(PixelLerp(CheckIfRange(xShift, yShift), CheckIfRange(xShift + 1, yShift), xFracPart),
						PixelLerp(CheckIfRange(xShift, (yShift + 1)), CheckIfRange(xShift + 1, yShift + 1), xFracPart), yFracPart);


				}


			}




		}

		else if (sampling_method == IMAGE_SAMPLING_MITCHELL) {
			// Your work here

			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{

					int x = floor(sx);
					int y = floor(sy);

					float dx = x - sx;
					float dy = y - sy;

					if (dx < 0)
					{
						dx += 1;
						x = x - 1;
					}
					if (dy < 0)
					{
						dy += 1;
						y = y - 1;
					}
					
					int xShift = i - x;
					int yShift = j - y;

					//shift first by integer part
					Pixel& p1 = newImg->GetPixel(i, j) = CheckIfRange(xShift, yShift);


					
					float red = 0;
					float green = 0;
					float blue = 0;

					for (int m = -1; m <= 2; m++)
					{
						for (int n = -1; n <= 2; n++)
						{
							//Pixel p = CheckIfRange(i + m, j + n);
							Pixel p = CheckIfRange(xShift + m, yShift + n);
							red += float(p.r)*MitchellNetravali(m - dx)*MitchellNetravali(dy - n);
							blue += float(p.b)*MitchellNetravali(m - dx)*MitchellNetravali(dy - n);
							green += float(p.g)*MitchellNetravali(m - dx)*MitchellNetravali(dy - n);

						}

					}

					red = (red <= 0) ? (0) : ((red >= 255) ? (255) : (red));
					blue = (blue <= 0) ? (0) : ((blue >= 255) ? (255) : (blue));
					green = (green <= 0) ? (0) : ((green >= 255) ? (255) : (green));

					Pixel p2(red, green, blue);


					

					//hat filter bilinear interpolation shift	
					p1 = p2;


				}


			}


		}


	}
	this->pixels = newImg->pixels;

}



Image* Image::Rotate(double angle)
{
  // For rotation of the image
 

	Image* newImg = new Image(height, width);
	
	for (int i = 0; i < width; i++)
	{

		for (int j = 0; j < height; j++)
		{

			
			Pixel p = this->CheckIfRange(i,j);
			newImg->GetPixel(i, height-j-1 ) = p;


		}

	}



    return newImg;
}



void Image::Fun()
{
    
}


Image* ImageMorph (Image* I0, Image* I1, int numLines, Line* L0, Line* L1, double t)
{
  

    return NULL;
}


/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
  // Sets the filter to use for Scale and Shift
  // You need to implement point sampling, hat filter and mitchell

    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}

Pixel Image::Sample (double u, double v, double sx, double sy)
{
 
  if (sampling_method == IMAGE_SAMPLING_POINT) {
    
  }

  else if (sampling_method == IMAGE_SAMPLING_HAT) {
    
  }

  else if (sampling_method == IMAGE_SAMPLING_MITCHELL) {
    
  }

  else {
    fprintf(stderr,"I don't understand what sampling method is used\n") ;
    exit(1) ;
  }

  return Pixel() ;
}

