#pragma once
#include "unit_test_base.h"

#include <fstream>

#define TEST_TIMES  5000
#define BUFFER_SIZE  128

using namespace std;

class LBufferThread : public ThreadBase
{
public:
	LBufferThread(RingBufferBase *pRingBufferL, int type, int times, int threadnum) {
		m_pRingBufferL = pRingBufferL; m_nType = type; m_IOTimes = times; m_nThreadNum = threadnum;
	}
	~LBufferThread() {}

	virtual void Run() {
		char writedata[] = "1234567890";
		int j = 0;
		int times = 0;
		bool stop = false;
		fstream file;
		char curdata[1];
		if (m_nThreadNum == 1)
		{
			if (m_nType == 0)
			{
				file.open("testdata", ios::in | ios::out | ios::binary);
				file.read(curdata, 1);
			}
			else
			{
				file.open("copydata", ios::out | ios::binary);
			}
		}
			
		

		while (!stop)
		{
			++j;
			j = j % 10;
			if (m_nType == 0)
			{				
				if (!m_pRingBufferL->IsFull())
				{	
					
					int re = m_pRingBufferL->WriteData(curdata, 1);
					if (re != 1)
					{
						//std::cout << "WriteData Fail: " << (int)m_pRingBufferL->GetLen() << std::endl;
					}
					else
					{
						//std::cout << "WriteData " << times << std::endl;
						++times;
						if (times >= m_IOTimes)
						{
							stop = true;
						}
						else
						{
							if (m_nThreadNum == 1) file.read(curdata, 1);
						}
					}
					
				}
				//TestSleep(1);
			}
			else
			{		
				if (!m_pRingBufferL->IsEmpty())
				{				
					
					int re = m_pRingBufferL->ReadData(curdata, 1);
					if (re != 1)
					{
						//std::cout << "ReadData Fail: " << (int)m_pRingBufferL->GetLen() << std::endl;
					}
					else
					{
						//std::cout << "ReadData " << (int)curdata[0] << std::endl;
						//file.write(curdata, 1);
						if (m_nThreadNum == 1)
						{
							file << curdata[0];
						}

						++times;
						if (times >= m_IOTimes)
						{
							stop = true;
						}
					}
					
				}
				//TestSleep(1);
			}

		};
		file.flush();
		file.close();
	}
private:
	RingBufferBase *m_pRingBufferL;
	int m_nType;
	int m_IOTimes;
	int m_nThreadNum;
};

class TestBuff : public TestBase
{
public:
	TestBuff() {}
	~TestBuff() {}
	void CreateThread(int num, int type)
	{
		fstream file1;
		int count = 0;
		file1.open("testdata", ios::out | ios::binary);
		for (size_t i = 0; i < TEST_TIMES; i++)
		{
			++count;
			char c = i % 10 + 48;
			file1 << c;
			if (i > 0 && i % 10 == 0)
			{
				++count;
				file1 << "\n";
			}
		}
		file1.close();
		int fsize = count;
		std::cout << " file size: " << fsize <<std::endl;

		uint64 start = TestGetTime();
		if (num > 5) num = 5;
		RingBufferBase *pRingBuffer = NULL;
		LBufferThread *pThreads[10];
		if (type == 1)
		{
			pRingBuffer = new RingBuffer(BUFFER_SIZE);
		}
		else if (type == 0)
		{
			pRingBuffer = new RingBufferSpsc(BUFFER_SIZE);
		}
		else
		{
			pRingBuffer = new RingBufferDN(BUFFER_SIZE);
		}

		for (int i = 0; i < num; i++)
		{
			pThreads[i * 2] = new LBufferThread(pRingBuffer, 0, fsize, num);
			pThreads[i * 2 + 1] = new LBufferThread(pRingBuffer, 1, fsize, num);
		}
		for (int i = 0; i < 2 * num; i++)
		{
			pThreads[i]->Start();
		}
		for (int i = 0; i < 2 * num; i++)
		{			
			pThreads[i]->Join();
		}

		uint64 stop = TestGetTime();
		if (type == 1)
		{
			std::cout << num << " Atom Lock RingBuffer Use Time: " << stop - start << std::endl;
		}		
		else if (type == 0)
		{
			std::cout << num << " RingBufferSpsc Use Time: " << stop - start << std::endl;
		}
		else
		{
			std::cout << num << " RingBufferDN Use Time: " << stop - start << std::endl;
		}

		fstream file11, file12;
		char curdata[2];
		file11.open("testdata", ios::in | ios::binary);
		file12.open("copydata", ios::in | ios::binary);
		string compareresult = "YES";
		for (int i = 0; i < fsize; i++)
		{
			file11.read(curdata, 1);
			file12.read(&curdata[1], 1);

			if (curdata[0] != curdata[1])
			{
				compareresult = "NO!!!!!!!";
				break;
			}
		}
		file11.close();
		file12.close();

		std::cout << " Compare Data Same : " << compareresult.c_str() << std::endl;

		delete pRingBuffer;
		for (int i = 0; i < 2 * num; i++)
		{
			delete pThreads[i];
		}
		
	}
	virtual bool Run()
	{
		CreateThread(1, 0);
		CreateThread(1, 1);
		CreateThread(1, 2);

		//CreateThread(2, 0);
		CreateThread(2, 1);
	
		return true;
	}

private:

};

