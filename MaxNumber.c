#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int main(int argc , char * argv[])
{
int my_rank; /* rank of process */
int p; /* number of process */
int source; /* rank of sender */
int dest; /* rank of reciever */
int tag = 0; /* tag for messages */
MPI_Status status; /* return status for */
/*Size of array and array*/
int *array = NULL;
int *subArray = NULL;
int sizeA;
/* recieve */

/* Start up MPI */
MPI_Init( &argc , &argv );
/* Find out process rank */
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
/* Find out number of process */
MPI_Comm_size(MPI_COMM_WORLD, &p);

/*Master Process*/
if( my_rank == 0)
{
    printf("Hello from master process.\n");
    printf("Number of slave processes is %d \n", p - 1);

    /*Get Size and elements of array from User*/
    printf("Please enter size of array...\n");
    scanf("%d", &sizeA);
    array = malloc(sizeA * sizeof(int));
    printf("Please enter array elements ...\n");
    for (int i = 0; i < sizeA; i++)
    {
        scanf("%d",&array[i]);
    }

    /* Send */
    /* Calculate sizes for each process */
    int *sizes = (int*)malloc(p * sizeof(int));
    int *offsets = (int*)malloc(p * sizeof(int));
    int startIndex = 0;

    for (int i = 1; i < p; i++)
    {
        sizes[i] = sizeA / (p -1); 
        if (i <= sizeA % (p - 1))
            sizes[i]++;
        offsets[i] = startIndex;
        startIndex += sizes[i];
    }
    sizes[0] = 0;

    

    for (dest = 1; dest < p; dest++)
    {
        int sizeOfSubarray = sizes[dest];

         /* Send size, starting index, and array portion */
        MPI_Send( &sizeOfSubarray, 1, MPI_INT, dest, tag,MPI_COMM_WORLD);


        MPI_Send(&offsets[dest], 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            

        MPI_Send( &array[offsets[dest]], sizeOfSubarray, MPI_INT, dest, tag,MPI_COMM_WORLD);


    }
    int maxNumber = array[0];
    int index = 0;

    for (int i = 1; i < sizes[0]; i++) {
        if (array[i] > maxNumber) {
            maxNumber = array[i];
            index = i;
        }
    }
    /*Receive*/
    for (source = 1; source < p; source++)
    {
        int subMax ,subIndex ;

        MPI_Recv(&subMax, 1, MPI_INT, source, tag, MPI_COMM_WORLD,&status);
        
        MPI_Recv(&subIndex, 1, MPI_INT, source, tag, MPI_COMM_WORLD,&status);

        /*Calculate max number*/
        if(subMax > maxNumber){
            maxNumber = subMax;
            index = subIndex;
        }
    }

    printf("Master process announce the final max which is %d and its index is %d.\n",maxNumber,index);
    printf("Thanks for using our program\n");
    free(array);
    free(sizes);
    free(offsets);

/* Slave Process */
}else{
/* Recieve  from master*/
        int sizeOfSubarray, startIndex;
        /* Size and startindex*/
        MPI_Recv( &sizeOfSubarray, 1, MPI_INT, 0, tag,MPI_COMM_WORLD,&status);

        MPI_Recv(&startIndex, 1, MPI_INT, 0, tag, MPI_COMM_WORLD,&status);

        /* Receives the portion of the array.*/
        subArray = malloc(sizeOfSubarray * sizeof(int));

        MPI_Recv( subArray, sizeOfSubarray, MPI_INT, 0, tag,MPI_COMM_WORLD,&status);

        /*calculate sub max number  for each sub array */
        int subMax = subArray[0];
        int subIndex = startIndex;
        

        for (int i = 1; i < sizeOfSubarray; i++)
        {
            if(subArray[i]> subMax ){
                subMax = subArray[i];
                subIndex = startIndex + i;
            }
        }

        printf("Hello from slave# %d Max number in my partition is %d and index is %d.\n",my_rank,subMax,subIndex - startIndex);


/*      Sends max number and its index back to master process.  */
        MPI_Send( &subMax, 1, MPI_INT, 0, tag,MPI_COMM_WORLD);


        MPI_Send(&subIndex, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

        free(subArray);
        
}


/* shutdown MPI */
/*printf("Thanks for using our program\n");*/
MPI_Finalize();

return 0;
}