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
			 int& currentDepth, std::vector<int>& visited_columns, std::vector<int>& visited_columns_history) {
    //backtrack to a valid parent
    while (domain[currentDepth].size() == 0) {
		visited_columns[visited_columns_history[currentDepth]]=0;
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
		/*
		std::cout << "SOLUTION ";
		for (auto& rowNum : temp) {
			std::cout << rowNum+1 << " ";
		}
		std::cout << std::endl;
		*/
	}
}

//generate all the combinations for a set of rows
void generateComb(std::vector<std::vector<int> >& sets,
        std::vector<std::vector<int> >& solutionSet) {
    std::vector<int> temp(sets.size());
    int depth = 0;
    _generateComb_aux(sets, temp, solutionSet, depth);

}

void pickColumn(std::vector<int> &visited_columns, std::vector<int> &visited_columns_history, std::vector<std::vector<std::vector<int> > >& files, std::vector<ColStruct::rowMaps> &colsTree, std::vector<std::vector<std::vector<int> > >& domain, int &currentDepth, int &lambda, int &asdf){
	ColStruct::rowMap empty_rowMap;
	ColStruct::rowMaps best_rowMaps;
	ColStruct::it it;

	double aveLineCount=std::numeric_limits<double>::infinity();
	int bestDomainSize=std::numeric_limits<int>::max();

	int bestColumn;
	std::vector<std::vector<int> > bestDomain;
	
	//initialization
	if(currentDepth==0){
		for(int k=0; k<files[0][0].size(); k++){
			ColStruct::rowMaps temp_rowMaps;
			std::vector<std::vector<int> > temp_domain;
			int totalBinCount=0;
			int totalLineCount=0;
			int fileNum=0;
			for(auto& file : files){
				temp_rowMaps.push_back(empty_rowMap);
				int rowNum=0;
				totalLineCount+=file.size();
				for(auto& row : file){
					it = temp_rowMaps[fileNum].find(row[k]);
					//if value is already mapped, append row number to the vector
					if (it != temp_rowMaps[fileNum].end()) {
						it->second.push_back(rowNum);
					}
					//if not found create a map
					else {
						temp_rowMaps[fileNum].insert(
													 std::pair<int, std::vector<int> >(row[k],
																					   std::vector<int>(1, rowNum)));
						++totalBinCount;
					}
					++rowNum;
				}
				fileNum++;
			}
			temp_domain=getMatching(temp_rowMaps, lambda);
			//if a better column is found
			//std::cout<<totalLineCount/(double)totalBinCount<<" "<<aveLineCount<<std::endl; 
			if(temp_domain.size() < bestDomainSize){
				bestDomainSize=temp_domain.size();
				bestDomain=temp_domain;
				aveLineCount=totalLineCount/(double)totalBinCount;
				best_rowMaps=temp_rowMaps;
				bestColumn=k;
				//std::cout<<bestColumn<<std::endl;
				//std::cout<<"here"<<std::endl;
			}
			else if(temp_domain.size() == bestDomainSize){
				if(totalLineCount/(double)totalBinCount < aveLineCount){
					bestDomainSize=temp_domain.size();
					bestDomain=temp_domain;
					aveLineCount=totalLineCount/(double)totalBinCount;
					best_rowMaps=temp_rowMaps;
					bestColumn=k;
					//std::cout<<bestColumn<<std::endl;
					//std::cout<<"here1"<<std::endl;
				}
			}
		}
		colsTree[0]=best_rowMaps;
		visited_columns[bestColumn]=1;
		visited_columns_history[0]=bestColumn;
		domain[0]=bestDomain;
	}
	//body of the tree
	else if(currentDepth<visited_columns.size()){	
		for(int k=0; k<visited_columns.size(); k++){
			//for unvisited columns
			if(visited_columns[k]==0){
				//get first combination from parent domain, and update rowMaps for k
				//one file at a time
				//leave all the rows that has the value, update notRemove
				ColStruct::rowMaps temp_rowMaps;
				std::vector<std::vector<int> > temp_domain;
				int totalBinCount=0;
				int totalLineCount=0;
				int fileNum=0;
				for(auto& file : files){
					temp_rowMaps.push_back(empty_rowMap);
					int value = domain[currentDepth - 1][0][fileNum];
					totalLineCount+=colsTree[currentDepth - 1][fileNum][value].size();
					for (auto& row : colsTree[currentDepth - 1][fileNum][value]) {
						it = temp_rowMaps[fileNum].find(file[row][k]);
						//if value is already mapped, append row number to the vector
						if (it != temp_rowMaps[fileNum].end()) {
							it->second.push_back(row);
						}
						//if not found create a map
						else {
							temp_rowMaps[fileNum].insert(std::pair<int, std::vector<int> >(file[row][k], std::vector<int>(1, row)));
							++totalBinCount;
						}
					}
					fileNum++;
				}
				temp_domain=getMatching(temp_rowMaps, lambda);
				//if a better column is found
				if(temp_domain.size() < bestDomainSize){
					bestDomainSize=temp_domain.size();
					bestDomain=temp_domain;
					aveLineCount=totalLineCount/(double)totalBinCount;
					best_rowMaps=temp_rowMaps;
					bestColumn=k;
				}
				
				else if(temp_domain.size() == bestDomainSize){
					if(totalLineCount/(double)totalBinCount < aveLineCount){
						bestDomainSize=temp_domain.size();
						bestDomain=temp_domain;
						aveLineCount=totalLineCount/(double)totalBinCount;
						best_rowMaps=temp_rowMaps;
						bestColumn=k;
					}
				}
				if(currentDepth>=asdf){
					break;
				}
			}
		}
		colsTree[currentDepth]=best_rowMaps;
        visited_columns[bestColumn]=1;
        visited_columns_history[currentDepth]=bestColumn;
		domain[currentDepth]=bestDomain;
		//pop the combination out of the parent's domain
		domain[currentDepth - 1].erase(domain[currentDepth - 1].begin());
	}
}

std::vector<std::vector<int> > backtracking(
        std::vector<std::vector<std::vector<int> > >& files, int lambda,
        int k, int asdf) {

	/* debugging */
	int maxDepth=0;
	int totalbranches=0;

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
	//construct a list currently processed columns
	std::vector<int>visited_columns(k, 0);
	std::vector<int>visited_columns_history(k, -1);

    ColStruct::rowMap empty_rowMap;
    ColStruct::rowMaps empty_rowMaps;
    ColStruct::it it;

    int domainsize_temp=0;

	/*
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
	*/

	int currentDepth = 0;
	pickColumn(visited_columns, visited_columns_history, files, colsTree, domain, currentDepth, lambda, asdf);

	//    domain[0] = getMatching(colsTree[0], lambda);
    std::cout<<"initial domain size: "<<domain[0].size()<<std::endl; 
	totalbranches+=domain[0].size();
    //eliminate domain before checkpoint.
    // while (domain[0].size()>checkpoint){
    //   domain[0].erase(domain[0].begin());
    //}
    //std::cout<<"checkpoint domain size: "<<domain[0].size()<<std::endl;

    int domain_temp=0;
    //done initializing
    while (!terminate) {
        //current state is a leaf
        if (currentDepth == depth - 1) {  
			maxDepth=depth-1;
			//succeed, get the all the combinations of rows
            if (domain[currentDepth].size() > 0) {

                std::vector<std::vector<int> > matches = domain[currentDepth];
				//int x=solutionSet.size();
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
				//std::cout<<solutionSet.size()-x<<" solutions found here"<<std::endl;
				visited_columns[visited_columns_history[currentDepth]]=0;
                --currentDepth;
            }
            //backtrack
            //check if the tree is exhausted, terminate if it is
            //or update the tree
			bool result = toValid(colsTree, domain, currentDepth, visited_columns, visited_columns_history);
			if (result == false) {
                terminate = true;
            }

        }
        //if current state is not a leaf
        else {
            //if domain is empty, backtrack
            if (domain[currentDepth].size() == 0) {
                //check if the tree is exhausted, terminate if it is
				bool result = toValid(colsTree, domain, currentDepth, visited_columns, visited_columns_history);
				if (result == false) {
                    terminate = true;
                }
            }
            //if current domain is not empty
            else {
				++currentDepth;
				if(currentDepth>maxDepth){
                    maxDepth=currentDepth;
                }
				pickColumn(visited_columns, visited_columns_history, files, colsTree, domain, currentDepth, lambda, asdf);
				/*
				std::cout<<currentDepth<<std::endl;
				for(auto& a: visited_columns){
					std::cout<<a<<" ";
				}
				std::cout<<std::endl;
				for(auto& a: visited_columns_history){
					std::cout<<a<<" ";
                }
				std::cout<<std::endl;
				*/				

				/*
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
				*/

                //update domain for current level
				//                domain[currentDepth] = getMatching(colsTree[currentDepth], lambda);
				/*				
				for(auto& a: domain){
					std::cout<<a.size()<<" ";
                }
				std::cout<<std::endl;
				*/
				totalbranches+=domain[currentDepth].size();
            }
        }
		if (domain[0].size()!=domainsize_temp){
		  domainsize_temp=domain[0].size();
		  //std::cout<<domainsize_temp<<" "<<domain[1].size()<<" "<<maxDepth+1<<std::endl;
		  std::cout<<domainsize_temp<<" "<<domain[1].size()<<std::endl;
		  maxDepth=0;
		}
    }
	std::cout<<"total branches created "<<totalbranches<<std::endl;
    return solutionSet;
}

int main(int argc, char *argv[]) {

    clock_t t;
    clock_t readfile_t;
    t = clock();

    int lambda = atoi(argv[1]);    
	int asdf=atoi(argv[2]);

    //initialize the file struct that's going to hold all the files
    std::vector<std::vector<std::vector<int> > > files;
    for (int i = 3; i < argc; i++) {
      readFile(files, argv[i]);
    }
	int k=files[0][0].size();

    std::vector<std::vector<int> > solutionSet;
    solutionSet = backtracking(files, lambda, k, asdf);

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
