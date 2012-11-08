/*
 * List.h
 *
 *  Created on: Oct 28, 2012
 *      Author: thypo
 */

#ifndef LIST_H_
#define LIST_H_

const int MaxItems = 100;
typedef double ItemType;

class FList{
public:
	FList();
	virtual ~FList();
	int Count() const;
	bool Add(const ItemType NewItem);
	ItemType Retrieve(const int Pos);
	bool Delete(const int Pos);

private:
	int Counter;
	ItemType Item[MaxItems];
	const int Index(const int Pos) const;
};



#endif /* LIST_H_ */
