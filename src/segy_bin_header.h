#ifndef __SEGY_BIN_HEADER
#define __SEGY_BIN_HEADER

#include <map>
#include <string>
#include <vector>

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif

#include "seismic_abstract_header.h"

class segy_bin_header : public seismic_abstract_header {
	void *obj = nullptr;
	static const std::vector<std::tuple<std::string, std::string, int>> f_map;
	std::map<std::string, int> fields;
public:
	segy_bin_header(const void *obj);
	segy_bin_header& operator=(const void *obj);
	~segy_bin_header();

	int				jobID();
	int				lineNum();
	int				reelNum();
	int				numTraces();
	int				numAuxTraces();
	int				sampleIntUS();
	int				sampleIntOrigUS();
	int				numSamples();
	int				numSamplesOrig();
	unsigned short	dataSampleFormat();
	unsigned short	fold();
	unsigned short	sortCode();
	unsigned short	vertSumCode();
	unsigned short	sweepFreqStart();
	unsigned short	sweepFreqEnd();
	unsigned short	sweepCode();
	unsigned short	taperType();
	unsigned short	correlatedTraces();
	unsigned short	gainRecovered();
	unsigned short	ampRecoveryMethod();
	unsigned short	unitSystem();
	unsigned short	polarity();
	unsigned short	vibPolarityCode();
	unsigned short	revisionNum();
	unsigned short	fixedTraceLengthFlag();
	unsigned short	numExtendedBlocks();

	virtual std::pair<std::string, int> get(int index);
	virtual int index_of(const std::string &name);
	virtual std::map<std::string, int> to_map();

private:
	void initialize();
};

#ifdef PYTHON
void py_segy_bin_header_init(py::module &m,
	py::class_<segy_bin_header, std::shared_ptr<segy_bin_header>> &py_segy_bin_header);
#endif

#endif
