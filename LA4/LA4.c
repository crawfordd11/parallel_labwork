#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern int* imageToMat(char* name, int* dims);
extern void matToImage(char* name, int* mat, int* dims);

int main( int argc, char** argv ) {
    int *matrix;
    int *temp;
    char *name="image.jpg";
    int *dims;
    dims=(int*) malloc(2*sizeof(int));

    //read image
    matrix=imageToMat(name,dims);
    int height=dims[0];
    int width=dims[1];

    int k=7;
    temp=(int*)malloc(height*width*sizeof(int));

    //pick a pixel (i,j)
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            int index=i*width+j;
            //setup for the convolution
            double sum=0;
            int counter=0;
            for(int u=-k;u<=k;u++){
                for(int v=-k;v<=k;v++){
                    if(i-u<0 || j-v<0 || i-u>=height ||j-v>=width) continue;
                    sum+=1*matrix[(i-u)*width+(j-v)];
                    counter++;
                }
            }
            temp[index]=(int)(sum/counter);
        }
    }
    

    //save image
    matToImage("processedImage.jpg",temp,dims);

    free(dims);
    free(matrix);
    free(temp);
    return 0;
}
