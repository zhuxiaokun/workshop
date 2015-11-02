/********************************************************************
created:	2009/3/17
filename: 	SPath.h
file path:	
file base:	SPath
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	It contains an algorithm that to compute global shortest path.
*********************************************************************/

#pragma once

#include <System/KType.h>
#include <iostream>
#include <fstream>
#include <iomanip>

class ISPath
{
public:
	virtual void SetMapSize(DWORD howManyNode) = 0;
	virtual bool SetCost(DWORD from, DWORD to, int cost) = 0;
	virtual void Compute() = 0;
	virtual int GetPath(DWORD from, DWORD to, DWORD* nodeIDBuffer, DWORD& size, int& cost) = 0;
	virtual int GetKindID() const = 0;
	virtual ~ISPath(){}
};

class KSPath : public ISPath
{
public:
	const static int MAX = 0x0fffffff;
#ifdef _DEBUG
private:
	void outputMatrix(const int* m, std::ostream& os)
	{
		if (!m || !m_dwNode) return;
		DWORD i, j;
		for (i = 0; i < m_dwNode; i++){
			for (j = 0; j < m_dwNode; j++){
				if (m[i * m_dwNode + j] == MAX){
					os << std::setw(5) << "MAX";
				}else{
					os << std::setw(5) << m[i * m_dwNode + j];
				}
			}
			os << std::endl;
		}
	}

public:
	void DebugTest()
	{
		SetMapSize(4);
		SetCost(0, 1, 10);
		SetCost(1, 0, 10);
		SetCost(1, 2, 10);
		SetCost(2, 1, 10);
		SetCost(1, 3, 10);
		SetCost(3, 1, 10);
		Compute();
		std::ofstream os("test.txt");
		os << "ConnectionMatrix:" << std::endl;
		outputMatrix(m_connectionMatrix, os);
		os << std::endl << "CostMatrix:" << std::endl;
		outputMatrix(m_costMatrix, os);
		os << std::endl << "PathMatrix:" << std::endl;
		outputMatrix(m_costMatrix, os);

		DWORD nodeBuff[1000];
		DWORD size;
		int cost = 0;
		GetPath(0, 3, nodeBuff, size, cost);
		for (DWORD i = 0; i < size; i++)
		{
			os << nodeBuff[i] << "->";
		}
	}
#endif // _DEBUG
public:
	KSPath();
	virtual ~KSPath();
	/**
	 * �����ܹ��Ľڵ�����
	 * ԭ���ļ�������ȫ���������
	 * param:
	 *		howManyNode:		�ڵ�����
	 */
	void SetMapSize(DWORD howManyNode);

	/**
	 * ����ĳ�ڽ�·�ߵĿ���
	 * param:
	 *		from:		Դ����
	 *		to:			Ŀ������
	 *		cost:		����ֵ
	 * return:
	 *		����������ã�����true������false
	 */
	bool SetCost(DWORD from, DWORD to, int cost)
	{
		if (from >= m_dwNode) return false;
		if (to >= m_dwNode) return false;
		if (!m_connectionMatrix) return false;
		_connect(from, to) = cost;
		return true;
	}

	/**
	 * ����·��
	 */
	void Compute();

	/**
	 * �õ����·��������ǰҪ�ȵ���Compute��������δ֪��
	 * param:
	 *		nodeIDBuffer:		in:�������������ָ��
	 *		size:				in:nodeIDBuffer���峤��		out:�����ĳ���
	 *		cost:				out:�ܿ���
	 * return:
	 *		> 0:				�ɹ�
	 *		-1:					Buffer����
	 *		-2:					·��������
	 *		����:				δ֪ԭ��
	 */
	int GetPath(DWORD from, DWORD to, DWORD* nodeIDBuffer, DWORD& size, int& cost);

	int GetKindID() const
	{
		return 1;
	}

protected:
	int _GetPath(DWORD from, DWORD to, DWORD* nodeIDBuffer, DWORD size, DWORD& pos) const;
protected:
	//�ܹ��ڵ���
	DWORD m_dwNode;

	//��cost���ڽӾ���
	int* m_connectionMatrix;
	//�������ɵ�·������
	int* m_pathMatrix;
	//���������cost����
	int* m_costMatrix;

	inline int& _cost(int x, int y)
	{
		return m_costMatrix[x * m_dwNode + y];
	}
	inline int& _path(int x, int y)
	{
		return m_pathMatrix[x * m_dwNode + y];
	}
	inline int& _connect(int x, int y)
	{
		return m_connectionMatrix[x * m_dwNode + y];
	}
	inline const int& _cost(int x, int y) const
	{
		return m_costMatrix[x * m_dwNode + y];
	}
	inline const int& _path(int x, int y) const
	{
		return m_pathMatrix[x * m_dwNode + y];
	}
	inline const int& _connect(int x, int y) const
	{
		return m_connectionMatrix[x * m_dwNode + y];
	}
public:
	friend std::ostream& operator <<(std::ostream& os, const KSPath& spath);
	friend std::istream& operator >>(std::istream& is, KSPath& spath);
};

std::ostream& operator <<(std::ostream& os, const KSPath& spath);
std::istream& operator >>(std::istream& is, KSPath& spath);
