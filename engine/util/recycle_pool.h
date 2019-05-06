
#ifndef RECYCLE_POOL_H
#define RECYCLE_POOL_H

#include <list>
#include "util/macro_tool.h"

//usage :typedef TRecyclePool<ClassName> PoolName;
//define public method 'Release' for ClassName

template<class EntityClass, int MaxSize=100>
class TRecyclePool
{
public:

	TRecyclePool() : m_nMaxSize(MaxSize)
	{

	}

	virtual ~TRecyclePool()
	{
		typename std::list<EntityClass*>::iterator it = m_listEntity.begin();
		for (; it != m_listEntity.end(); ++it)
		{
			SAFE_RELEASE(*it);
		}
		m_listEntity.clear();
	}

	void PushEntity(EntityClass* pEntity)
	{
		if (pEntity == NULL)
		{
			return;
		}
		if (m_listEntity.size() >= m_nMaxSize)
		{
			pEntity->Release();
		}
		else
		{
			m_listEntity.push_back(pEntity);
		}
	}

	EntityClass* PopEntity()
	{
		if (m_listEntity.empty())
		{
			return NULL;
		}
		EntityClass* pEntity = m_listEntity.front();
		m_listEntity.pop_front();
		return pEntity;
	}

private:
	size_t					m_nMaxSize;
	std::list<EntityClass*>		m_listEntity;
};

#endif

