/ Copyright(C) 2018 Iti Shree



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <highgui.h>

/* Constant declaration */
#define MAX_IMAGESIZE  1024
#define MAX_BRIGHTNESS  255 /* Maximum gray level */
#define GRAYLEVEL       256 /* No. of gray levels */
#define MAX_FILENAME    256 /* Filename length limit */
#define MAX_BUFFERSIZE  256

/* Global constant declaration */
/* Image storage arrays */
unsigned char image1[MAX_IMAGESIZE][MAX_IMAGESIZE],
  image2[MAX_IMAGESIZE][MAX_IMAGESIZE];
int x_size1, y_size1, /* width & height of image1*/
  x_size2, y_size2; /* width & height of image2 */

/* Prototype declaration of functions */
void load_image_file( ); /* image input */
void save_image_file( ); /* image output*/

void load_image_file(char *filename)
/* Input of header & body information of pgm file */
/* for image1[ ][ ]，x_size1，y_size1 */
{
  char buffer[MAX_BUFFERSIZE];
  FILE *fp; /* File pointer */
  int max_gray; /* Maximum gray level */
  int x, y; /* Loop variable */

  /* Input file open */
  fp = fopen(filename, "rb");
  if (NULL == fp) {
    printf("     The file doesn't exist!\n\n");
    exit(1);
  }
  /* input of x_size1, y_size1 */
  x_size1 = 0;
  y_size1 = 0;
  while (x_size1 == 0 || y_size1 == 0) {
    fgets(buffer, MAX_BUFFERSIZE, fp);
    if (buffer[0] != '#') {
      sscanf(buffer, "%d %d", &x_size1, &y_size1);
    }
  }
  /* input of max_gray */
  max_gray = 0;
  while (max_gray == 0) {
    fgets(buffer, MAX_BUFFERSIZE, fp);
    if (buffer[0] != '#') {
      sscanf(buffer, "%d", &max_gray);
    }
  }
  if (x_size1 > MAX_IMAGESIZE || y_size1 > MAX_IMAGESIZE) {
    printf("     Image size exceeds %d x %d\n\n",
	   MAX_IMAGESIZE, MAX_IMAGESIZE);
    printf("     Please use smaller images!\n\n");
    exit(1);
  }
  if (max_gray != MAX_BRIGHTNESS) {
    printf("     Invalid value of maximum gray level!\n\n");
    exit(1);
  }
  /* Input of image data*/
  for (y = 0; y < y_size1; y++) {
    for (x = 0; x < x_size1; x++) {
      image1[y][x] = (unsigned char)fgetc(fp);
    }
  }
  fclose(fp);
}
void save_image_file(char *filename)
/* Output of image2[ ][ ], x_size2, y_size2 */
{
  FILE *fp; /* File pointer */
  int x, y; /* Loop variable */

  fp = fopen(filename, "wb");
  /* output of file header information */
  fputs("P5\n", fp);
  fputs("# Created by Image Processing\n", fp);
  fprintf(fp, "%d %d\n", x_size2, y_size2);
  fprintf(fp, "%d\n", MAX_BRIGHTNESS);
  /* Output of image data */
  for (y = 0; y < y_size2; y++) {
    for (x = 0; x < x_size2; x++) {
      fputc(image2[y][x], fp);
    }
  }
  fclose(fp);
}
void make_histogram_image( )
/* Histogram of image1 is output into image2 */
{
  long int histogram[GRAYLEVEL];
  long int max_frequency; /* Maximum frequency */
  int i, j, x, y; /* control variable */
  int height; /* height of histogram */
  int top; /* the most frequent graylevel */

  /* Calculation of histogram */
  for (i = 0; i < GRAYLEVEL; i++) {
    histogram[i] = 0;
  }
  printf("Now, histogram of input image is calculated\n\n");
  for (y = 0; y < y_size1; y++) {
    for (x = 0; x < x_size1; x++) {
      histogram[image1[y][x]]++;
    }
  }
  /* calculation of maximum frequency */
  max_frequency = histogram[0];
  top = 0;
  for (i = 1; i < GRAYLEVEL; i++) {
    if (histogram[i] > max_frequency) {
      max_frequency = histogram[i];
      top = i;
    }
  }
  printf("Maximum frequency = %d at level of %d\n\n", max_frequency, top);
  /* Histogram image generation */
  x_size2 = IMAGESIZE;
  y_size2 = IMAGESIZE;
  for (y = 0; y < y_size2; y++) {
    for (x = 0; x < x_size2; x++) {
      image2[y][x] = 0;
    }
  }
  for (i = 0; i < GRAYLEVEL; i++) {
    height = (int)(MAX_BRIGHTNESS /
		   (double)max_frequency * (double)histogram[i]);
    for (j = 0; j < height; j++) {
      image2[IMAGESIZE-1-j][i] = MAX_BRIGHTNESS;
    }
  }
}

int main(int argc, char *argv[])
{

  load_image_data( );      /* Input of image1 */
  make_histogram_image( ); /* Histogram generation */
  save_image_data( );      /* Output of image2 */
  return 0;
}

