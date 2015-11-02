#pragma once
#include <mysqldb/KTableCache.h>
#include "connectionPool.h"

class KCacheObject;

namespace jg { namespace mysql {

	typedef bool (*funcPrepareSqlContext)(KSqlContext* ctx, KCacheObject* cacheObj, LMREG::CppTable* args);
	class TableCacheUsePool : public KTableCacheReader
	{
	public:
		friend class KCacheObject;
		friend class KRefreshThread;
		friend class KCacheManager;

		class KRefreshThread : public System::Thread::KThread
		{
		public:
			KRefreshThread(TableCacheUsePool& cache);
			~KRefreshThread();

		public:
			void Execute();  // �̺߳���
			void Stop();

		public:
			TableCacheUsePool& m_cache;
			int m_refreshUpdateInterval;
			int m_refreshDeleteInterval;
			int m_refreshCheckoutInterval;
			TIME_T m_lastRefreshModify;
			TIME_T m_lastRefreshCheckOut;
			TIME_T m_lastRefreshDelete;
			volatile BOOL m_stopDesired;
		};

	public:
		TableCacheUsePool();
		virtual ~TableCacheUsePool();

	public: // �����ӿ�
		static TableCacheUsePool* CreateInstance(KSQLTableSchema& schema, funcPrepareSqlContext funcPrepareCtx, const CreateParam& createParam);

	public:	// ����ӿ�
		BOOL Initialize(funcPrepareSqlContext funcPrepareCtx, KSQLTableSchema& schema, const CreateParam& createParam);
		void Finalize();

	public:
		void finalize();
		bool prepareSqlContext(KCacheObject* cacheObj, LMREG::CppTable* args);

	public: // �ڲ�ʹ��
		void _markAllPartialModified(HANDLE hRec);
		int _getPartialModifiedCount(HANDLE hRec);

	public:
		/**
		* �����д��������������͵ĳ�����Ϊ�˰�ȫ�ϵĿ���
		* ������ģ���������н��г��ȼ�飬�Ծ��緢��ʹ���ϵĴ���
		*/
		HANDLE FindRecord(const void* pKey, int len);
		HANDLE LoadRecord(const void* pKey, int len);
		HANDLE CreateRecord(const void* pKey, int len);
		BOOL DiscardRecord(const void* pKey, int len);
		BOOL DeleteRecord(const void* pKey, int len);
		BOOL CheckoutRecord(const void* pKey, int len);

		//BOOL EmergencyCheckout(const void* pKey, int len, void* pUserData);
		//BOOL EmergencyCheckout(HANDLE hRec, void* pUserData);

		BOOL DiscardRecord2(HANDLE hRec);
		BOOL DeleteRecord2(HANDLE hRec);
		BOOL CheckoutRecord2(HANDLE hRec);

		// �Զ���������(identity�ֶ�)
		HANDLE CreateRecordDirect();

		// ��pKeyΪ����������¼
		HANDLE CreateRecordDirect(void* pKey, int len);

	public:
		BOOL _IsValidHandle(HANDLE hRec);

	public:
		virtual BOOL on_recordLoaded(HANDLE h) = 0;
		virtual BOOL on_unloadRecord(HANDLE h) = 0;

	public: // ���²����ڲ�ʹ�� // ���PKEY::key_type�ĳ���
		virtual BOOL _checkKeyLength(int len) const = 0;

		virtual HANDLE _findRecord(const void* pKey) = 0;
		virtual HANDLE findRecord(KTableRecordData& recData) = 0;

		virtual BOOL SetPrimaryKey(KCacheObject& cache, const void* pKey) = 0;
		virtual BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache) = 0;

		virtual System::Collections::KString<512> Key2String(const void* pKey) const = 0;
		virtual System::Collections::KString<512> GetKeyString(HANDLE hRec) const = 0;
		virtual System::Collections::KString<512> GetKeyString(KTableRecordData& recData) const = 0;
		virtual BOOL GetPrimaryKey(KCacheObject& cache, void* pKey) = 0;
		virtual BOOL IsPrimaryKey(KCacheObject& cache, const void* pKey) = 0;
		virtual BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key) = 0;

	protected:
		// ���ݿ����������һ����¼���ڴ���
		BOOL _LoadRecord(HANDLE hRec);

		// ���ݿ����������һ����¼
		BOOL _InsertRecord(HANDLE hRec, BOOL withKey);

		// ���ݿ���������ڴ����ݸ������ݿ��еļ�¼
		BOOL _UpdateRecord(HANDLE hRec);

		// ���ݿ���������ڴ����ݸ������ݿ��еļ�¼�������±��Ϊ�޸��˵��ֶ�
		BOOL _PartialUpdateRecord(HANDLE hRec);

		// ���ݿ������construction���ݿ���ɾ����¼
		BOOL _DeleteRecord(HANDLE hRec);

	protected:
		BOOL _fillDefaults(HANDLE hRec);

	public: // ͬ���߳�ʹ��
		BOOL RealInsertRecord(HANDLE hRec);
		BOOL RealUpdateRecord(HANDLE hRec);
		BOOL RealPartialUpdateRecord(HANDLE hRec);
		BOOL RealDeleteRecord(HANDLE hRec);
		BOOL RealCheckoutRecord(HANDLE hRec);

	//public:
	//	void ClearSqlError();
	//	DWORD GetLastSqlError() const;
	//	const char* GetLastSqlErrorMessage() const;

	//public:
	//	BOOL isConnActive();
	//	BOOL reconnect();
	//	BOOL keepConnectionFresh();

	//protected:
	//	// �ӽ���������ȡ��һ��
	//	BOOL QueuePush(KEmergencyEvent& event);
	//	BOOL QueuePop(KEmergencyEvent& event);
	//	BOOL QueueWait(int ms);
	//	void OnEmergencyFinished(BOOL success, void* pUserData);

	private:
		BOOL _CreateInsertSql();
		BOOL _CreateUpdateSql();
		int  _CreatePartialUpdateSQL(HANDLE hRec); // ���ز�������
		BOOL _CreateDeleteSql();
		BOOL _CreateSelectSql();

	private:
		HANDLE  _AllocRecord();
		HANDLE  _AllocFromBottom();
		HANDLE  _AllocFromFree();
		void    _FreeRecord(HANDLE hRec);
		void	_initRecord(KRecordHead* pHead);

	public:
		KSqlContext m_sqlCtx;
		funcPrepareSqlContext m_funcPrepareCtx;

	private:
		System::Collections::KDString<256> m_sqlInsert;
		System::Collections::KDString<256> m_sqlUpdate;
		System::Collections::KDString<256> m_sqlPartialUpdate;
		System::Collections::KDString<256> m_sqlDelete;
		System::Collections::KDString<256> m_sqlSelect;
		KSQLFieldDescribe* m_sqlParams;

		System::Sync::KThreadMutex m_mx;
		KRefreshThread m_refreshThread;

		// ��ʱ�������
		System::Collections::KQueueFifo<KEmergencyEvent,System::Sync::KMTLock,512> m_emergencyQueue;
		IEmergencyObserver* m_pEmergencyObserver;
	};

} }
