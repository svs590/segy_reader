#ifndef __OBJ_BASE
#define __OBJ_BASE

#include "data_types.h"

class obj_base {
public:
	virtual object_type type_id() = 0;
};

#endif
