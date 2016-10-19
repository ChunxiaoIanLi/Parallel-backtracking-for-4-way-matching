
/*
 * matching.cpp
 *
 *  Created on: 2015 M11 23
 *      Author: Ian
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>
#include "Struct.h"
#include <mpi.h>

//for sleep
#include <unistd.h>

void readFile(std::vector<std::vector<std::vector<int> > >& files,
	      std::string fileName) {
    
    //use 2D array to hold the entries for each file
    //    FILE A       FILE B
    //[[[1,2,3,4,5], [[1,2,3,4,5], ... , ]
    //  [2,3,5,5,6],  [2,3,5,5,6],
    //  [7,8,4,4,3]], [7,8,4,4,3]]
    std::string line;
    std::ifstream fp(fileName);
    int         my_rank;   /* My process rank           */


    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    //push an empty vector to the end of files
    std::vector<std::vector<int> > newFile;
    
    if (fp.is_open()) {
        //read each row
        while (getline(fp, line)) {
	    if(line!=""){            
		//add a new row to file
		std::vector<int> newRow;
		//parse each row put the values in the file buffer
		std::stringstream buff(line);
				
		//buffValue is each number in a row
		int buffValue;
				
		while (buff >> buffValue) {
		    newRow.push_back(buffValue);
		}
		newFile.push_back(newRow);
	    }
	}
    } else {
        std::cout << "Failed to read files" << std::endl;
    }
    
    files.push_back(newFile);
}

void _getMatching_aux(ColStruct::rowMaps& rowMaps,
		      std::vector<std::vector<int> >& rows, int depth, int& lambda,
		      std::vector<int>& temp) {
    if (depth >= 0) {
        //rows with value num in column files[depth][col]
        //the values are added in reverse order to the files order, ie: D C B A etc
        for (auto& num : rowMaps[depth]) {
            //this will put the values in correct order
            temp[depth] = num.first;
            _getMatching_aux(rowMaps, rows, depth - 1, lambda, temp);
        }
        
    } else {
        //at last file
        int sum = 0;
        for (auto& num: temp) {
            sum += num;
        }
        //one combination found
        if (sum == lambda) {
		    rows.push_back(temp);
        }
    }
}

std::vector<std::vector<int> > getMatching(ColStruct::rowMaps& rowMaps,
					   int& lambda) {
    //returns a 2D vector containing all the number combinations that sums to lambda
    //temp is a vector to keep the current combination
    std::vector<std::vector<int> > rows;
    std::vector<int> temp(rowMaps.size());
    int depth = rowMaps.size() - 1;
    _getMatching_aux(rowMaps, rows, depth, lambda, temp);
    return rows;
}

void printFiles(std::vector<std::vector<std::vector<int> > >& files) {
    std::string fileString[] = { "A", "B", "C", "D", "E", "F", "G" };
    //Loop over all files
    int fileCounter = 0;
    for (auto file : files) {
        std::cout << "File: " << fileString[fileCounter] << std::endl;
        // Loop over all the columns(maps)
        for (auto row : file) {
            for (auto num : row) {
                std::cout << num << " ";
            }
            std::cout << std::endl;
        }
    }
}

bool toValid(std::vector<ColStruct::rowMaps>& colsTree,
	     std::vector<std::vector<std::vector<int> > >& domain,
	     int& currentDepth) {
    //backtrack to a valid parent
    while (domain[currentDepth].size() == 0) {
        --currentDepth;
    }
    //tree is exhausted
    if (currentDepth < 0) {
        return false;
    }
    //currentDepth is at a valid node
    else {
        return true;
    }
    
}

void _generateComb_aux(std::vector<std::vector<int> >& sets,
		       std::vector<int>& temp, int depth) {
    if ((size_t) depth < sets.size()) {
        for (auto& row : sets[depth]) {
            temp[depth] = row;
            _generateComb_aux(sets, temp, depth + 1);
        }
    } else {
        //print out the current solution, for debugging purposes
      
	  std::cout << "SOLUTION ";
	  for (auto& rowNum : temp) {
	  std::cout << rowNum+1 << " ";
	  }
	  std::cout << std::endl;
      
    }
}

//generate all the combinations for a set of rows
void generateComb(std::vector<std::vector<int> >& sets) {
    std::vector<int> temp(sets.size());
    int depth = 0;
    _generateComb_aux(sets, temp, depth);
    
}

void backtracking(
		  std::vector<std::vector<std::vector<int> > >& files, int lambda,
		  int k) {
    
    int         my_rank;   /* My process rank           */
    int         tag=0;
    int         donor_tag=1; /* tag for donor */
    int         receiver_tag=2; /* receiver_tag */
    int         from_receiver_tag=3;
    MPI_Status  status;
    MPI_Request request;
    int         flag=0;
    int         receiver_process;
    int         donor_process;
    int         domain_receive_size;
    int         lines_receive_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    //match/domain received from process 0
    std::vector<int> domain_recv(files.size());

    
    ColStruct::rowMap empty_rowMap;
    ColStruct::rowMaps empty_rowMaps;
    ColStruct::it it;
    
    //create one rowMaps instance for each depth,
    //thus construct a tree
    std::vector<ColStruct::rowMaps> colsTree(k);

    int tag_recv;
    //listen for job to execute
    //get message size and receive message
    MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &domain_receive_size);
    if(domain_receive_size==1){
        //receiving jobs from other process
        while(domain_receive_size==1){
	    //get donor
	    MPI_Recv(&donor_process, domain_receive_size, MPI_INT, 0, receiver_tag, MPI_COMM_WORLD, &status);
	    
	    //listen to donor
	    MPI_Probe(donor_process, tag, MPI_COMM_WORLD, &status);
	    MPI_Get_count(&status, MPI_INT, &domain_receive_size);
	    //consume the message anyways
	    domain_recv.resize(domain_receive_size);
	    MPI_Recv(&domain_recv[0], domain_receive_size , MPI_INT, donor_process, tag, MPI_COMM_WORLD, &status);
	    
	    //if didn't get any job, listen to dispatcher again
	    if(domain_recv[0]==-1){
		//signal completion
		std::vector<int> exhaust_msg(1,-1);
		MPI_Send(&exhaust_msg[0], 1, MPI_INT, 0, from_receiver_tag, MPI_COMM_WORLD);
		MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &domain_receive_size);
	    }
	    else{
		MPI_Send(&my_rank, 1, MPI_INT, 0, from_receiver_tag, MPI_COMM_WORLD);
	    }
	    
	}

    }
    else{
	donor_process=0;
        domain_recv.resize(domain_receive_size);
        MPI_Recv(&domain_recv[0], domain_receive_size, MPI_INT, donor_process, tag, MPI_COMM_WORLD, &status);
    }
    
    
    //if didn't receive terminate msg
    while(domain_recv[0]!=-1){	
        int depth = k;
        //construct a domain list
        //domain=[[[10,20,30],[20,10,30]], [[15,15,30],[30,20,10]].....] domain for each column
        std::vector<std::vector<std::vector<int> > > domain(k);
        bool terminate = false;

        //add domains received
        int top_level= domain_recv[0];
        for(int i=0; i<(domain_recv.size()-1)/files.size(); ++i){
	    std::vector<int> temp_domain;
            for(int j=0; j<files.size(); ++j){
                temp_domain.push_back(domain_recv[1+i*files.size()+j]);
            }
            domain[top_level].push_back(temp_domain);
        }
        int currentDepth = top_level;

	if(donor_process!=0){
	    std::vector<int> lines_recv;
	    std::vector<std::vector<int> > lines_in_files(files.size());
	    for(int i=0; i<files.size(); ++i){
		//listen to donor for colTree line numbers
		MPI_Probe(donor_process, tag, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &lines_receive_size);
		//consume the message anyways
		lines_recv.resize(lines_receive_size);
		MPI_Recv(&lines_recv[0], lines_receive_size , MPI_INT, donor_process, tag, MPI_COMM_WORLD, &status);
		lines_in_files[i]=lines_recv;
	    }
	    
	    //make colTress
	    //initialize top_level column of colsTree
	    int fileNum = 0;
	    colsTree[top_level] = empty_rowMaps;
	    for (auto& file : lines_in_files) {
		colsTree[top_level].push_back(empty_rowMap);
		
		for (auto& row : file) {
		    it = colsTree[top_level][fileNum].find(files[fileNum][row][top_level]);
		    //if value is already mapped, append row number to the vector
		    if (it != colsTree[top_level][fileNum].end()) {
			it->second.push_back(row);
		    }
		    //if not found create a map
		    else {
			colsTree[top_level][fileNum].insert(
							    std::pair<int, std::vector<int> >(files[fileNum][row][top_level],
											      std::vector<int>(1, row)));
		    }
		}
		//move to next file
		++fileNum;
	    }
	}
	else{
	    //initialize first column of colsTree
	    int fileNum = 0;
	    colsTree[0] = empty_rowMaps;
	    for (auto& file : files) {
		colsTree[0].push_back(empty_rowMap);
		int rowNum = 0;
		for (auto& row : file) {
		    it = colsTree[0][fileNum].find(row[0]);
		    //if value is already mapped, append row number to the vector
		    if (it != colsTree[0][fileNum].end()) {
			it->second.push_back(rowNum);
		    }
		    //if not found create a map
		    else {
			colsTree[0][fileNum].insert(
						    std::pair<int, std::vector<int> >(row[0],
										      std::vector<int>(1, rowNum)));
		    }
		    ++rowNum;
		}
		//move to next file
		++fileNum;
	    }
	}
	/*        done initializing        */
        
        while (!terminate) {
	    
	    MPI_Iprobe(0, donor_tag, MPI_COMM_WORLD, &flag, &status);
	    //received split signal
	    if(flag==1){
			
		MPI_Recv(&receiver_process, 1, MPI_INT, 0, donor_tag, MPI_COMM_WORLD, &status);
				
                //find top level
                top_level=0;
                while(top_level<depth && domain[top_level].size()==0){
                    ++top_level;
                }
		
		//tree is already exhausted
                if(top_level>=depth-1 || (top_level==currentDepth && domain[top_level].size()==1)){
		    std::vector<int> exhaust_msg(1,-1);
                    MPI_Send(&exhaust_msg[0], 1, MPI_INT, receiver_process, tag, MPI_COMM_WORLD);
                }

		//if not then give half of the top level domain to receiver, with top_level at index 0
                else{
		    std::vector<int > domain_send;
                    domain_send.push_back(top_level);
                    int domain_send_size;
                    domain_send_size=1+((domain[top_level].size()-1)/2);
							
		    for(int i=domain_send_size-1; i>=0; --i){
                        for(auto j: domain[top_level][i]){
                            domain_send.push_back(j);
                        }
                    }
					
                    for(int i=domain_send_size; i>0; --i){
			domain[top_level].erase(domain[top_level].begin());
                    }
		    
		    //send the domains
		    MPI_Send(&domain_send[0], (domain_send_size*files.size()+1), MPI_INT, receiver_process, tag, MPI_COMM_WORLD);

		    //prepare to send lines from each file
		    for(int i=0; i<files.size(); ++i){
			std::vector<int> lines_send;
			for(auto& map: colsTree[top_level][i]){
			    for(auto& row: map.second){
				lines_send.push_back(row);
			    }
			}
			MPI_Send(&lines_send[0], lines_send.size(), MPI_INT, receiver_process, tag, MPI_COMM_WORLD);
		    }
		}
                flag=0;
	    }

	    //current state is a leaf
            if (currentDepth == depth - 1) {
                //succeed, get the all the combinations of rows
                if (domain[currentDepth].size() > 0) {
                    
                    std::vector<std::vector<int> > matches = domain[currentDepth];
                    
                    //matches=[[20,80],[40,60]...]
                    //match=[20, 80]
                    for (auto& match : matches) {
                        //sets for generating all the possibilities
                        //            File A      File B
                        //sets=[[rowN, rowM],[rowL, rowQ]....]
                        std::vector<std::vector<int> > sets;
                        for (size_t i = 0; i < files.size(); i++) {
                            //rows have value match[i] for i'th file
                            sets.push_back(colsTree[currentDepth][i][match[i]]);
                        }
                        //generate all the possible combinations
                        generateComb(sets);
                    }
                    --currentDepth;
                }
                //backtrack
                //check if the tree is exhausted, terminate if it is
                //or update the tree
                bool result = toValid(colsTree, domain, currentDepth);
                if (result == false) {
                    terminate = true;
                }
            }
            //if current state is not a leaf
            else {
                //if domain is empty, backtrack
                if (domain[currentDepth].size() == 0) {
                    //check if the tree is exhausted, terminate if it is
                    bool result = toValid(colsTree, domain, currentDepth);
                    if (result == false) {
                        terminate = true;
                    }
                }
                //if current domain is not empty
                else {
                    //move one level down the tree
                    ++currentDepth;
                    //get first combination from parent domain, and update rowMaps for currentDepth
                    //one file at a time
                    //leave all the rows that has the value, update notRemove
                    int file = 0;
                    colsTree[currentDepth] = empty_rowMaps;
                    while (file < int(files.size())) {
                        //construct a rowMap for current file and add to currentDepth
                        colsTree[currentDepth].push_back(empty_rowMap);
                        //domain[currentDepth][file] is the value in file(file) that is in the combination
                        int value = domain[currentDepth - 1][0][file];
                        for (auto& row : colsTree[currentDepth - 1][file][value]) {
                            it = colsTree[currentDepth][file].find(
								   files[file][row][currentDepth]);
                            //if value is already mapped, append row number to the vector
                            if (it != colsTree[currentDepth][file].end()) {
                                it->second.push_back(row);
                            }
                            //if not found create a map
                            else {
                                colsTree[currentDepth][file].insert(
								    std::pair<int, std::vector<int> >(
												      files[file][row][currentDepth],
												      std::vector<int>(1, row)));
                            }
                        }
                        //move to next file
                        ++file;
                    }
                    
                    //pop the combination out of the parent's domain
                    domain[currentDepth - 1].erase(
						   domain[currentDepth - 1].begin());
                    
                    //update domain for current level
                    domain[currentDepth] = getMatching(colsTree[currentDepth],
						       lambda);
                }
            }
        }

	//signal completion
	MPI_Send(&my_rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	//listen for job to execute
	//get message size and receive message

	MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_INT, &domain_receive_size);

	if(domain_receive_size==1){			
	    //receiving jobs from other process
	    while(domain_receive_size==1){
		if(status.MPI_TAG==donor_tag){
		    //get receiver
		    MPI_Recv(&receiver_process, domain_receive_size, MPI_INT, 0, donor_tag, MPI_COMM_WORLD, &status);
		    //tree is already exhausted
		    std::vector<int> exhaust_msg(1,-1);
		    MPI_Send(&exhaust_msg[0], 1, MPI_INT, receiver_process, tag, MPI_COMM_WORLD);
		    MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		    MPI_Get_count(&status, MPI_INT, &domain_receive_size);
		    if(domain_receive_size==4){
			domain_recv.resize(domain_receive_size);
			MPI_Recv(&domain_recv[0], domain_receive_size , MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		    }
		}
		else{
		    //get donor
		    MPI_Recv(&donor_process, domain_receive_size, MPI_INT, 0, receiver_tag, MPI_COMM_WORLD, &status);
		    //listen to donor
		    MPI_Probe(donor_process, tag, MPI_COMM_WORLD, &status);
		    MPI_Get_count(&status, MPI_INT, &domain_receive_size);
		    //consume the message anyways
		    domain_recv.resize(domain_receive_size);
		    MPI_Recv(&domain_recv[0], domain_receive_size , MPI_INT, donor_process, tag, MPI_COMM_WORLD, &status);
		    
		    //if didn't get any job, listen to dispatcher again
		    if(domain_recv[0]==-1){
			//signal completion
			std::vector<int> exhaust_msg(1,-1);
			MPI_Send(&exhaust_msg[0], 1, MPI_INT, 0, from_receiver_tag, MPI_COMM_WORLD);
			MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_INT, &domain_receive_size);
			if(domain_receive_size==4){
			    domain_recv.resize(domain_receive_size);
			    MPI_Recv(&domain_recv[0], domain_receive_size , MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			}
		    }
		    else{
			MPI_Send(&my_rank, 1, MPI_INT, 0, from_receiver_tag, MPI_COMM_WORLD);
		    }
		}
	    }
	}
	else{
	    domain_recv.resize(domain_receive_size);
	    MPI_Recv(&domain_recv[0], domain_receive_size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	}
    }
    return;
}

void dispatcher(std::vector<std::vector<std::vector<int> > >& files,
		int lambda, int k){
    int         rank;      /* current process' rank     */
    int         p;         /* The number of processes   */
    int         tag=0;
    int         donor_tag=1;
    int         receiver_tag=2;
    int         from_receiver_tag=3;
    int         source;
    int         feedback;
    int         p_done;
    
    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    std::vector<int> flags(p-1, 1);
    std::vector<MPI_Status> status(p-1);
    std::vector<MPI_Request> requests(p-1);

    //all the idle processor's
    std::vector<int> idle(p-1);
    
    //create one rowMaps instance for each depth,
    //thus construct a tree
    std::vector<ColStruct::rowMaps> colsTree(k);
    //construct a domain list
    //domain=[[[10,20,30],[20,10,30]], [[15,15,30],[30,20,10]].....] domain for each column
    std::vector<std::vector<std::vector<int> > > domain(k);
    
    ColStruct::rowMap empty_rowMap;
    ColStruct::rowMaps empty_rowMaps;
    ColStruct::it it;
    bool terminate=true;
        
    //initialize first column of colsTree
    int fileNum = 0;
    colsTree[0] = empty_rowMaps;
    for (auto& file : files) {
        colsTree[0].push_back(empty_rowMap);
        int rowNum = 0;
        for (auto& row : file) {
            it = colsTree[0][fileNum].find(row[0]);
            //if value is already mapped, append row number to the vector
            if (it != colsTree[0][fileNum].end()) {
                it->second.push_back(rowNum);
            }
            //if not found create a map
            else {
                colsTree[0][fileNum].insert(
					    std::pair<int, std::vector<int> >(row[0],
									      std::vector<int>(1, rowNum)));
            }
            ++rowNum;
        }
        //move to next file
        ++fileNum;
    }
    
    domain[0] = getMatching(colsTree[0], lambda);
    
    if(domain[0].size()>0){
        terminate=false;
    }
    
    //fill idle
    rank=1;
    while (rank<p){
        idle[rank-1]=rank;
        ++rank;
    }
    
    while(!terminate){
        //if there is a process idling, assign a job to it
        while(idle.size()>0 and !terminate){
	    std::cout<<domain[0].size()<<std::endl;
	    std::cout<<"idle: "<<idle.size()<<std::endl;
	    domain[0][0].insert(domain[0][0].begin(), 0);
	    MPI_Send(&domain[0][0][0], domain[0][0].size(), MPI_INT, idle[0], tag, MPI_COMM_WORLD);
            //pop match from domain and pop process from idle
	    domain[0].erase(domain[0].begin());
            if(domain[0].size()<=0){
                terminate=true;
            }
            //listen for feedback, p_done is the process just finished job (non-blocking)
            MPI_Irecv(&p_done, 1, MPI_INT, idle[0], tag,
		      MPI_COMM_WORLD, &requests[idle[0]-1]);
	    flags[idle[0]-1]=0;
	    idle.erase(idle.begin());
	}
	
        if(!terminate){
            //check if any process finished their jobs
            for(int process=1; process<p; ++process){
		if(flags[process-1]==0){
		    MPI_Test(&requests[process-1], &flags[process-1], &status[process-1]);
		    //received from process
		    if(flags[process-1]==1){
		      //add available to idle
			idle.push_back(process);
		    }
		}
	    }
	}
    }
    
    /* start randomized scheduling */
    
    //construct the complement of idle processors, busy
    std::vector<int> busy;
    for(int i=1; i<p; ++i){
	if(flags[i-1]==0){
	    busy.push_back(i);
	}
    }
    //loop for using randomized parallelism
    while(busy.size()>0){
	//continue only if busy.size()>0 and idle.size()>0
	if(busy.size()>0 && idle.size()>0){
	  
	    /* start assigning an idle process to a the first busy process */			
	    //get first idle process and remove it from idle
	    int receiver_process=idle[0];
	    int donor_process=busy[0];
	    //rotate busy so that the first process becomes the last in the vector
	    std::rotate(busy.begin(), busy.begin()+1, busy.end());
	    //send signal to both receiver and donor
	    MPI_Send(&donor_process, 1, MPI_INT, receiver_process, receiver_tag, MPI_COMM_WORLD);
	    MPI_Send(&receiver_process, 1, MPI_INT, donor_process, donor_tag, MPI_COMM_WORLD);
	    
	    MPI_Recv(&p_done, 1, MPI_INT, MPI_ANY_SOURCE, from_receiver_tag, MPI_COMM_WORLD, &status[receiver_process-1]);
			
	    //modify busy and idle if p_done is not -1
	    if(p_done!=-1){
		flags[receiver_process-1]=0;
		idle.erase(idle.begin());
		busy.push_back(receiver_process);
		MPI_Irecv(&p_done, 1, MPI_INT, receiver_process, tag,
			  MPI_COMM_WORLD, &requests[receiver_process-1]);
	    }
		std::cout<<"idle: "<<idle.size()<<std::endl;
	}
	//check if any process finished their jobs
        for(int process=1; process<p; ++process){
            if(flags[process-1]==0){
                MPI_Test(&requests[process-1], &flags[process-1], &status[process-1]);
                //received from process
                if(flags[process-1]==1){
                    //add process to idle
                    idle.push_back(process);
                    //remove process from busy
                    busy.erase(std::remove(busy.begin(),busy.end(), process), busy.end());
					std::cout<<"idle: "<<idle.size()<<std::endl;
				}
            }
        }
    }
	
    std::cout<<"done"<<std::endl;    
    //send singal to other processes to terminate
    rank=1;
    std::vector<int> terminate_msg(4, -1);
    while (rank<p){
        //tell all other processes to terminate
        MPI_Send(&terminate_msg[0], 4, MPI_INT, rank, tag, MPI_COMM_WORLD);
        ++rank;
    }
}

int main(int argc, char *argv[]) {
    
    int         my_rank;   /* My process rank           */
    int         p;         /* The number of processes   */
    int         lambda;       /* lambda   */
    int         k;           /* number of columns in the files   */
    MPI_Status  status;
    clock_t t;
    t = clock();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    lambda = atoi(argv[1]);
   	
    //initialize the file struct that's going to hold all the files
    std::vector<std::vector<std::vector<int> > > files;
    for (int i = 2; i < argc; i++) {
        readFile(files, argv[i]);
    }
    t=clock()-t;
    
    float fileReadingT=((float) t) / CLOCKS_PER_SEC;
    if(my_rank==0){
	float temp;
	for(int i=1; i<p; ++i){
	    MPI_Recv(&temp, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
	    fileReadingT+=temp;
	}
	fileReadingT=fileReadingT/p;
    }
    else{
	MPI_Send(&fileReadingT, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }
    
    if(my_rank==0){
        k=1;
    }
    else{
        k = files[0][0].size();
    }

    //process 0 deals with first column and dispatches jobs
    if (my_rank==0){
        dispatcher(files, lambda, k);
    }
    //other processes do backtracking
    else{
	backtracking(files, lambda, k);
    }

    t = clock() - t;
    float executionT=((float) t) / CLOCKS_PER_SEC;
    if(my_rank==0){
      float temp;
      for(int i=1; i<p; ++i){
	MPI_Recv(&temp, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
	executionT+=temp;
      }
      executionT=executionT/p;
    }
    else{
      MPI_Send(&executionT, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }
    if(my_rank==0){
      std::cout << "It took "<< fileReadingT << " seconds to read files"<<std::endl;   
      std::cout << "It took " << executionT << " seconds to finish the search"<<std::endl;
    }

    MPI_Finalize();

    return 0;
}
