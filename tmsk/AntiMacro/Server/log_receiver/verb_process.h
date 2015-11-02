#pragma once
#include "verb_process.h"
#include <CommSession/KPropertyMap.h>
#include <System/Collections/KTree.h>

typedef void (*verb_function)(KPropertyMap& m, bool fromNetwork);
typedef JG_C::KMapByTree<ccstr,verb_function,KccstriCmp> VerbFunctionMap;
void process_verb(KPropertyMap& m, bool fromNetwork);
