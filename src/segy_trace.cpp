#include "segy_trace.h"

using namespace std;


segy_trace::segy_trace(const segy_trace &trace) {
	data = trace.data;
	header = trace.header;
}

segy_trace::segy_trace(const segy_trace_header &header, int num_samples, const float *data) {
	this->header = shared_ptr<seismic_trace_header>(new segy_trace_header(header));
	this->data.resize(num_samples);
	for (int i = 0; i < num_samples; ++i)
		this->data(i) = data[i];
}

segy_trace::segy_trace(const segy_trace_header &header, const Eigen::VectorXf &data) {
	this->header = shared_ptr<seismic_trace_header>(new segy_trace_header(header));
	this->data = data;
	num_samples = static_cast<int>(data.size());
}

segy_trace::segy_trace(shared_ptr<segy_trace_header> header, const Eigen::VectorXf &data) {
	this->header = header;
	this->data = data;
	num_samples = static_cast<int>(data.size());
}

segy_trace::segy_trace(shared_ptr<segy_trace_header> header, int num_samples, const float *data) {
	this->header = header;
	this->data.resize(num_samples);
	for (int i = 0; i < num_samples; ++i)
		this->data(i) = data[i];
}

segy_trace::~segy_trace() { }


shared_ptr<seismic_trace_header> segy_trace::get_header() {
	return header;
}

void segy_trace::set_header(shared_ptr<seismic_trace_header> header) {
	this->header = header;
}

Eigen::VectorXf &segy_trace::get_data() {
	return data;
}

void segy_trace::set_data(const Eigen::VectorXf &data) {
	this->data = data;
}