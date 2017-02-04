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
#include <cmath>

float calculateSD(std::vector<int> data)
{
	if(data.size()==1){
		return 0.0;
	}
	
	float sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for(i = 0; i < data.size(); ++i)
        {
            sum += data[i];
        }

    mean = sum/data.size();

    for(i = 0; i < data.size(); ++i)
        standardDeviation += pow(data[i] - mean, 2);

    return sqrt(standardDeviation / (data.size()-1));
}


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
        for (std::vector<int>::iterator it = temp.begin(); it != temp.end();
			 ++it) {
            sum += *it;
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
	for(auto& rowmap: rowMaps){
		if(rowmap.size()==0){
			return rows;
		}
	}
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
			 int& currentDepth, std::vector<std::vector<float> >& standard_ds) {
    //backtrack to a valid parent
    while (domain[currentDepth].size() == 0) {
		if(currentDepth>=0){
			for(auto& sd: standard_ds[currentDepth]){
				sd=0;
			}
		}
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
					   std::vector<int>& temp, std::vector<std::vector<int> >& solutionSet,
					   int depth) {
    if ((size_t) depth < sets.size()) {
        for (auto& row : sets[depth]) {
            temp[depth] = row;
            _generateComb_aux(sets, temp, solutionSet, depth + 1);
        }
    } else {
        solutionSet.push_back(temp);
        //print out the current solution, for debugging purposes
		
		  std::cout << "SOLUTION ";
		  for (auto& rowNum : temp) {
		  std::cout << rowNum+1 << " ";
		  }
		  std::cout << std::endl;
 
	}
}

//generate all the combinations for a set of rows
void generateComb(std::vector<std::vector<int> >& sets,
				  std::vector<std::vector<int> >& solutionSet) {
	std::vector<int> temp(sets.size());
    int depth = 0;
    _generateComb_aux(sets, temp, solutionSet, depth);

}

std::vector<std::vector<int> > backtracking(
											std::vector<std::vector<std::vector<int> > >& files, int lambda,
											int k) {

    bool terminate = false;
    int depth = k;
    //solution contains all the solutions, empty vector if no matching is found
	std::vector<std::vector<int> > solutionSet;
    //create one rowMaps instance for each depth,
    //thus construct a tree
	std::vector<ColStruct::rowMaps> colsTree(k);
    //construct a domain list
    //domain=[[[10,20,30],[20,10,30]], [[15,15,30],[30,20,10]].....] domain for each column
	std::vector<std::vector<std::vector<int> > > domain(k);

	ColStruct::rowMap empty_rowMap;
	ColStruct::rowMaps empty_rowMaps;
	ColStruct::it it;

	std::vector<std::vector<float> > standard_ds(k, std::vector<float>(files.size()));

    int domainsize_temp=0;

    //initialize first column of colsTree
    int fileNum = 0;
    colsTree[0] = empty_rowMaps;
    for (auto& file : files) {
		std::vector<int> temp_data;
        colsTree[0].push_back(empty_rowMap);
        int rowNum = 0;
        for (auto& row : file) {
            it = colsTree[0][fileNum].find(row[0]);
            //if value is already mapped, append row number to the vector
            if (it != colsTree[0][fileNum].end()) {
                it->second.push_back(rowNum);
				temp_data.push_back(row[0]);
            }
            //if not found create a map
            else {
                colsTree[0][fileNum].insert(
											std::pair<int, std::vector<int> >(row[0],
																			  std::vector<int>(1, rowNum)));
				temp_data.push_back(row[0]);
			}
            ++rowNum;
        }
		standard_ds[0][fileNum]=calculateSD(temp_data);
        //move to next file
        ++fileNum;
    }

    domain[0] = getMatching(colsTree[0], lambda);
	std::cout<<"initial domain size: "<<domain[0].size()<<std::endl; 

    //eliminate domain before checkpoint.
    // while (domain[0].size()>checkpoint){
    //   domain[0].erase(domain[0].begin());
    //}
    //std::cout<<"checkpoint domain size: "<<domain[0].size()<<std::endl;

    int domain_temp=0;
    //done initializing
    int currentDepth = 0;
    while (!terminate) {

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
                    generateComb(sets, solutionSet);
                }
                --currentDepth;
            }
            //backtrack
            //check if the tree is exhausted, terminate if it is
            //or update the tree
            bool result = toValid(colsTree, domain, currentDepth, standard_ds);
            if (result == false) {
                terminate = true;
            }

        }
		//if current state is not a leaf
        else {
            //if domain is empty, backtrack
            if (domain[currentDepth].size() == 0) {
                //check if the tree is exhausted, terminate if it is
                bool result = toValid(colsTree, domain, currentDepth, standard_ds);
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
  					std::vector<int> temp_data;
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
							temp_data.push_back(files[file][row][currentDepth]);
						}
                        //if not found create a map
                        else {
                            colsTree[currentDepth][file].insert(
																std::pair<int, std::vector<int> >(
																								  files[file][row][currentDepth],
																								  std::vector<int>(1, row)));
							temp_data.push_back(files[file][row][currentDepth]); 
						}
                    }
					standard_ds[currentDepth][file]=calculateSD(temp_data);
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
		/*
		if (domain[0].size()!=domainsize_temp){
			domainsize_temp=domain[0].size();
			std::cout<<domainsize_temp<<" "<<domain[1].size()<<std::endl;
		}
		*/
		for(auto& level: standard_ds){
			std::cout<<"[ ";
			for(auto& sd: level){
				std::cout<<sd<<" ";
			}
			std::cout<<"] ";
		}
		std::cout<<std::endl;

    }
    return solutionSet;
}

int main(int argc, char *argv[]) {

    clock_t t;
    clock_t readfile_t;
    t = clock();

    int lambda = atoi(argv[1]);    

    //initialize the file struct that's going to hold all the files
	std::vector<std::vector<std::vector<int> > > files;
    for (int i = 2; i < argc; i++) {
		readFile(files, argv[i]);
    }
	int k=files[0][0].size();

	std::vector<std::vector<int> > solutionSet;
    solutionSet = backtracking(files, lambda, k);

    if (solutionSet.size() == 0) {
		std::cout << "no solution" << std::endl;
    }
    else{
		std::cout<<"there are "<<solutionSet.size()<<" solutions"<<std::endl;
    }

    t = clock() - t;
	std::cout << "It took me " << t << " clicks (" << ((float) t) / CLOCKS_PER_SEC << " seconds)."<<std::endl;
    return 0;
}
