#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <mpi.h>

extern void matToImage(char* filename, int* mat, int* dims);

int main(int argc, char **argv){

	int rank, numranks;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numranks);

	MPI_Barrier(MPI_COMM_WORLD);
	double start = MPI_Wtime();

	int *full_matrix = NULL;
	int *recvcounts = NULL;
	int *displs = NULL;

	int size_modifier=5*6*7*8*9;//enough prime factors to cover first ten ranks without breaking the size of int later on
	int nx=3*size_modifier; //cols
	int ny=2*size_modifier; //rows
	int maxiter=255;
	int *local_mat;
	double area,i_c,r_c,i_z,r_z;
	double r_start,r_end,i_start,i_end;

	int global_outside = 0;
	int numoutside=0;
	int iter;
	//vars for window size 3:2 ratio
	r_start=-2;
	r_end=1;
	i_start=-1;
	i_end=1;

	// Master process initializes recvcounts and displs for gathering results
	if (rank == 0) {
		full_matrix = malloc((size_t)nx * ny * sizeof(int));
		recvcounts = malloc(numranks * sizeof(int));
		displs = malloc(numranks * sizeof(int));
	}

	int base = ny / numranks;
	int rem = ny % numranks;

	int local_delta = base + (rank < rem ? 1 : 0);
	int i_start_rank = rank * base + (rank < rem ? rank : rem);
	int i_end_rank = i_start_rank + local_delta;
	int local_count = local_delta * nx;

	local_mat=(int*)malloc(local_count*sizeof(*local_mat));

	if (!local_mat) {
		printf("Rank %d: malloc failed\n", rank);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	MPI_Gather(&local_count, 1, MPI_INT,
			recvcounts, 1, MPI_INT,
			0, MPI_COMM_WORLD);

	if (rank == 0) {
		displs[0] = 0;
		for (int i = 1; i < numranks; i++) {
			displs[i] = displs[i-1] + recvcounts[i-1];
		}
	}		

	

	#pragma omp parallel for collapse(2) private(i_c,r_c,i_z,r_z,iter) reduction(+:numoutside) schedule(dynamic)
	for(int i=i_start_rank;i<i_end_rank;i++){ //rows
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
			local_mat[(i-i_start_rank)*nx+j]=iter;
		}
	}

	MPI_Reduce(&numoutside, &global_outside, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	MPI_Gatherv(local_mat, local_delta * nx, MPI_INT,
            full_matrix, recvcounts, displs, MPI_INT,
            0, MPI_COMM_WORLD);

	if(rank == 0){
		area = (r_end - r_start)*(i_end - i_start) * (1.0*nx*ny - global_outside)/(1.0*nx*ny);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double end = MPI_Wtime();
	
	if(rank==0){
		printf("Area of Mandelbrot set = %f\n",area);
		printf("Full work took %f seconds\n", end - start);
		printf("Number of threads: %d\n", omp_get_max_threads());
		printf("Number of ranks: %d\n", numranks);
		printf("Image dimensions: %d x %d\n", nx, ny);
		int dims[2] = {ny,nx};
		matToImage("mandelbrot.jpg",full_matrix,dims);
	}

	//cleanup
	free(local_mat);

	if(rank == 0){
		free(full_matrix);
		free(recvcounts);
		free(displs);
	}

	MPI_Finalize();

}
