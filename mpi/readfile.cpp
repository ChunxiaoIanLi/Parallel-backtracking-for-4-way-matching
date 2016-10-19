#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <malloc.h>
//#include <mpi.h>


int main(int argc, char *argv[]) {
	/*
	int my_rank;
	int p;
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
	*/
	
	std::vector<std::vector<std::vector<short > > > matrices;
	matrices.reserve(argc);
	std::string line;
	int buffValue;
	int k=0;
	int rowCount;
	
	for(int i=1; i<argc; ++i){
		std::ifstream fp(argv[i]);
		rowCount=0;
		if (fp.is_open()) {
			while (getline(fp, line)) {
				if(line!=""){
					++rowCount;
				}
				if(i==1 && rowCount==1){
					std::stringstream buff(line);
					while (buff >> buffValue) {
						++k;
					}
				}
          }
		}
		matrices.emplace_back(std::vector<std::vector<short > >());
		matrices.back().reserve(rowCount);
	}
 
	int file=0;
	for(int i=1; i<argc; ++i){
		std::ifstream fp(argv[i]);
		if (fp.is_open()) {
			while (getline(fp, line)) {
				if(line!=""){
					//add a new row to file
					matrices[file].emplace_back(std::vector<short>());
					matrices[file].back().reserve(k);
					//parse each row put the values in the file buffer
					std::stringstream buff(line);
					//buffValue is each number in a row
					while (buff >> buffValue) {
						matrices[file].back().emplace_back(buffValue);
					}
				}
			}
		}
		else {
			std::cout << "Failed to read files" << std::endl;
		}
		++file;
	}

	for(auto i: matrices[0]){
		for(auto j: i){
			std::cout<<j<<" ";
		}
		std::cout<<std::endl;
	}
	malloc_stats();
	//while(1){}
	//MPI_Finalize();
	return 0;
}
