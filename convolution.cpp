#include <cmath>
#include "convolution.h"

bool convolution(unsigned char* in, unsigned char* out, int xSize, int ySize, 
                float* kernel, int kernelX, int kernelY)
{
	int i;
	int j;
	int m;
	int n;
	unsigned char *pointIn;
	unsigned char *pointInTemp;
	unsigned char *pointOut;
    float *pointKernel;
	int xC;
	int yC;
	int minRow;
	int maxRow;                           
	int minCol;
	int maxCol;                           
    float sum;  

    // Finding the center of the Kernel
    xC = kernelX >> 1;
    yC = kernelY >> 1;

    // Finding the pointer values for the Kernel
    pointIn = pointInTemp = &in[xSize * yC + xC];
    pointOut = out;
    pointKernel = kernel;

    // The actual convolution mathematics
    for(i= 0; i < ySize; ++i) // i is the number of rows
    {
        maxRow = i + yC;
        minRow = i - ySize + yC;

        for(j = 0; j < xSize; ++j) // j is the number of columns
        {
            maxCol = j + xC;
            minCol = j - xSize + xC;
            sum = 0;

			// Flipping the kernel and doing multiplication
            for(m = 0; m < kernelY; ++m) // m is the number of kernel rows
            {
                if(m <= maxRow && m > minRow)
                {
                    for(n = 0; n < kernelX; ++n)
                    {
                        if(n <= maxCol && n > minCol)
                            sum += *(pointIn - n) * *pointKernel;

                        ++pointKernel; 
                    }
                }
                else
                    pointKernel += kernelX;

                pointIn -= xSize; 
            }

            // Converts the - numbers to + numbers (can't have a - brightness value!)
            *pointOut = (unsigned char)((float)fabs(sum) + 0.5f);
            pointKernel = kernel;         
            pointIn = ++pointInTemp;      
            ++pointOut;             
        }
    }

    return true;
}
