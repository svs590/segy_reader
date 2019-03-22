#ifndef __SEISMIC_DP
#define __SEISMIC_DP

#include <string>
#include <memory>

#include "obj_base.h"
#include "segy_trace.h"
#include "seismic_header_map.h"
#include "seismic_abstract_header.h"
#include "seismic_trace_header.h"


class seismic_data_provider : public obj_base {
protected:
	std::string filename;

public:
	seismic_data_provider(const std::string &filename_in) {
		filename = filename_in;
	}
	seismic_data_provider() { }

	virtual std::string text_header() = 0;
	virtual std::shared_ptr<seismic_abstract_header> bin_header() = 0;
	virtual std::shared_ptr<seismic_header_map> header_map() = 0;

	virtual std::shared_ptr<seismic_trace_header> trace_header(int index) = 0;
	virtual std::shared_ptr<seismic_trace> get_trace(int index) = 0;

	virtual void preprocessing() = 0;

	// get_geometry
	// get_traces(ilineno)
	// get_traces(xlineno)
	// get_traces(points) (interpolation)

	virtual void close() = 0;
	virtual int traces_count() = 0;
	virtual int samples_count() = 0;
	virtual float sample_interval() = 0;
};

#endif