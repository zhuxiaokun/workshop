/********************************************************************
created:	2008/12/25
created:	25:12:2008   16:21
filename: 	KCircularBuffer.h
file ext:	h
author:		jungle

purpose:

1.加一套更通用的流式接口 added by xueyan 2009-1-21
*********************************************************************/

#pragma once
#include "../KType.h"

namespace System {			namespace Collections {

	class KCircularBuffer
	{
	private:
		char*			m_pBuffer;
		// 这是为读取数据时，转角数据缓存用的
		char*			m_pWrappedBuffer;
		int				m_nBufferLength;
		int				m_nTempPos;
		int				m_nTempDataLength;
		volatile int	m_nMaxDataLength;
		volatile int	m_nCurrentPos;
		volatile int	m_nPopPos;

	public:
		KCircularBuffer()
		{
			m_pBuffer = NULL;
		}
		~KCircularBuffer()
		{
			Destroy();
		}
		BOOL Init(int nLength, int nWrappedLen = 1024)
		{
			ASSERT(nLength > 0);
			ASSERT(m_pBuffer == NULL);
			m_pBuffer = new char[nLength];

			m_pWrappedBuffer = SAFE_NEW_ARRAY(char, nWrappedLen);

			m_nBufferLength		= nLength;
			m_nMaxDataLength	= 0;
			m_nCurrentPos		= 0;
			m_nPopPos			= 0;
			ASSERT(m_pBuffer != NULL);
			return TRUE;
		}
		void Destroy()
		{
			m_nCurrentPos		= 0;
			m_nPopPos			= 0;
			m_nMaxDataLength	= 0;
			if(m_pBuffer != NULL)
			{
				delete []m_pBuffer;
				m_pBuffer = NULL;
			}

			SAFE_DELETE_ARRAY(m_pWrappedBuffer);
		}
		void Reset()
		{
			m_nCurrentPos		= 0;
			m_nPopPos			= 0;
			m_nMaxDataLength	= 0;
		}

		// 判断是否有数据可读，在读线程中使用
		BOOL	IsReadBufferEmpty()
		{
			if(m_nPopPos == m_nCurrentPos)
				return TRUE;
			return FALSE;
		}

		int	GetDataSize()
		{
			int nReadPos = m_nPopPos;
			int nWritePos = m_nCurrentPos;

			//  |rrrrrrrr|wwwwwwww|............|
			//  0        R        W(D)         E
			// 如果写游标在后，读游标在前，则返回两者差值
			if(nWritePos >= nReadPos)
				return nWritePos - nReadPos;

			//  |wwwwwwww|rrrrrrrr|wwwwwwww|...|
			//  0        W        R        D   E
			// 读游标在后，写游标在前，表示数据已经写循环了
			else
				return (m_nMaxDataLength - nReadPos) + nWritePos;
		}

		int GetBufferSize()
		{
			return m_nBufferLength;
		}

		char*	Alloc(int nLength)
		{
			char*	pRet		= NULL;
			int		nUseLen		= nLength + sizeof(int);
			// 因为游标数据会在另外的线程修改，所以要先clone一份
			int		nPopPos		= m_nPopPos;
			// 游标应在修改数据后修改，暂时使用临时值
			m_nTempPos			= m_nCurrentPos;
			m_nTempDataLength	= m_nMaxDataLength;

			// 如果buffer没有开始重新开始使用
			if(m_nTempPos >= nPopPos)
			{
				// 如果剩下的buffer够分配
				if(m_nTempPos + nUseLen <= m_nBufferLength)
				{
					// Log(LOG_DEBUG, "申请%d内存,从%d到%d,正常分配,%d使用中", nUseLen, m_nTempPos, m_nTempPos+nUseLen,m_nTempPos+nUseLen - nPopPos);
					pRet				= m_pBuffer + m_nTempPos;
					m_nTempPos			+= nUseLen;
					m_nTempDataLength	= m_nTempPos;
				}
				// 剩下的buffer不够分配
				else if(nUseLen <= nPopPos)
				{
					// Log(LOG_DEBUG, "申请%d内存,从%d到%d,从头分配,%d使用中", nUseLen, 0, nUseLen,m_nTempPos+nUseLen - nPopPos);
					pRet			= m_pBuffer;
					m_nTempPos		= nUseLen;
				}
				else
				{
					// 无法从m_pBuffer中申请nUseLen长度的数据！
					ASSERT(FALSE);
				}
			}
			else
			{
				if(m_nTempPos + nUseLen <= nPopPos)
				{
					// Log(LOG_DEBUG, "申请%d内存,从%d到%d,正常分配[*],%d使用中", nUseLen, m_nTempPos, m_nTempPos+nUseLen,m_nTempPos+nUseLen + m_nTempDataLength - nPopPos);
					pRet			= m_pBuffer + m_nTempPos;
					m_nTempPos		+= nUseLen;
				}
				else
				{
					// 无法从m_pBuffer中申请nUseLen长度的数据！
					ASSERT(FALSE);
				}
			}

			// 在buffer开始位置使用4字节来标记使用长度
			if(pRet != NULL)
			{
				*reinterpret_cast<int*>(pRet) = nUseLen;
				pRet += sizeof(int);
			}

			return pRet;
		}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// added by xueyan 2009-1-21
		BOOL	PushStream(char* strStream, int nLen)
		{
			BOOL bRet = FALSE;

			ASSERT(strStream);
			ASSERT(0 < nLen);

			// 这里不用加4个字节的长度了。
			// 因为游标数据会在另外的线程修改，所以要先clone一份
			int	nPopPos				= m_nPopPos;
			int nCurrentPos			= m_nCurrentPos;
			int nMaxDataLength		= m_nMaxDataLength;

			int nCanWriteDataLen = 0;
			int nNewCurPos = nCurrentPos + nLen;

			// 如果buffer没有开始重新开始使用
			if(nCurrentPos >= nPopPos)
			{
				// 这里把可写的长度都减一，是为了在写的时候不要让m_nPopPos和m_nCurrentPos重在一起
				nCanWriteDataLen = m_nBufferLength + nPopPos - nCurrentPos - 1;
				if(nLen > nCanWriteDataLen)
				{
					// 无法从m_pBuffer中申请nLen长度的数据！
					//ASSERT(FALSE);
					return FALSE;
				}
				else
				{
					// 如果剩下的buffer够分配
					if(nNewCurPos  <= m_nBufferLength)
					{
						memcpy(m_pBuffer + nCurrentPos, strStream, nLen);
						nCurrentPos += nLen;
						nMaxDataLength	= nCurrentPos;
						bRet = TRUE;
					}
					// 剩下的buffer不够分配, 流式的话应该去把它拼起来
					else
					{
						INT nDiffLengthHead = nNewCurPos - m_nBufferLength;
						INT nDiffLengthTail = m_nBufferLength - nCurrentPos;

						// 这样写是线程不安全的。
						/*PushStream(strStream, nDiffLengthTail);
						bRet = PushStream(strStream+nDiffLengthTail, nDiffLengthHead);*/

						memcpy(m_pBuffer + nCurrentPos, strStream, nDiffLengthTail );
						memcpy(m_pBuffer, strStream + nDiffLengthTail, nDiffLengthHead );
						nCurrentPos = nDiffLengthHead;
						nMaxDataLength = m_nBufferLength;
						bRet = TRUE;
					} // end else		
				} // end else

			} // end if nCurrentPos >= nPopPos
			else
			{
				nCanWriteDataLen = nPopPos - nCurrentPos - 1;
				if(nLen > nCanWriteDataLen)
				{
					// 无法从m_pBuffer中申请nLen长度的数据！
					//ASSERT(FALSE);
					return FALSE;
				}
				else
				{
					memcpy(m_pBuffer + nCurrentPos, strStream, nLen);
					nCurrentPos		+= nLen;
					bRet = TRUE;
				}
			}

			if ( bRet )
			{
				m_nCurrentPos = nCurrentPos;

				m_nMaxDataLength = nMaxDataLength;
			}

			return bRet;
		}

		// 两种取值的方法，一种是流式的，有多少取多少，流式的要把真实长度返回
		// 一种是要按包的大小取，不足就不取了，默认是流式
		char*	GetStream(int& nLen, BOOL bPacket = FALSE)
		{
			ASSERT(0 < nLen);

			int	nPopPos				= m_nPopPos;
			int nCurrentPos			= m_nCurrentPos;

			int nCanReadDataLen = 0;

			//写入数据时，已经减一保护了，所以这里可以这样判断 
			if(nPopPos == nCurrentPos)
			{
				// 没有数据
				nLen = 0;
				return NULL;
			}
			else if(nCurrentPos > nPopPos)
			{
				nCanReadDataLen = nCurrentPos - nPopPos;
			}
			else
			{
				nCanReadDataLen = m_nBufferLength + nCurrentPos - nPopPos;
			}

			
			if ( nLen > nCanReadDataLen )
			{
				if ( bPacket )
				{
					// 不是流式
					// 数据不足,不取
					nLen = 0;
					return NULL;
				}
				else
				{
					// 流式
					// 数据不足,就应当所有的都放出去
					nLen  = nCanReadDataLen;
				}
				
			}

			// 判断这次取的数据是不是要转角的。
			int nNewPopPos = nPopPos + nLen;
			if ( nNewPopPos > m_nBufferLength )
			{	
				// 要转角了，就把数据分两段拷贝出去
				// 要转角的话, 肯定是nPopPos > nCurrentPos

				INT nDiffLengthHead = nNewPopPos - m_nBufferLength;
				INT nDiffLengthTail = m_nBufferLength - nPopPos;

				memcpy(m_pWrappedBuffer, m_pBuffer + nPopPos, nDiffLengthTail );
				memcpy(m_pWrappedBuffer + nDiffLengthTail, m_pBuffer, nDiffLengthHead );
				//nPopPos = nDiffLengthHead;

				return m_pWrappedBuffer;
			}
			else
			{
				// 不转角，就直接把内存地址放出去
				ASSERT(m_nPopPos <= m_nBufferLength);
				return m_pBuffer + m_nPopPos;
			}
		}

		BOOL	PopStream(int nLen)
		{
			ASSERT(0 < nLen);

			BOOL bRet = FALSE;
			int	nCurrentPos		= m_nCurrentPos;
			int nPopPos		= m_nPopPos;

			int nCanReadDataLen = 0;

			//写入数据时，已经减一保护了，所以这里可以这样判断 
			if(nPopPos == nCurrentPos)
			{
				// 没有数据
				return FALSE;
			}
			else if(nCurrentPos > nPopPos)
			{
				nCanReadDataLen = nCurrentPos - nPopPos;
			}
			else
			{
				nCanReadDataLen = m_nBufferLength + nCurrentPos - nPopPos;
			}

			if ( nLen > nCanReadDataLen )
			{
				// 这里先ASSERT一下，不应当有这种情况出现吧。by xueyan
				ASSERT(FALSE);
				// 数据不足
				return FALSE;
			}

			INT nNewPopPos = nPopPos + nLen;
			if ( nNewPopPos > m_nBufferLength )
			{	
				// 要转角了，就把数据分两段拷贝出去
				// 要转角的话, 肯定是nPopPos > nCurrentPos
				INT nDiffLengthHead = nNewPopPos - m_nBufferLength;
				INT nDiffLengthTail = m_nBufferLength - nPopPos;

				/*memcpy(pWrappedData, m_pBuffer + nPopPos, nDiffLengthTail );
				memcpy(pWrappedData + nDiffLengthTail, m_pBuffer, nDiffLengthHead );*/
				nPopPos = nDiffLengthHead;

				bRet = TRUE;
			}
			else
			{
				// 不转角，就直接把内存地址放出去
				nPopPos = nNewPopPos;
				bRet = TRUE;
			}

			// 真正修改游标
			if ( bRet )
			{
				m_nPopPos = nPopPos;
			}

			return bRet;
		}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		BOOL	Push()
		{
			if(m_nTempPos == m_nCurrentPos)
				return FALSE;

			m_nCurrentPos = m_nTempPos;
			m_nMaxDataLength = m_nTempDataLength;
			return TRUE;
		}

		char*	Get()
		{
			if(m_nPopPos == m_nCurrentPos)
				return NULL;
			ASSERT(m_nPopPos <= m_nBufferLength);

			return m_pBuffer + m_nPopPos + sizeof(int);
		}

		BOOL	Pop()
		{
			BOOL bRet = FALSE;
			int	nCurPos		= m_nCurrentPos;
			int nMaxDataPos	= m_nMaxDataLength;
			int nPopPos		= m_nPopPos;

			// 上次已经pop到最后了，这次从头开始pop
			if(nPopPos == nMaxDataPos && nPopPos > nCurPos)
				nPopPos = 0;

			int nLength		= *reinterpret_cast<int*>(m_pBuffer + nPopPos);

			ASSERT(nLength > 0 && nLength <= m_nBufferLength - nPopPos);
			// pop的位置和当前游标一致，表明没有数据在buffer中，无需pop
			if(nPopPos == nCurPos)
			{
				bRet = FALSE;
				ASSERT(FALSE);
			}
			else if(nPopPos < nCurPos)
			{
				// 
				if(nPopPos + nLength <= nCurPos)
				{
					// Log(LOG_DEBUG, "...................POP%d内存,从%d到%d,正常,%d使用中", nLength, nPopPos, nPopPos+nLength,nCurPos - nPopPos-nLength);
					nPopPos += nLength;
					bRet = TRUE;
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else	//nPopPos > nCurPos
			{
				if(nPopPos + nLength <= nMaxDataPos)
				{
					// Log(LOG_DEBUG, "...................POP%d内存,从%d到%d,正常[*],%d使用中", nLength, nPopPos, nPopPos+nLength,nCurPos + nMaxDataPos - nPopPos-nLength);
					nPopPos += nLength;
					// 如果pop到buf最末端，将pop的位置指向数据的起始位置
					if(nPopPos == nMaxDataPos)
					{
						nPopPos = 0;
						// Log(LOG_DEBUG, "...................pop到末尾，准备从头pop");
					}
					bRet = TRUE;
				}
				else
				{
					// pop时长度大于buffer里的数据长度
					ASSERT(FALSE);
				}
			}

			// 真正修改游标
			m_nPopPos = nPopPos;
			return bRet;
		}
		
	};



	//==============================================================================================
	// Class: KExpandableCircularBuffer
	// 
	// 从环形队列继承。当buffer满了时，创建一个更大的新buffer，后续写操作都在新buffer里，读操作读完旧buffer后，
	// 将读指针移到新buffer上，然后删去旧buffer。
	//
	// 释放机制也类似，当buffer长期不饱满时，创建一个较小的新buffer。
	//==============================================================================================
#define SAMPLE_USAGE_COUNT  10000		// 每10000次写作一个决定，是否缩减
#define SHRINK_USAGE_RATIO	3			// Buffer 1/3 以上使用率则为正常
#define IDEAL_USAGE_RATIO	2			// 缩减时，Buffer长度缩到最高使用量的2倍
#define	MAX_BUFFER_COUNT	2

	// ***注：目前只使用1个备用Buffer，如果有多个备用Buffer，需根据多Buffer协调逻辑调整这部分代码！
	// ...
	// *** end
	class KExpandableCircularBuffer
	{
	protected:
		KCircularBuffer	m_Buffer[MAX_BUFFER_COUNT];
		BYTE			m_byReadBufferIndex;
		BYTE			m_byWriteBufferIndex;
		int				m_nExpandSize;
		int				m_nWrappeLength;


		KCircularBuffer* GetReadBuffer()
		{
			return &m_Buffer[m_byReadBufferIndex];
		}

		KCircularBuffer* GetWriteBuffer()
		{
			return &m_Buffer[m_byWriteBufferIndex];
		}

		KCircularBuffer* AllocWriteBuffer(KCircularBuffer* pBuffer)
		{
			// ***注：目前只使用1个备用Buffer，如果有多个备用Buffer，需根据多Buffer协调逻辑调整这部分代码！
			// 如果读写Buffer不是同一个，表示备用Buffer已使用
			if(m_byWriteBufferIndex != m_byReadBufferIndex)
			{
				ASSERT(FALSE);
				return NULL;
			}
			// 计算新的Buffer尺寸
			int nBufferSize = pBuffer->GetBufferSize() + m_nExpandSize;

			// 设置新的写Buffer序号
			m_byWriteBufferIndex = m_byReadBufferIndex == 0 ? 1 : 0;
			// *** end

			// 初始化新的写Buffer
			pBuffer = GetWriteBuffer();
			pBuffer->Init(nBufferSize, m_nWrappeLength);
			return pBuffer;
		}

		void TryFreeOldReadBuffer(KCircularBuffer* pBuffer)
		{
			// ***注：目前只使用1个备用Buffer，如果有多个备用Buffer，需根据多Buffer协调逻辑调整这部分代码！
			// 如果当前读写不是同一个Buffer时，并且读Buffer中没有数据了，就将读Buffer指向写Buffer，同时销毁旧的读Buffer
			if(m_byWriteBufferIndex != m_byReadBufferIndex && pBuffer->IsReadBufferEmpty())
			{
				pBuffer->Destroy();
				// 将读Buffer指向当前写Buffer
				// 注意：先销毁旧的读Buffer，后置下面的序号，防止写线程先访问这个Buffer
				m_byReadBufferIndex = m_byWriteBufferIndex;
			}
			// *** end
		}


		////==============================================================================================
		//// Funcition RecordBufferUsage: 记录当前buffer使用量，N次后如果使用量小于1/3，则缩小buffer
		////==============================================================================================
		//void RecordBufferUsage()
		//{
		//	KCircularBuffer* pBuffer = GetReadBuffer();
		//	// 算出当前使用量
		//	int nUsage = pBuffer->GetDataSize();

		//	// 新buffer要叠加
		//	if (m_pNewBuffer != NULL)
		//		nUsage += m_nNewPushPos;

		//	if (nUsage > m_nHighUsage)
		//		m_nHighUsage = nUsage;

		//	if (++m_nUsageCount >= SAMPLE_USAGE_COUNT)
		//	{
		//		// 经过一段时间了，做个决策
		//		m_nUsageCount = 0;
		//		if (m_pNewBuffer == NULL && m_nHighUsageCount * SHRINK_USAGE_RATIO < m_nBufferLength)
		//		{
		//			// 10000次写，buffer利用率不到1/3，我们缩小buffer到最大使用的2倍
		//			int nLen = m_nHighUsageCount * IDEAL_USAGE_RATIO;
		//			if (nLen > m_nMinLength)
		//			{
		//				ExpandBuffer(nLen);
		//				//m_nHighUsage = 0;		//第一次缩减，让存活周期长一点
		//				return
		//			}

		//		}
		//		m_nHighUsage = 0;
		//	}
		//}

	public:
		KExpandableCircularBuffer()
		{
			m_byReadBufferIndex = 0;
			m_byWriteBufferIndex = 0;
			// 每次增长1024字节
			m_nExpandSize	= 1024;
		}

		~KExpandableCircularBuffer()
		{
			Destroy();
		}

		BOOL Init(int nLength, int nWrappedLen = 1024, int nExpandSize = 1024)
		{
			m_nWrappeLength = nWrappedLen;
			m_nExpandSize = nExpandSize;
			return m_Buffer[m_byReadBufferIndex].Init(nLength, nWrappedLen);
		}

		void Destroy()
		{
			for(int i = 0; i < MAX_BUFFER_COUNT; i++)
				m_Buffer[i].Destroy();

			m_byReadBufferIndex = 0;
			m_byWriteBufferIndex = 0;
		}

		void Reset()
		{
			for(int i = 0; i < MAX_BUFFER_COUNT; i++)
				m_Buffer[i].Reset();

			m_byReadBufferIndex = 0;
			m_byWriteBufferIndex = 0;
		}

		BOOL	PushStream(char* strStream, int nLen)
		{
			BOOL bRet = FALSE;
			KCircularBuffer* pBuffer = GetWriteBuffer();

			// 尝试将数据写到当前写Buffer
			bRet = pBuffer->PushStream(strStream, nLen);

			// 如果写失败，需要将数据写到到备用Buffer
			if(!bRet)
			{
				pBuffer = AllocWriteBuffer(pBuffer);
				// 无法申请到新的Buffer
				if(pBuffer == NULL)
				{
					ASSERT(FALSE);
					return NULL;
				}

				// 将数据写到新的Buffer中
				bRet = pBuffer->PushStream(strStream, nLen);
			}
			return bRet;
		}

		char*	GetStream(int& nLen, BOOL bPacket = FALSE)
		{
			KCircularBuffer* pBuffer = GetReadBuffer();
			return pBuffer->GetStream(nLen, bPacket);
		}

		BOOL	PopStream(int nLen)
		{
			BOOL bRet = FALSE;
			KCircularBuffer* pBuffer = GetReadBuffer();
			bRet = pBuffer->PopStream(nLen);

			// 尝试释放旧的读Buffer
			TryFreeOldReadBuffer(pBuffer);
			return bRet;
		}

		char*	Alloc(int nLength)
		{
			char* pRet = NULL;
			KCircularBuffer* pBuffer = GetWriteBuffer();

			// 从当前Buffer中申请内存
			pRet = pBuffer->Alloc(nLength);

			// 当前写Buffer满，需要到备用Buffer来申请内存
			if(pRet == NULL)
			{
				pBuffer = AllocWriteBuffer(pBuffer);
				// 无法申请到新的Buffer
				if(pBuffer == NULL)
				{
					ASSERT(FALSE);
					return NULL;
				}

				// 从新的Buffer中申请内存
				pRet = pBuffer->Alloc(nLength);
			}

			return pRet;
		}

		BOOL	Push()
		{
			KCircularBuffer* pBuffer = GetWriteBuffer();
			return pBuffer->Push();
		}

		char*	Get()
		{
			KCircularBuffer* pBuffer = GetReadBuffer();
			return pBuffer->Get();
		}

		BOOL	Pop()
		{
			BOOL bRet = FALSE;
			KCircularBuffer* pBuffer = GetReadBuffer();
			bRet = pBuffer->Pop();

			// 尝试释放旧的读Buffer
			TryFreeOldReadBuffer(pBuffer);
			return bRet;
		}

	};
} /* Collections */				} /* System */

// using namespace ::System::Collections;