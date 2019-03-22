#ifndef __SEISMIC_TRACE
#define __SEISMIC_TRACE

#include <memory>
#include <vector>

#include "obj_base.h"
#include "seismic_trace_header.h"

class seismic_trace : public obj_base {
protected:
	std::shared_ptr<seismic_trace_header> header;
	std::vector<float> data;
	int num_samples;

public:
	virtual std::shared_ptr<seismic_trace_header> get_header() = 0;
	virtual void set_header(std::shared_ptr<seismic_trace_header> header) = 0;
	virtual std::vector<float> get_data() = 0;
	virtual void set_data(const std::vector<float> &data) = 0;
};

#endif
