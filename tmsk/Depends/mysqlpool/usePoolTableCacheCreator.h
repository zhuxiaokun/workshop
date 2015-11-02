#pragma once
#include "tableCacheUsePool.h"

namespace jg { namespace mysql {

	class UsePoolTableCacheCreator
	{
	public:
		TableCacheUsePool* Create(TableCacheSetting& tableSetting);
	};

} }
