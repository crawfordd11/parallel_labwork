#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

extern void matToImage(char* filename, int* mat, int* dims);
extern void matToImageColor(char* filename, int* mat, int* dims);

int main(int argc, char **argv){
    int nx=600*200;
    int ny=400*200;
    int* matrix=(int*)malloc(nx*ny*sizeof(int));

    int maxIter=255;
    double xStart=-2;
    double xEnd=1;
    double yStart=-1;
    double yEnd=1;
    //C=x0+iy0
    double x0;
    double y0;
    //Z=x+iy
    double x=0;
    double y=0;

    int idx,iter;

    double start = omp_get_wtime(); 

    #pragma omp parallel for private(x0,y0,x,y,iter,idx) schedule(dynamic)
    for(int i=0;i<ny;i++){
        for(int j=0;j<nx;j++){
            idx=i*nx+j;
            //convert from pixel to world space
            x0=xStart+(1.0*j/nx)*(xEnd-xStart);
            y0=yStart+(1.0*i/ny)*(yEnd-yStart);

            x=0;
            y=0;
            iter=0;
            while(iter<maxIter){
                iter++;
                //do z=z^2+c
                double temp=x*x-y*y+x0;
                y=2*x*y+y0;
                x=temp;

                //check the value
                if(x*x+y*y>4){
                    break;
                }

            }
       
            matrix[idx]=iter;

        }
    }

    int dims[2];
    dims[0]=ny;
    dims[1]=nx;
    
    double end = omp_get_wtime(); 
    printf("Work took %f seconds\n", end - start);
    printf("Number of threads: %d\n", omp_get_max_threads());
    printf("Image dimensions: %d x %d\n", nx, ny);

    matToImage("mandelbrot.jpg", matrix, dims);
    free(matrix);
    return 0;
}

