#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <mpi.h>

extern void matToImage(char* filename, int* mat, int* dims);

int main(int argc, char **argv){
	int nx=600; //cols
	int ny=400; //rows
	int maxiter=255;
	int *mat;
	double area,i_c,r_c,i_z,r_z;
	double r_start,r_end,i_start,i_end;

	int numoutside=0;
	int iter;
	//vars for window size 3:2 ratio
	r_start=-2;
	r_end=1;
	i_start=-1;
	i_end=1;

	mat=(int*)malloc(nx*ny*sizeof(*mat));


	for(int i=0;i<ny;i++){       //rows
		for (int j=0;j<nx;j++){  //cols
			i_c=i_start+i/(ny*1.0)*(i_end-i_start);
			r_c=r_start+j/(nx*1.0)*(r_end-r_start);
			i_z=i_c;
			r_z=r_c;
			iter=0;
			while(iter<maxiter){
				iter=iter+1;
				double r_t=r_z*r_z-i_z*i_z;
				double i_t=2.0*r_z*i_z;	
				i_z=i_t+i_c;
				r_z=r_t+r_c;
				if(r_z*r_z+i_z*i_z > 4){
					numoutside=numoutside+1;
					break;
				}
			}
			mat[i*nx+j]=iter;
		}
	}


	area=(r_end-r_start)*(i_end-i_start)*(1.0*nx*ny-numoutside)/(1.0*nx*ny);

	printf("Area of Mandelbrot set = %f\n",area);
	int dims[2] = {ny,nx};
	// matToImage("image.jpg",mat,dims);
}
