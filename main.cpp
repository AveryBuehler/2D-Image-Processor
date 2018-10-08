#include <cstdio>
#include <cstdlib>
#include <glut.h>
#include "convolution.h"
#include <cstring>

// Image data for openGL
struct ImageData
{
    GLint   x;      // X-Resolution of image
    GLint   y;      // Y-Resolution of image
    GLenum  format; // Monochrome
    GLenum  type;   // 8-bit Grayscale
    GLvoid  *buf;   // Image pixels
};

// GLUT CALLBACK functions
void mainDisplay();
void originalDisplay();
void convolutionDisplay();
void reformat(int w, int h);

void initGL();
int  initGLUT(int argc, char **argv);
bool memoryShare();
bool rawLoad(char *fileName, int x, int y, unsigned char *data);

// Image information
const char *FILE_NAME = "lena.raw"; //Images {peppers.raw, baboon.raw, cameraman.raw, lena.raw}
const int  xRes = 256;
const int  yRes = 256;
const int  maxRes = 1024;

// Initializing variables for use
ImageData *image;
unsigned char *inBuf;
unsigned char *tempBufferOne;
unsigned char *tempBufferTwo;
char fileName[maxRes];
int imageX;
int imageY;
int mainWindow;
int originalPic;
int convolutionPic;

int main(int argc, char **argv)
{
        strcpy(fileName, FILE_NAME);
        imageX = xRes;
        imageY = yRes;
   
	// Allocating memory for new variables
	if (!memoryShare())
	{
		return 0;
	}

    // Opening the RAW image file
    if(!rawLoad(fileName, imageX, imageY, inBuf))
    {
        return 0;
    }

	// A 3x3 Edge Detection Kernel
	//float kernel[9] = {-1, -1, -1,
					//   -1,  8, -1
					 //  -1, -1, -1};

    // A 5x5 Gaussian Blur Kernel
    float kernel[25] = { 1/256.0f,  4/256.0f,  6/256.0f,  4/256.0f, 1/256.0f,
                         4/256.0f, 16/256.0f, 24/256.0f, 16/256.0f, 4/256.0f,
                         6/256.0f, 24/256.0f, 36/256.0f, 24/256.0f, 6/256.0f,
                         4/256.0f, 16/256.0f, 24/256.0f, 16/256.0f, 4/256.0f,
                         1/256.0f,  4/256.0f,  6/256.0f,  4/256.0f, 1/256.0f};

	// A 5x5 Unsharp Mask Kernel
	//float kernel[25] = {-1 / 256.0f,  -4 / 256.0f,  -6 / 256.0f,  -4 / 256.0f, -1 / 256.0f,
						//-4 / 256.0f, -16 / 256.0f, -24 / 256.0f, -16 / 256.0f, -4 / 256.0f,
						//-6 / 256.0f, -24 / 256.0f, -476 / 256.0f, -24 / 256.0f, -6 / 256.0f,
						//-4 / 256.0f, -16 / 256.0f, -24 / 256.0f, -16 / 256.0f, -4 / 256.0f,
						//-1 / 256.0f,  -4 / 256.0f,  -6 / 256.0f,  -4 / 256.0f, -1 / 256.0f};

    convolution(inBuf, tempBufferOne, imageX, imageY, kernel, 5, 5);

	// Calling upon OpenGL functions to display
    mainWindow = initGLUT(argc, argv);
    glutDisplayFunc(mainDisplay);
    glutReshapeFunc(reformat);           
    originalPic = glutCreateSubWindow(mainWindow, 0, 0, imageX, imageY);
    glutDisplayFunc(originalDisplay);
    convolutionPic = glutCreateSubWindow(mainWindow, imageX, 0, imageX, imageY);
    glutDisplayFunc(convolutionDisplay);
    glutMainLoop(); 

    return 0;
}

bool memoryShare()
{
    image = new ImageData;

    inBuf = new unsigned char[imageX * imageY];
    tempBufferOne = new unsigned char[imageX * imageY];
    tempBufferTwo = new unsigned char[imageX * imageY];

    image->x = imageX;
    image->y = imageY;
    image->format = GL_LUMINANCE;
    image->type = GL_UNSIGNED_BYTE;
    image->buf = (GLvoid*)inBuf;

    return true;
}


bool rawLoad(char *fileName, int x, int y, unsigned char *data)
{
    if(!fileName || !data)
        return false;

    FILE *fp;
    if((fp = fopen(fileName, "r")) == NULL)
    {
        return false;
    }

    fread(data, 1, x*y, fp);
    fclose(fp);

    return true;
}

int initGLUT(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); 
    glutInitWindowSize(2*imageX, imageY);
    glutInitWindowPosition(100, 100);
    int handle = glutCreateWindow(argv[0]);
    return handle;
}

// Basic function to initialize OpenGL
void initGL()
{
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_FLAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_LOGIC_OP);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
}

// Call back functions
void mainDisplay()
{
    glutSetWindow(mainWindow);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

void originalDisplay(void)
{
    glutSetWindow(originalPic);
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, imageY);
    glPixelZoom(1.0, -1.0);
    glDrawPixels(image->x, image->y, image->format, image->type, image->buf);
    glPixelZoom(1.0, 1.0);
    glutSwapBuffers();
}

void convolutionDisplay()
{
    glutSetWindow(convolutionPic);
    glClear(GL_COLOR_BUFFER_BIT); 
    glRasterPos2i(0, imageY);
    glPixelZoom(1.0, -1.0);
    glDrawPixels(image->x, image->y, image->format, image->type, tempBufferOne);
    glPixelZoom(1.0, 1.0);
    glutSwapBuffers();
}

// Display the window with the two images (original and convoluted)
void reformat(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glutSetWindow(originalPic);
    glutPositionWindow(0,0);
    glutReshapeWindow(imageX, imageY);
    glViewport(0, 0, imageX, imageY);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, imageX, 0, imageY);
    glutSetWindow(convolutionPic);
    glutPositionWindow(imageX, 0);
    glutReshapeWindow(imageX, imageY);
    glViewport(0, 0, imageX, imageY);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, imageX, 0, imageY);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}