/********************************************************************
created:	2009/3/17
filename: 	SPath.h
file path:	
file base:	SPath
file ext:	cpp
author:		Allen Wang
copyright:	AuroraGame

purpose:	Implementation part of SPath.
*********************************************************************/

#include "SPath.h"

void KSPath::Compute()
{
	if (!m_dwNode || !m_connectionMatrix) throw 0;
	//init fill cost matrix and path matrix
	DWORD i, j, k;
	for (i = 0; i < m_dwNode; i++){
		for (j = 0; j < m_dwNode; j++){
			_cost(i, j) = _connect(i, j);
			if (_cost(i, j) != MAX && _cost(i, j) != 0){
				_path(i, j) = i;
			}else{
				_path(i, j) = MAX;
			}
		}
	}


	//use FlodWarshall to create shortest path
	for (k = 0; k < m_dwNode; k++) {
		for (i = 0; i < m_dwNode; i++) {
			for (j = 0; j < m_dwNode; j++) {
				if (_cost(i, j) > _cost(i, k) + _cost(k, j)){
					_cost(i, j) = _cost(i, k) + _cost(k, j);
					_path(i, j) = _path(k, j);
				}
			}
		}
	}
}

int KSPath::GetPath(DWORD from, DWORD to, DWORD* nodeIDBuffer, DWORD& size, int& cost)
{
	if (from >= m_dwNode || to >= m_dwNode || !m_connectionMatrix)
	{
		cost = MAX;
		size = 0;
		return -2;
	}
	cost = _cost(from, to);
	DWORD pos = 0;
	if (_GetPath(from, to, nodeIDBuffer, size, pos) > 0)
	{
		size = pos;
		return 1;
	}
	else if (pos == size)
	{
		return -1;
	}
	else
	{
		size = 0;
		return -2;
	}
}

int KSPath::_GetPath(DWORD from, DWORD to, DWORD* nodeIDBuffer, DWORD size, DWORD& pos) const
{
	if (from == to)
	{
		nodeIDBuffer[pos++] = from;
		return 1;
	}
	if (_path(from, to) == MAX)
	{
		return -2;
	}
	else
	{
		int rtnValue = 1;
		rtnValue = _GetPath(from, _path(from, to), nodeIDBuffer, size, pos);
		if (rtnValue <= 0) return rtnValue;
		if (pos == size)
		{
			return -1;
		}
		nodeIDBuffer[pos++] = to;
	}

	return 1;
}

KSPath::KSPath()
{
	m_dwNode = 0;
	m_connectionMatrix = NULL;
	m_pathMatrix = NULL;
	m_costMatrix = NULL;
}

KSPath::~KSPath()
{
	if (m_connectionMatrix)
	{
		delete[] m_connectionMatrix;
		delete[] m_pathMatrix;
		delete[] m_costMatrix;
		m_connectionMatrix = NULL;
		m_pathMatrix = NULL;
		m_costMatrix = NULL;
	}
}

void KSPath::SetMapSize( DWORD howManyNode )
{
	m_dwNode = howManyNode;
	if (m_connectionMatrix)
	{
		delete[] m_connectionMatrix;
		delete[] m_pathMatrix;
		delete[] m_costMatrix;
		m_connectionMatrix = NULL;
		m_pathMatrix = NULL;
		m_costMatrix = NULL;
	}
	if (m_dwNode)
	{
		m_connectionMatrix = new int[howManyNode * howManyNode];
		m_pathMatrix = new int[howManyNode * howManyNode];
		m_costMatrix = new int[howManyNode * howManyNode];
		DWORD i;
		for (i = 0; i < howManyNode * howManyNode; i++)
		{
			m_connectionMatrix[i] = MAX;
		}
		for (i = 0; i < howManyNode; i++)
		{
			_connect(i, i) = 0;
		}
	}
}

std::ostream& operator <<(std::ostream& os, const KSPath& spath)
{
	os.write((const char*)&spath.m_dwNode, sizeof(spath.m_dwNode));
	if (spath.m_dwNode != 0)
	{
		os.write((const char*)spath.m_connectionMatrix, sizeof(int) * spath.m_dwNode * spath.m_dwNode);
		os.write((const char*)spath.m_pathMatrix, sizeof(int) * spath.m_dwNode * spath.m_dwNode);
		os.write((const char*)spath.m_costMatrix, sizeof(int) * spath.m_dwNode * spath.m_dwNode);
	}
	return os;
}

std::istream& operator >>(std::istream& is, KSPath& spath)
{
	DWORD nodeCount = 0;
	is.read((char*)&nodeCount, sizeof(DWORD));
	spath.SetMapSize(nodeCount);
	if (nodeCount != 0)
	{
		is.read((char*)spath.m_connectionMatrix, sizeof(int) * spath.m_dwNode * spath.m_dwNode);
		is.read((char*)spath.m_pathMatrix, sizeof(int) * spath.m_dwNode * spath.m_dwNode);
		is.read((char*)spath.m_costMatrix, sizeof(int) * spath.m_dwNode * spath.m_dwNode);
	}
	return is;
}
