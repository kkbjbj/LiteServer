
#include "hashid_gen.h"

HashIDGenerator::HashIDGenerator(int max)
{
	m_nCapacity = max;
	int upCap = 16;
	while(upCap < max)
	{
		upCap *= 2;
	}
	m_nMod = upCap - 1;  //a % 2^n = a & (2^n - 1)
	m_nCount = 0;
	m_pIDList = new struct HashIDNode[max];
	m_pHash = new struct HashIDNode*[upCap];

	for (int i = 0; i < max; ++i)
	{
		m_pIDList[i].id = -1;
		m_pIDList[i].next = NULL;
	}

	for (int i = 0; i < upCap; ++i)
	{
		m_pHash[i] = NULL;
	}
}

HashIDGenerator::~HashIDGenerator()
{
	delete []m_pIDList;
	delete []m_pHash;

	m_pIDList = NULL;
	m_pHash = NULL;
}

bool HashIDGenerator::Insert(int id)
{
	struct HashIDNode *node = NULL; 
	for (int i = 0; i < m_nCapacity; ++i)
	{
		if (-1 == m_pIDList[i].id)
		{
			node = &m_pIDList[i];
			break;
		}
	}

	if (NULL != node->next)
	{
		return false;
	}

	if (NULL == node)
	{
		return false;
	}

	++m_nCount;
	int hash = id & m_nMod;
	if (m_pHash[hash])
	{
		node->next = m_pHash[hash];
	}

	m_pHash[hash] = node;

	return true;
}

int HashIDGenerator::Remove(int id)
{
	int hash = id & m_nMod;
	struct HashIDNode *node = m_pHash[hash];
	if (NULL == node)
	{
		return -1;
	}

	if (node->id == id)
	{
		m_pHash[hash] = node->next;
		node->id = -1;
		node->next = NULL;
		--m_nCount;
		return int(node - m_pIDList);
	}

	while(node->next)
	{
		if (node->next->id == id)
		{
			struct HashIDNode *cur = node->next;
			node->next = cur->next;

			cur->id = -1;
			cur->next = NULL;
			--m_nCount;
			return int(cur - m_pIDList);
		}
		node = node->next;
	}
	return -1;
}

int HashIDGenerator::Find(int id)
{
	int hash = id & m_nMod;
	struct HashIDNode *node = m_pHash[hash];
	while(node)
	{
		if (node->id == id)
		{
			return int(node - m_pIDList);
		}
		node = node->next;
	}

	return -1;
}
