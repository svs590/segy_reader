#ifndef __SEGY_TRACE
#define __SEGY_TRACE

#include <string>
#include <exception>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "geolib_defines.h"

#include "segy_trace_header.h"
#include "seismic_trace.h"
#include "seismic_trace_header.h"

class segy_trace_header;

class segy_trace : public seismic_trace {
public:
	segy_trace() {}
	segy_trace(const segy_trace &header);
	segy_trace(const segy_trace_header &header, int num_samples, const float *data);
	segy_trace(const segy_trace_header &header, const Eigen::VectorXf &data);
	segy_trace(std::shared_ptr<segy_trace_header> header, const Eigen::VectorXf &data);
	segy_trace(std::shared_ptr<segy_trace_header> header, int num_samples, const float *data);
	~segy_trace();

	virtual std::shared_ptr<seismic_trace_header> get_header();
	virtual void set_header(std::shared_ptr<seismic_trace_header> header);
	virtual Eigen::VectorXf &get_data();
	virtual void set_data(const Eigen::VectorXf &data);

	virtual object_type type_id() { return object_type::SEGY_TRACE; }
};

#endif
