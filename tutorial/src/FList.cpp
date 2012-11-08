/*
 * FList.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: thypo
 */

#pragma hdrstop

#include "FList.h"

#pragma package(smart_init)

FList::FList() : Counter(0){

}

FList::~FList(){

}

int FList::Count() const{
	return Counter;
}

const int FList::Index(const int Pos) const{
	return Pos-1;
}

bool FList::Add(const ItemType NewItem){
	if((Counter < 0 ) || (Counter > MaxItems))
		return 0;
	else{
		Item[Counter] = NewItem;
		Counter++;
		return 1;
	}
}

ItemType FList::Retrieve(const int Pos){
	if((Pos < 0) || (Pos > Counter)){
		return 0;
	} else{
		return Item[Pos];
	}
}

bool FList::Delete(const int Pos){
	if((Pos < 0) || (Pos > Counter)){
		return 0;
	} else{
		for(int i = Pos + 1; i <= Counter; i++){
			Item[Index(i-1)] = Item[Index(i)];
		}
		Counter--;
		return true;
	}
}




