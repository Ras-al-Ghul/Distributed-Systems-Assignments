# Distributed Systems Assignments

### Erlang Matrix Multiplication
 - Install Erlang on Ubuntu <https://tecadmin.net/install-erlang-on-ubuntu/> This might not be a straighforward task on some distros.
 - `singlematrixmul.erl` is the code for matrix multiplication on a single process. `distmatrixmul.erl` is the sample template code for distributed matrix multiplication on spawned nodes. `distmatrixmul2.erl` is distributed matrix multiplication where the granularity is a row of A and a column of B at a process/node. `distmatrixmul3.erl` is distributed matrix multipliation where the granularity is a row of A and entire B matrix at a process/node. `examples.txt` contains a sample invocation of the code and a Python snippet to generate matrices of given dimensions.
 - To run (eg): Start an erlang node with `erl -sname node` and in that node execute `distmatrixmul2:multiply([[1,2,3,4],[2,3,4,3]], [[2,2],[1,1],[6,2],[11,5]]).`
 - `Report.pdf` contains our interpretations of the timing results of the two granularities used.
 - You could try `nodes(known)` and `nodes(connected)` to see the nodes known to the current node (nodes which were spawned in our case) and nodes which are currently connected to the node that you run the command from.

### mpi
 - `mpi_4.cpp` is about sorting an array using mpi - distributes computation amongst spawned processes depending on the size of the input array. `mpi_10.cpp` is about solving a set of linear equations using Gaussian Elimination [Link](https://stackoverflow.com/questions/25236369/c-parallel-implementation-of-gauss-elimination-with-mpi?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa) - distributes computation depending on the number of linear equations (where number of unknowns = number of equations) and the number of spawned processes.
 - Install mpi using <https://jetcracker.wordpress.com/2012/03/01/how-to-install-mpi-in-ubuntu/>
 - To run: `mpic++ mpi_x.cpp` and `mpirun -n 4 ./a.out` - you will be prompted for inputs. 4 is the number of processes which will be created in this case, you could vary it.

### rpc
 - Simulates a client-server setting where the communication is through rpc.
 - The client tells the server to execute the Unix `last` command and the output is printed on the client terminal. `prog.x` creates a string type for communication and sets its size.
 - To execute: `make`, `sudo rpcbind` and run `./server` on one terminal window and `./client last [args for last]` on another terminal window.
