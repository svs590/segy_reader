#pragma once

#include <string>
#include <exception>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "segy_trace_header.h"
#include "seismic_trace.h"
#include "seismic_trace_header.h"
#include "segy_file.h"

class segy_trace_header;

class segy_trace : public seismic_trace {
    seismic_variant_vector f_data;
    endian_order f_order = endian_order::big;

public:
    segy_trace() { }
	segy_trace(const segy_trace &trace);
	segy_trace(
        const segy_trace_header &header, 
        const byte_t *data, 
        int num_samples, 
        segy_data_format format, 
        endian_order order
    );
	segy_trace(
        const segy_trace_header &header, 
        const seismic_variant_vector &data
    );
	segy_trace(
        std::shared_ptr<segy_trace_header> header, 
        const seismic_variant_vector &data
    );
	segy_trace(std::shared_ptr<segy_trace_header> header, 
        const byte_t *data, 
        int num_samples, 
        segy_data_format format,
        endian_order order
    );
	~segy_trace();

	virtual std::shared_ptr<seismic_trace_header> header();
	virtual void set_header(std::shared_ptr<seismic_trace_header> header);
	virtual seismic_variant_vector &data();
	virtual void set_data(const seismic_variant_vector &data);

	virtual object_type type_id() { return object_type::SEGY_TRACE; }

private:
    void parse(const byte_t *data, int num_samples, segy_data_format format, endian_order order);
};
