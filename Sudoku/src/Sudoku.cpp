//============================================================================
// Name        : Sudoku.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

int a[9][9];


bool rowRule(int i, int j, int k){
	for(int g = 0; g<9; g++){
		if(a[i][g] == k){
			return 0;
		}
	}
	return 1;
}

bool columnRule(int i, int j, int k){
	for(int g = 0; g<9; g++){
		if(a[g][j] == k){
			return 0;
		}
	}
	return 1;
}

bool squareRule(int i, int j, int k){
	int r = (int) ceil((i+1)/3.);
	int c = (int) ceil((j+1)/3.);
	for (int u = (r-1)*3; u < ((r-1)*3 + 3); ++u) {
		for (int v = (c-1)*3; v < (c-1)*3 + 3; ++v) {
			if(a[u][v] == k){
				return 0;
			}
		}
	}
	return 1;
}



int main() {
	int un=0,row,column,temp;
	bool b=1;
	const char* filename="src/sudoku";
	ifstream in(filename);
	if(!in.is_open()){
		cout << "Problem with the file." << endl;
		return 0;
	} else{
		cout << "File opened." << endl;
		for(int i=0; i<9; i++){
			for(int j=0; j<9; j++){
				in >> a[i][j];
				if(!a[i][j])
					un++;
			}
		}
		in.close();
	}

	cout << "sudoku that is read:" << endl;
	for(int i=0; i<9; i++){
		for(int j=0; j<9; j++){
			cout << a[i][j] << " ";
		}
		cout << endl;
	}

	//main loop
	cout << "Calculating the solution." << endl;
	while(un && b){
		b=0;
		for(int i=0; i<9; i++){
			if(i>8){
				cout << "problem" << endl;
			}
			for(int j=0; j<9; j++){
				if(a[i][j]){
					continue;
				}
				temp = 0;
				for (int k = 1; k < 10; ++k) {
					if(rowRule(i,j,k) && columnRule(i,j,k) && squareRule(i,j,k)){
						if(!temp){
							temp = k;
						} else{
							temp = 0;
							break;
						}
					}
				}
				if(temp){
					a[i][j] = temp;
					b = 1;
					un--;
				}
			}
		}
	}
	if(!b){
		cout << "The sudoku cannot be solved by this algorithm." << endl;
	} else{
		cout << "The sudoku is solved." << endl;
	}
	for(int i=0; i<9; i++){
		for(int j=0; j<9; j++){
			cout << a[i][j] << " ";
		}
		cout << endl;
	}
	return 0;
}
