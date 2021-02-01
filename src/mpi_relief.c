
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "relief.h"

int number_amount;
MPI_Status status;

// ********************** MAIN ******************** //

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming at least 2 processes for this task
    if (world_size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

	int PNAMT[5];
	float *Samples;
	
    if (world_rank == 0) {
		// Read the data file
		FILE *fp;
		
		fp = fopen(argv[1],"r");
		
		fseek(fp,0,SEEK_SET);
		fscanf(fp, "%u\n%u\t%u\t%u\t%u", &PNAMT[0], &PNAMT[1], &PNAMT[2], &PNAMT[3], &PNAMT[4]);
		
		// Sending Configuration Data to Slaves
		for (int i = 1; i < PNAMT[0]; i ++)
			MPI_Send(
					/* data         = */ PNAMT,
					/* count        = */ 5,
					/* datatype     = */ MPI_INT,
					/* destination  = */ i,
					/* tag          = */ 0,
					/* communicator = */ MPI_COMM_WORLD);
		
		// Reading Features from File
		Samples = (float *) malloc(PNAMT[1] * (PNAMT[2] + 1));
		
		for (int i = 0; i < PNAMT[1]; i ++)
			for (int j = 0; j < PNAMT[2] + 1; j ++)
				fscanf(fp, "%f", &Samples[i * (PNAMT[2] + 1) + j]);
		
		fclose(fp);
		
 				
        // Sending Partitioned Features to Slaves
        for (int i = 1; i < PNAMT[0]; i ++){
			MPI_Send(
					/* data         = */ &Samples[(i-1) * (PNAMT[2]+1) * ((int) (PNAMT[1]+1) / (PNAMT[0]-1))],
					/* count        = */ (PNAMT[2]+1) * ((int) (PNAMT[1]+1) / (PNAMT[0]-1)),
					/* datatype     = */ MPI_FLOAT,
					/* destination  = */ i,
					/* tag          = */ 0,
					/* communicator = */ MPI_COMM_WORLD);
		}

	MPI_Status status;

	int *Result;
	Result = (int *) malloc(2 * PNAMT[4] * sizeof(int));
	
	for (int i = 1; i < PNAMT[0]; i ++){
	  
	  // Probe for an incoming message from process zero
	  MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
	  
	  // When probe returns, the status object has the size and other
	  // attributes of the incoming message. Get the message size
	  MPI_Get_count(&status, MPI_INT, &number_amount);
		
	  if (number_amount != PNAMT[4])
	   printf("\nslave %d receives %d but expected %d\n",world_rank,number_amount,PNAMT[4]);

	 MPI_Recv(
		  /* data         = */ &Result[(i-1) * PNAMT[4]],
		  /* count        = */ PNAMT[4],
		  /* datatype     = */ MPI_INT,
		  /* source       = */ i,
		  /* tag          = */ 0,
		  /* communicator = */ MPI_COMM_WORLD,
		  /* status       = */ MPI_STATUS_IGNORE);
	}

	qsort(Result, PNAMT[4], sizeof(int), intcmp);
	
	printf("Results : ");
	printf("%d ",Result[0]);
	for(int i = 1; i < 2 * PNAMT[4]; i++){
	  if (Result[i] == Result[i-1])
	    continue;
	  printf("%d ",Result[i]);
	}
	printf("\n");
	
	free(Samples);
	free(Result);

	// ----------------- SLAVES ------------------
	
    } else {        

        MPI_Recv(
                /* data         = */ PNAMT,
                /* count        = */ 5,     // Header of Text File
                /* datatype     = */ MPI_INT,
                /* source       = */ 0,
                /* tag          = */ 0,
                /* communicator = */ MPI_COMM_WORLD,
                /* status       = */ &status);
		
		int A = PNAMT[2];
		int N = (int) (PNAMT[1] + 1) / (PNAMT[0]-1);
		
		int receive_size = (N * (A+1));
				
		// Probe for an incoming message from process zero
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

		// When probe returns, the status object has the size and other
		// attributes of the incoming message. Get the message size
		MPI_Get_count(&status, MPI_FLOAT, &number_amount);
		
		if (number_amount != receive_size)
			printf("\nSlave %d receives %d but expected %d\n",world_rank,number_amount,receive_size);
			
		Samples = (float *) malloc(sizeof(float) * number_amount);
		
		MPI_Recv(
                /* data         = */ Samples,
                /* count        = */ number_amount,
                /* datatype     = */ MPI_FLOAT,
                /* source       = */ 0,
                /* tag          = */ 0,
                /* communicator = */ MPI_COMM_WORLD,
                /* status       = */ MPI_STATUS_IGNORE);

		//printf("Source\t:%d \nTag\t:%d \nError\t:%d \nCancell\t:%d \nSize\t:%d \n",status.MPI_SOURCE,status.MPI_TAG,status.MPI_ERROR,status._cancelled,status._ucount);
        
		int *Result;
		Result = (int *) malloc(PNAMT[4] * sizeof(int));

		Relief((const float *)Samples, (const int *)&PNAMT[1], Result);

		printf("Slave %d Results: ",world_rank);
		for(int i = 0; i < PNAMT[4]; i++)
		  printf("%d ",Result[i]);
		printf("\n");
		
		MPI_Send(
			 /* data         = */ Result,
			 /* count        = */ PNAMT[4],
			 /* datatype     = */ MPI_INT,
			 /* destination  = */ 0,
			 /* tag          = */ 0,
			 /* communicator = */ MPI_COMM_WORLD);

		free(Samples);
		free(Result);
    }

    MPI_Finalize();
}


