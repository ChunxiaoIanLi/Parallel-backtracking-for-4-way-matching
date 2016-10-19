/*
 * fileStruct.h
 *
 *  Created on: Nov 26, 2015
 *      Author: ian
 */

#ifndef STRUCT_H_
#define STRUCT_H_

class ColStruct {

public:
	//(vector)colTree=[rowMaps at depth(column) 1, rowMaps at depth(column) 2, rowMaps at depth(column) 3......]
	//(vector)rowMaps=[rowMap(A), rowMap(B), rowMap(C) ...]
	//(map)rowMap=[10:(vector)[1,3,4 ...], 20:[2,6,8 ...], 40:[7,9,10 ...] ...]
	//each element in fileStruct is rowMap, a map with the number as index and
	//row numbers(vector) as key
	typedef std::map<int, std::vector<int> > rowMap;
	typedef rowMap::iterator it;
	typedef std::vector<rowMap> rowMaps;
};

#endif /* STRUCT_H_ */
