
#include "LA4.h"

extern int* imageToMat(char* name, int* dims);
extern void matToImage(char* name, int* mat, int* dims);

int main( int argc, char** argv ) {

    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status stat;

    double startfull=MPI_Wtime();

    int *matrix;
    int *temp;
    char *name="image.jpg";
    int *dims = (int*) malloc(2*sizeof(int));
   
    //read image
    if(rank==0){
        printf("Reading image...\n");
        
        matrix=imageToMat(name,dims);
    }
    
    //broadcast dimensions and image
    double startbcast=MPI_Wtime();
    
    printf("Broadcasting image...\n");

    MPI_Bcast(dims,2,MPI_INT,0,MPI_COMM_WORLD);

    /* refactor later
        int height=dims[0];
        int width=dims[1];
    */

    if(rank!=0) matrix = (int*) malloc(dims[0]*dims[1]*sizeof(int));

    MPI_Bcast(matrix,dims[0]*dims[1],MPI_INT,0,MPI_COMM_WORLD);

    double endbcast=MPI_Wtime();

    //initalize ranks and calculate convolution
    double startcalc=MPI_Wtime();

    printf("Calculating convolution...\n");

    //check for proper number of processes
    if(dims[0]%numranks !=0){
        if(rank==0) printf("Error: Height does not divide evenly by number of processes.\n");
        free(dims);
        free(matrix);
        MPI_Finalize();
        return 0;
    }


    int height=dims[0];
    int width=dims[1];  
    int numrows=dims[0] / numranks;

    int myRowStart=rank*numrows;
    int myRowEnd=(rank+1)*numrows;
    if(rank==numranks-1) myRowEnd=dims[0];

    

    //create kernel size history: 51
    int size=3;
    int range=size/2;
    int k=range;//consider removing later, but it makes the code more readable
    double *gKernel[size];
    for (int i=0;i<size;i++){
        gKernel[i]=(double*)malloc(size*sizeof(double));
    }
    
    //fill kernel
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            gKernel[i][j]=1.0/(size*size);
        }
    }

    temp=(int*)malloc(numrows*width*sizeof(int));

    //pick a pixel (i,j)
    for(int i=myRowStart;i<myRowEnd;i++){
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
    
    

    double endcalc=MPI_Wtime();
    

    double startgather=MPI_Wtime();

    printf("Gathering results...\n");

    //mpi gather
    MPI_Gather(temp,numrows*width,MPI_INT,matrix,numrows*width,MPI_INT,0,MPI_COMM_WORLD);
    //save image
    if(rank==0) matToImage("processedImage.jpg",matrix,dims);

    double endgather=MPI_Wtime();

    //cleanup
    double endfull=MPI_Wtime();

    printf("Number of Ranks: %d\n",numranks);
    printf("Full time: %f\n",endfull-startfull);
    printf("Bcast time: %f\n",endbcast-startbcast);
    printf("Calc time: %f\n", endcalc-startcalc);
    printf("Gather time: %f\n",endgather-startgather);  

    free(dims);
    free(matrix);
    free(temp);
    for(int i=0; i<size; i++) free(gKernel[i]);

    MPI_Finalize();

    return 0;
}
