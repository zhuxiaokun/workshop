#pragma once
#include "../interface/PathFinderDefine.h"
#include "../interface/IFinderPlugin.h"

namespace KPathFinderAbout
{
	class IAlgorithmBase
	{
	public:
		IAlgorithmBase():m_maxLoop(MAX_LOOP),m_maxLineLength(MAX_LENGTH),mpPlugin(NULL),m_pBest(NULL) {}
		virtual ~IAlgorithmBase() {}

		virtual inline void SetMaxLoop(int_r maxLoop) {ASSERT_I(maxLoop > 0);m_maxLoop = maxLoop;}
		inline int_r GetMaxLoop() const {return m_maxLoop;}
		virtual inline void SetMaxLineLength(int_r nLength) {ASSERT_I(nLength > 0);m_maxLineLength = nLength;}
		inline int_r GetMaxLineLength() const {return m_maxLineLength;}
		virtual inline void SetPlugin(IFinderPlugin * pPlugin) {ASSERT_I(pPlugin);mpPlugin = pPlugin;}
		inline IFinderPlugin * GetPlugin() const {return mpPlugin;}
		inline _Node * GetBestNode() const { return m_pBest; }

	public:
		virtual int_r GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did) = 0;
		virtual _Node * GetBestNodeFromStart(bool bIsDis=true) 
		{
			if (m_pBest)
			{
				_Node* pNodeStart = m_pBest;

				int_r nXDis = -2;
				int_r nYDis = -2;
				float dis = 0.0f;


				// 这是一个反转程序
				KPathFinderAbout::_Node* pNode = m_pBest->parent;
				KPathFinderAbout::_Node* p1 = NULL;
				KPathFinderAbout::_Node* p2 = pNodeStart;
				if ( pNode )
				{
					nXDis = pNodeStart->x - pNode->x;
					nYDis = pNodeStart->y - pNode->y;
				}
				
			
				for(; pNode != NULL;)
				{
					if ( bIsDis )
						dis = GetPlugin()->AS_Cost(pNodeStart, p2, 0, NULL);
					if ( bIsDis && nXDis == pNodeStart->x - pNode->x && nYDis == pNodeStart->y - pNode->y && dis < m_maxLineLength )
					{
						// 中间直线上的点不用记了
					}
					else
					{
						// 产生转角了。
						if ( p2 != pNodeStart ) 
						{
							p2->parent = p1;
							p1 = p2;
							p2 = pNodeStart;

						}
						nXDis = pNodeStart->x - pNode->x;
						nYDis = pNodeStart->y - pNode->y;
					}
					pNodeStart = pNode;
					pNode = pNode->parent;
				}
				p2->parent = p1;
				p1 = p2;
				p2 = pNodeStart;

				p2->parent = p1;
				m_pBest = p2;
			}
			return m_pBest;
		}
		virtual void ReleaseNodes() = 0;

		virtual void clear_open() {ASSERT(FALSE);}
		virtual bool add_to_open(const _Node * pNode) {ASSERT(FALSE);return true;}
		virtual bool erase_from_open(const _Node * pNode) {ASSERT(FALSE);return true;}
		virtual _Node * find_from_open(int_r nID) {ASSERT(FALSE);return NULL;}
		virtual _Node * get_from_open(int_r nIdx) {ASSERT(FALSE);return NULL;}

		virtual void clear_close() {ASSERT(FALSE);}
		virtual bool add_to_close(const _Node * pNode) {ASSERT(FALSE);return true;}
		virtual bool erase_from_close(const _Node * pNode) {ASSERT(FALSE);return true;}
		virtual _Node * find_from_close(int_r nID) {ASSERT(FALSE);return NULL;}
		virtual _Node * get_from_close(int_r nIdx) {ASSERT(FALSE);return NULL;}

	protected:
		int_r m_maxLoop;
		int_r m_maxLineLength;
		IFinderPlugin * mpPlugin;
		_Node * m_pBest;
	};
}