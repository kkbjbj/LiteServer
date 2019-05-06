#pragma once
#include "unit_test_base.h"

class TestObj
{
public:
	TestObj() {}
	virtual ~TestObj() {}
	void Init() { std::cout << "Init() " << std::endl; }
	void UnInit() { std::cout << "UnInit() " << std::endl; }
};

class TestObjectPool : public TestBase
{
public:
	TestObjectPool() {}
	~TestObjectPool() {}

	virtual bool Run()
	{
		HashIDGenerator *gen = new HashIDGenerator(1000);
		for (int i = 0; i < 10000; ++i)
		{
			if (!gen->IsFull())
			{
				gen->Insert(i);
			}
		}
		std::cout << gen->IsFull() << std::endl;

		SetObjPool<TestObj>::Instance()->Init(2);
		TestObj *p = SetObjPool<TestObj>::Instance()->GetFreeObj();
		SetObjPool<TestObj>::Instance()->ReleaseObj(p);

		delete gen;

		return true;
	}

private:

};

