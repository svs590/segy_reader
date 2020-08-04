#ifndef __OBJ_BASE
#define __OBJ_BASE

#include "data_types.h"


enum class seismic_coords_type { CDP, SRC };

class obj_base {
public:
	virtual object_type type_id() = 0;
};

#endif
