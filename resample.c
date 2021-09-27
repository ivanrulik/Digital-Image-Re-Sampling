/*
Re Sample Images
Ivan Alexander Rulik Cote
09/18/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc/malloc.h>  
#include <memory.h>

#define max(x, y) ((x>y) ? (x):(y))
#define min(x, y) ((x<y) ? (x):(y))


int xdim;
int ydim;
int maxraw;
unsigned char *image;
unsigned char *imageOut;

/*
New variables defined for the project
*/

float t; // escale factor modified by the user IDEA: make it the 4th argument for an easier use
int xdimOut; // place holder for the new dimension of the output image
int ydimOut; // place holder for the new dimension of the output image

void ReadPGM(FILE*);
void WritePGM(FILE*);
/* This new fuction will take a given file and apply Bilinear Interpolation to it */
void BilinearInterpolation(FILE*, float t);


int main(int argc, char **argv)
{
  int i, j;
  FILE *fp;

  if (argc != 4){
    printf("Usage: MyProgram <input_ppm> <output_ppm> \n");
    printf("       <input_ppm>: PGM file \n");
    printf("       <output_ppm>: PGM file \n");
    exit(0);              
  }
  /* This code reads the escaling factor as an argument and changes its type from char*
  to float using the function atof(), then it prints the value to give feedback */
  t = atof(argv[3]);
  printf("Escale Factor = %3.2f \n",t);

  /* begin reading PGM.... */
  printf("begin reading PGM.... \n");
  if ((fp=fopen(argv[1], "rb"))==NULL){
    printf("read error...\n");
    exit(0);
  }
  ReadPGM(fp);
 
  
  /* Begin writing PGM.... */
  printf("Begin writing PGM.... \n");
  if ((fp=fopen(argv[2], "wb")) == NULL){
     printf("write pgm error....\n");
     exit(0);
   }
  
  /* Calculate the new dimension of the output image by multiplying the 
  scale factor and then rounding it to the closest integer */
  xdimOut = round(t*xdim); 
  ydimOut = round(t*ydim);
  printf("Out Width=%d, Out Height=%d \n",xdimOut,ydimOut);

  BilinearInterpolation(fp,t);
  WritePGM(fp);

  free(image);

  return (1);
}



void ReadPGM(FILE* fp)
{
    int c;
    int i,j;
    int val;
    unsigned char *line;
    char buf[1024];


    while ((c=fgetc(fp)) == '#')
        fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "P%d\n", &c) != 1) {
       printf ("read error ....");
       exit(0);
     }
     if (c != 5 && c != 2) {
       printf ("read error ....");
       exit(0);
     }

     if (c==5) {
       while ((c=fgetc(fp)) == '#')
         fgets(buf, 1024, fp);
       ungetc(c, fp);
       if (fscanf(fp, "%d%d%d",&xdim, &ydim, &maxraw) != 3) {
         printf("failed to read width/height/max\n");
         exit(0);
       }
       printf("Width=%d, Height=%d \nMaximum=%d\n",xdim,ydim,maxraw);

       image = (unsigned char*)malloc(sizeof(unsigned char)*xdim*ydim);
       getc(fp);

       line = (unsigned char *)malloc(sizeof(unsigned char)*xdim);
       for (j=0; j<ydim; j++) {
          fread(line, 1, xdim, fp);
          for (i=0; i<xdim; i++) {
            image[j*xdim+i] = line[i];
         }
       }
       free(line);

     }

     else if (c==2) {
       while ((c=fgetc(fp)) == '#')
         fgets(buf, 1024, fp);
       ungetc(c, fp);
       if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
         printf("failed to read width/height/max\n");
         exit(0);
       }
       printf("Width=%d, Height=%d \nMaximum=%d,\n",xdim,ydim,maxraw);

       image = (unsigned char*)malloc(sizeof(unsigned char)*xdim*ydim);
       getc(fp);

       for (j=0; j<ydim; j++)
         for (i=0; i<xdim; i++) {
            fscanf(fp, "%d", &val);
            image[j*xdim+i] = val;
         }

     }

     fclose(fp);
}


void WritePGM(FILE* fp)
{
  int i,j;
  

  fprintf(fp, "P5\n%d %d\n%d\n", xdimOut, ydimOut, 255);
  for (j=0; j<ydimOut; j++)
    for (i=0; i<xdimOut; i++) {
      fputc(imageOut[j*xdimOut+i], fp);
    }

  fclose(fp);
  
}

void BilinearInterpolation(FILE* fp, float t)
{
  int i,j;
  int T1, T2;
  int A[2],B[2],C[2],D[2];
  float x,y;
  float distX, distY;

  imageOut = (unsigned char*)malloc(sizeof(unsigned char)*xdimOut*ydimOut);
  for (j=0; j<ydimOut; j++)
  for (i=0; i<xdimOut; i++) {
    /* Visual representation of the loop to sample the 
    new pixel intensities based on the original image
    A##########B
    |##########|
    -####ij####-
    T1#########|
    |#########T2
    C##########D
    */
    x=i/t; // current position of the sampled image mapped to the orginal on the x axis
    y=j/t; // current position of the sampled image mapped to the orginal on the y axis
    A[0]=floor(x); // left top corner neighbor x
    A[1]=floor(y)+1; // left top corner neighbor y
    B[0]=floor(x)+1; // right top corner neighbor x
    B[1]=floor(y)+1; // right top corner neighbor y
    C[0]=floor(x); // left botton corner neighbor x
    C[1]=floor(y); // left botton corner neighbor y
    D[0]=floor(x) + 1; // right bottom corner neighbor x
    D[1]=floor(y); // right bottom corner neighbor y
    distX=x-floor(x); // x distance from current pixel to bottom left corner
    distY=y-floor(y); // y distance from current pixel to bottom left corner
    T1 = distY*image[(A[1]*xdim)+A[0]] + (1-distY)*image[(C[1]*xdim)+C[0]]; // y distance of the (i,j) pixel to the left lower edge corner and theleft top neighbor
    T2 = distY*image[(B[1]*xdim)+B[0]] + (1-distY)*image[(D[1]*xdim)+D[0]]; // x distance of the (i,j) pixel to the left lower edge corner
    imageOut[j*xdimOut+i] = distX*T2 + (1-distX)*T1;
  }
}
