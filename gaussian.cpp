#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

int main(int argc, char *argv[]) {
	std::string line;
	std::ifstream fp(argv[1]);
	std::vector<std::vector<int> > matrix;

    if (fp.is_open()) {
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
                matrix.push_back(newRow);
            }
        }
    }
    else {
		std::cout << "Failed to read files" << std::endl;
    }

	int total_col=matrix[0].size();
	int col=0;
	while(col<total_col){
		std::map<int, int> countTimes;
		std::map<int, int>::iterator it;
		for(auto row: matrix){
			it=countTimes.find(row[col]);
			if(it!=countTimes.end()){
				it->second+=1;
			}
			else{
				countTimes.insert(std::pair<int, int>(row[col], 1));
			}
		}
		++col;

		std::cout<<"Column: "<<col<<std::endl;
		for(auto num: countTimes){
			std::cout<<num.first<<" "<<num.second<<std::endl;
		}
	}

	return 0;
}
