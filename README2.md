# relief-mpi
Distributed implementation of "Relief" feature selection algorithm with MPI

# Input
it accept inputs from a text file.
the file must have a header containing 5 numbers :
- P = number of slave processors (including the master)
- N = number of samples  (ROWS)
- A = number of features (COLUMNS)
- M = number of iterations
- T = number of chosen features

# Output
each Slaves prints its choosen features (index of feature)
Master will print unique combination of all chosen feature indices.

# Run
supposed you have a mpi cluster (ssh, shared public-keys, hostnames)
suppodes both Master node and Slaves all have an account with the same name i.e user1
supposed copies of this repository is on the same path on all the nodes.

## Compile : 
mpicc -Wall ./mpi_relief.c ./relief.c -o relief 

## Copy
now you should copy the output file "relief" to all other nodes : 
scp relief user1@node-001:$PWD
scp relief user1@node-002:$PWD
...

## Execute
mpirun -host localhost,node-001,node-002 ./relief data.txt

(you can put the hostnames in a file then pass the filename to mpirun)
