#ifndef HASHID_GEN_H
#define HASHID_GEN_H

#include <stddef.h>

struct HashIDNode
{
	int id;
	struct HashIDNode *next;
};

class HashIDGenerator
{
public:
	HashIDGenerator(int max);
	~HashIDGenerator();

public:
	bool Insert(int id);
	int Remove(int id);	
	int Find(int id);
	bool IsFull() { return m_nCount >= m_nCapacity; }

private:
	int m_nCapacity;
	int m_nMod;
	int m_nCount;
	struct HashIDNode *m_pIDList;
	struct HashIDNode **m_pHash;
};
#endif