#include <vector>
#include <math.h>
int main(int argc, char *argv[]){
	int my_rank;
	int p;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	//order of williamson
    n=argv[1];
	std::vector<int> seq(1+ceil(n-1/2), -1);
    seq[0]=1;
    //second half of the seq
	std::vector<int> counter(ceil(n-1/2)-ceil(log2(p)));

	std::ofstream ofs;
	ofs.open(outfilep, std::ofstream::out | std::ofstream::app);
	ofs<<"string"<<std::endl;
	ofs.close();

	MPI_Finalize();
}
