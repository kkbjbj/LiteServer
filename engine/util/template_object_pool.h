#ifndef TEMPLATE_OBJECT_POOL_H
#define TEMPLATE_OBJECT_POOL_H

#include <set>
//usage: #define PoolName SetObjPool<ClassName>::Instance() 
//define public method 'Init', 'Uinit' for ClassName

template<class Obj>
class SetObjPool
{
	SetObjPool(){m_nNewTotal = 0; m_nUseTotal = 0;}
public:
	static SetObjPool* Instance()
	{
		static SetObjPool m_ObjPool; 
		return &m_ObjPool;
	}

	void Init( size_t newCount)
	{
		for (size_t i=0;i<newCount;++i)
		{
			Obj* pObj = new Obj;
			pObj->Init();
			m_nNewTotal++;
			m_setObj.insert(pObj);
		}
	}

	void UnInit()
	{
		while(!m_setObj.empty())
		{
			Obj* pObj = *m_setObj.begin();
			m_setObj.erase(m_setObj.begin());
			delete pObj;
		}
	}

	Obj*  GetFreeObj()
	{
		Obj* pObj = NULL;
		if(m_setObj.empty())
		{
			pObj = new Obj;
			pObj->Init();
			m_nNewTotal++;
		}
		else
		{
			pObj = *m_setObj.begin();
			pObj->Init();
			m_setObj.erase(m_setObj.begin());
		}
		m_nUseTotal++;
		return pObj;
	}
	void  ReleaseObj(Obj* pObj)
	{
		if(pObj)
		{
			pObj->UnInit();
			m_setObj.insert(pObj);
			m_nUseTotal--;
		}
	}

	~SetObjPool()
	{
		UnInit();
	}

	inline int GetNewTotal(){ return m_nNewTotal; }
	inline int GetUseTotal(){ return m_nNewTotal - (int)m_setObj.size(); }

protected:
	std::set<Obj*>  m_setObj;
	int             m_nNewTotal;
	int				m_nUseTotal;
};

#endif
