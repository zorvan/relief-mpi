#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
// ********************** Unique Random Number Generator ******************** //

void uniqrand(int n, int *array){
	int i = 0; 
	int j,r;

	srand(time(NULL));

	while (i < n) {
        r = rand() % n;
		
		for (j = 0; j < i; j++)
			if (array[j] == r)
				break;

		if (j == i)
			array[i++] = r;
	}
}

// ********************** Relief Algorithm ******************** //

float Max(const float *x,int n){
	float maximum = x[0];
	
	for (int i = 1; i< n; i++)
		if (x[i] > maximum)
			maximum = x[i];
			
	return maximum;		
}

// -------------------------------

float Min(const float *x,int n){
	float minimum = x[0];
	
	for (int i = 1; i< n; i++)
		if (x[i] < minimum)
			minimum = x[i];
			
	return minimum;
}

// -------------------------------

float Diff(int A,float *I1,float *I2,float maxA,float minA){
	if (maxA == minA)
		return 0;
	
	return abs(I1[A] - I2[A]) / (maxA - minA);
}

// -------------------------------

float ManhattanDistance(const float *x, float *y, int n){
	float distance = 0;
	
	for(int i = 0; i < n; i++)
		distance += abs(x[i]-y[i]);
	
	return distance;
}

// -------------------------------

int intcmp (const void *a, const void *b) {
   return ( *(int *)a - *(int *)b );
}

int floatcmp(const void* elem1, const void* elem2)
{
    if(*(const float*)elem1 < *(const float*)elem2)
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}

// -------------------------------

void Relief (const float *Instances,const int *namt, int *sel){
  
	const int ROWS = namt[0];
	const int COLUMNS = namt[1];
	const int COLUMNSPLUS = COLUMNS+1;
	const int ITERATIONS = namt[2];
	const int THRESHOLD = namt[3];

	int r, qr[ITERATIONS];
	
	float *R,*H,*M;
	float *W,*WW;
	float Mdist,Hdist;
	float maxA,minA;
	float d;

	
	srand(time(NULL));

	//qr  = (int *) malloc(COLUMNS * sizeof(int));
	W  = (float *) malloc(COLUMNS * sizeof(float));
	memset(W, 0, COLUMNS * sizeof(float));
	
	// Find maxA and minA
	maxA = Max(&Instances[0], COLUMNS);
	minA = Min(&Instances[0], COLUMNS);
	for (int i = 1; i < ROWS; i ++){
	  d = Max(&Instances[i * COLUMNSPLUS], COLUMNS);
		if(d > maxA)
			maxA = d;
		
		d = Min(&Instances[i * COLUMNSPLUS], COLUMNS);
		if(d < minA)
			minA = d;
	}

	uniqrand(ITERATIONS, qr); // m unique random number < n

	for (int i = 0; i < ITERATIONS; i ++){

	  r = qr[i];
	  R = (float *) &Instances[r * COLUMNSPLUS];
	  
		// Find H and M
		Mdist = Hdist = ROWS *  (maxA - minA);

		if(r)
		  H = M = (float *) &R[0];
		else
		  H = M = (float *) &R[COLUMNSPLUS];
		
		for (int j = 0; j < ROWS; j ++){
			if (j == r)
				continue;
			
			d = ManhattanDistance(&Instances[j * COLUMNSPLUS], R, COLUMNS);

			if (Instances[j * COLUMNSPLUS + COLUMNS] == R[COLUMNS]){ // Hit?
				if (d < Hdist){
					Hdist = d;
					H = (float *) &Instances[j * COLUMNSPLUS];
				}
			}
			else // Miss?
				if (d < Mdist){
					Mdist = d;
					M = (float *) &Instances[j * COLUMNSPLUS];
				}
		}

		for (int A = 0; A < COLUMNS - 1; A ++)
		  W[A] += (Diff(A,R,M,maxA,minA) - Diff(A,R,H,maxA,minA)) / ITERATIONS;
	}
	
	WW = (float *) malloc(COLUMNS * sizeof(float));
	memcpy(WW, W, COLUMNS * sizeof(float));
	qsort(WW, COLUMNS, sizeof(float), floatcmp);
	

	int flag;
	// find the best weight index
	for(int i = 0; i < THRESHOLD; i++)
	  for (int j = 0; j < COLUMNS; j++)
		if (W[j] == WW[COLUMNS-1-i]){ // new index?
		  flag = 1;
		  for (int k = 0; k <= i; k ++) // filter previously selected indexes
			if (sel[k] == j){ 
			  flag = 0;
			  break; // exit k for
			}
		  
		  if (flag){
			sel[i] = j;
			break; // exit j for
		  }
		}

	qsort(sel, THRESHOLD, sizeof(int), intcmp);
	
	free(W);
	free(WW);
}

// -------------------------------

void test(void){
	
	const int N=10, A=7, M=3, T=3;
	const int namt[4] = {N,A,M,T};

	int sel[3]={A}; // init to be out of range
	
	const float Instances[10*8] = {	11.0,	2,		13,		4,		5,		6,		2,		0,
								-1,		8,		-9,		4,		-5,		-6,		-7,		1,
								8.2,	-2.3,	3.4,	-4.5,	5.5,	2.7,	7.8,	1,
								1.7,	5,		3.1,	1.8,	5,		-6,		-7,		0,
								9.5,	7,		-2.2,	3.7,	-4.4,	5,		6,		1,
								-1.1,	-2,		3.3,	-4.6,	1,		6,		1,		0,
								6.1,	5.7,	3.4,	4.5,	5,		2.9,	-7,		0,
								1.6,	2,		3.5,	2.4,	-5,		-6,		1,		1,
								7.7,	-2,		-8.7,	-8.3,	5,		6,		-7.2,	0,
								0,		5,		2.8,	3.2,	2,		-5,		6,		0};
	
	
	
	Relief (Instances,namt,sel);
	for(int i = 0; i < T; i++)
	  printf("%d ",sel[i]);

	printf("\n");

}
