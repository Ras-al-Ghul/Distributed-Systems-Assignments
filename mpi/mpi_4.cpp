#include "mpi.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <map>

#define mp make_pair

const int MASTER = 0;
const int MAXARRAYSIZE = 20;

using namespace std;

int data[MAXARRAYSIZE];

int main (int argc, char *argv[])
{
	int numtasks, taskid, rc, dest, offset, i, j, tag1, tag2, tag3, source, chunksize; 
	int n;
	
	int update(int myoffset, int chunk, int myid);
	
	MPI_Status status;

	/***** Initializations *****/
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	printf ("MPI task %d has started...\n", taskid);
	
	// chunksize = (ARRAYSIZE / numtasks);
	tag2 = 1;
	tag1 = 2;
	tag3 = 3;

	/***** Master task only ******/
	if (taskid == MASTER)
	{
		/* Initialize the array */
		cout<<"Enter number of elements"<<endl;
		scanf("%d", &n);
		if(n<numtasks){
			cout<<"Decrease the number of tasks"<<endl;
			return 0;
		}
		cout<<"Enter the elements"<<endl;
		for(i = 0; i < n; ++i)
		{
			scanf("%d", &data[i]);
		}

		int offsets[numtasks] = {};
		chunksize = n/numtasks;
		int oldc = chunksize;
		/* Send each task its portion of the array - master keeps 1st part */
		offset = chunksize;
		int temp = n-chunksize;
		for (dest = 1; dest < numtasks; dest++) 
		{
			MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
			if(temp>=chunksize and dest != numtasks-1){
				MPI_Send(&chunksize, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
				MPI_Send(&data[offset], chunksize, MPI_INT, dest, tag3, MPI_COMM_WORLD);
				printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
				offset = offset + chunksize;
				offsets[dest] = chunksize;
				temp -= chunksize;
			}
			else{
				MPI_Send(&temp, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
				MPI_Send(&data[offset], temp, MPI_INT, dest, tag3, MPI_COMM_WORLD);
				printf("Sent %d elements to task %d offset= %d\n",temp,dest,offset);
				offset = offset + temp;
				offsets[dest] = temp;
				temp -= temp;
			}
		}

		/* Master does its part of the work */
		offset = 0;
		offsets[0] = chunksize;
		update(offset, chunksize, taskid);

		for (dest=1; dest < numtasks; dest++)
		{
			MPI_Recv(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD, &status);
			MPI_Recv(&chunksize, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD, &status);
			MPI_Recv(&data[offset], chunksize, MPI_INT, dest, tag3, MPI_COMM_WORLD, &status);
		}
		
		multimap<int, int> get_min;
		offset = 0;
		int left[numtasks] = {};
		for(int i=0; i<numtasks; ++i){
			if(offset < n){
				get_min.insert(mp(data[offset],i));
				offset += oldc;
				left[i] = 1;
			}
		}
		
		int newdata[n], k = 0;

		while(not get_min.empty()){
			multimap<int, int>::iterator it = get_min.begin();
			get_min.erase(it);
			newdata[k++] = it->first;
			if(left[it->second] < offsets[it->second]){
				get_min.insert(mp(data[(it->second*oldc)+left[it->second]], it->second));
				left[it->second]++;
			}
		}

		cout<<"final array"<<endl;
		for (int i = 0; i < n; ++i)
		{
			cout<<newdata[i]<<" ";
		}
		cout<<endl;
	}  /* end of master section */

	/***** Non-master tasks only *****/
	if (taskid > MASTER)
	{
		/* Receive my portion of array from the master task */
		source = MASTER;
		MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		MPI_Recv(&chunksize, 1, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);
		MPI_Recv(&data[offset], chunksize, MPI_INT, source, tag3, MPI_COMM_WORLD, &status);
		
		update(offset, chunksize, taskid);

		MPI_Send(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD);
		MPI_Send(&chunksize, 1, MPI_INT, source, tag2, MPI_COMM_WORLD);
		MPI_Send(&data[offset], chunksize, MPI_INT, source, tag3, MPI_COMM_WORLD);
	} /* end of non-master */

	MPI_Finalize();

}/* end of main */


void update(int myoffset, int chunk, int myid)
{
	sort(data+myoffset, data+myoffset+chunk);

	cout << "Task: " << myid << " Elements: ";
	for(int i=0; i<chunk; ++i)
		cout<<data[myoffset+i]<<" ";
	cout<<endl;
	return;
}


