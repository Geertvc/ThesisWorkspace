//============================================================================
// Name        : tutorial.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "FList.h"
#include "sum.h"
using namespace std;



int main() {
	cout << "Please enter the numbers you want to know the average of: " << endl;
	FList List;
	bool done = 0;
	double number = 0.0;
	while(!done){
		cout << "Enter a number (or something else if you have entered enough).";
		cin >> number;
		if(cin.fail()){
			done = 1;
		} else{
			List.Add(number);
		}
	}


	double davg = 0.0;
	davg = sum(List)/List.Count();

	cout << "The average is " << davg << endl << endl;
	return 0;
}

//int main() {
//	FList List;
//	List.Add(1240.42);
//	List.Add(283.15);
//	List.Add(42.48);
//	List.Add(582.12);
//	List.Add(60.14);
//	List.Add(8022.28);
//
//	cout << "List of items";
//	for(int i = 0; i < List.Count(); i++)
//		cout << "\nItem No." << i + 1 << ": " << List.Retrieve(i);
//	cout << "\nNumber of items: " << List.Count() << " items\n\n";
//
//	List.Delete(5);
//	List.Delete(2);
//	cout << "List of items";
//	for(int i = 0; i < List.Count(); i++)
//		cout << "\nItem No." << i + 1 << ": " << List.Retrieve(i);
//	cout << "\nNumber of items: " << List.Count() << " items\n\n";
//
//	//getch();
//	return 0;
//}
