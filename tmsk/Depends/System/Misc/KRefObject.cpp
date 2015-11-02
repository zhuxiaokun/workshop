#include "KRefObject.h"
namespace System { 

	void KRefObject::DeleteThis()
	{
		delete this;
	}
}