// <https://en.wikipedia.org/wiki/Gaussian_elimination>
// <https://en.wikipedia.org/wiki/LU_decomposition>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <mpi.h>

using namespace std;

const int MAX = 500;

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int i,j,k;
	int map[MAX];
	double A[MAX][MAX], b[MAX], x[MAX], sum, temp;
	
	int n;
	int rank, nprocs;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if(rank==0)
	{
		cout<<"Enter the number of equations"<<endl;
		scanf("%d", &n);

		cout<<"Enter the A matrix"<<endl;
		for(i=0; i<n; ++i){
			cout<<"Eqn "<<i+1<<endl;
			for(j=0; j<n; ++j){
				scanf("%lf", &A[i][j]);
			}
		}
		cout<<"Enter the b vector"<<endl;
		for(i=0; i<n; ++i){
			scanf("%lf", &b[i]);
		}
		// n = 2; n = 3;
		// A[0][0] = 5; A[0][1] = 2; 
		// A[0][0] = 2; A[0][1] = 1; A[0][2] = -1;
		// A[1][0] = 1; A[1][1] = 3; 
		// A[1][0] = -3; A[1][1] = -1; A[1][2] = 2;
		// A[2][0] = -2; A[2][1] = 1; A[2][2] = 2;
		printf("Matrix A\n");
		for(i=0; i<n; i++)
		{
			for(j=0; j<n; j++)
				printf("%10.5lf ", A[i][j]);
			printf("\n");
		}
		// b[0] = 10; b[1] = 7; 
		// b[0] = 8; b[1] = -11; b[2] = -3;
		printf("\nVector b\n");
		for(i=0; i<n; i++)
			printf("%10.5lf ", b[i]);
		printf("\n");
	}

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&A[0][0], MAX*MAX, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&b, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);    

	for(i=0; i<n; i++)
	{
		map[i] = i % nprocs;
	} 

	for(k=0;k<n;k++)
	{
		MPI_Bcast(&A[k][k], n-k, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		MPI_Bcast(&b[k], 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		for(i= k+1; i<n; i++) 
		{
			if(map[i] == rank)
			{
				temp = A[i][k]/A[k][k];

				for(j=k+1; j<n; j++)
				{
					A[i][j] = A[i][j] - (temp*A[k][j]);
				}
				b[i] = b[i] - (temp*b[k]);
			}
		}
	}

	if(rank==0)
	{ 
		x[n-1] = b[n-1]/A[n-1][n-1];
		for(i=n-2; i>=0; i--)
		{
			sum=0;
			for(j=i+1; j<n; j++)
			{
				sum = sum + (A[i][j]*x[j]);
			}
			x[i] = (b[i]-sum)/A[i][i];
		}
	}
	
	if(rank==0)
	{ 
		printf("\nVector x:");
		for(i=0; i<n; i++)
		{
			printf("\nx%d = %.2f\t",i,x[i]);

		}
		cout<<endl;
	}
		
	MPI_Finalize();
	return 0;
}