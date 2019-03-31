#include "segy_bin_header.h"
#include "utils.h"

#include <iostream>
#include <iterator>

using namespace std;

DLLIMPORT int				cseis_csNativeSegyBinHeader_jobID(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_jobID(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_lineNum(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_reelNum(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_numTraces(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_numAuxTraces(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_sampleIntUS(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_sampleIntOrigUS(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_numSamples(const void *obj);
DLLIMPORT int				cseis_csNativeSegyBinHeader_numSamplesOrig(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_dataSampleFormat(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_fold(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_sortCode(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_vertSumCode(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_sweepFreqStart(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_sweepFreqEnd(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_sweepCode(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_taperType(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_correlatedTraces(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_gainRecovered(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_ampRecoveryMethod(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_unitSystem(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_polarity(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_vibPolarityCode(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_revisionNum(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_fixedTraceLengthFlag(const void *obj);
DLLIMPORT unsigned short	cseis_csNativeSegyBinHeader_numExtendedBlocks(const void *obj);


const vector<tuple<string, string, int>> segy_bin_header::f_map = {
	{"Job identification number",	"",				4},
	{"Line number", 	"",				4},
	{"Reel number",		"",			4},
	{"Number of data traces", "",					2},
	{"Number of auxiliary", 	"",			2},
	{"Sample interval", 	"",		2},
	{"Sample interval original field recording", "",	2},
	{"Number of samples", 	"",		2},
	{"Number of samples per data trace original field recording", "",		2},
	{"Data sample format code",	"",	2},
	{"Ensemble fold", 			"",		2},
	{"Trace sorting code", 		"",		2},
	{"Vertical sum code", 		"",	2},
	{"Sweep frequency start", 	"",	2},
	{"Sweep frequency end", 	"",		2},
	{"Sweep length", 			"",	2},
	{"Sweep type code", 			"",	2},
	{"Trace number of sweep channel",	"",	2},
	{"Sweep trace taper length at start",	"",		2},
	{"Sweep trace taper length at end",	"",	2},
	{"Taper type", 		"",	2},
	{"Correlated data traces ", 	"",			2},
	{"Binary gain recovered",	"",	2},
	{"Amplitude recovery method", 		"",	2},
	{"Measurement system",			"",	2},
	{"num Extended Blocks",	"",	numExtendedBlocks()}
}

segy_bin_header::segy_bin_header(const void *obj) {
	this->obj = const_cast<void*>(obj);
	initialize();
}

segy_bin_header& segy_bin_header::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	initialize();
	return *this;
}

segy_bin_header::~segy_bin_header() {

}

int				segy_bin_header::jobID() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_jobID(obj);
}

int				segy_bin_header::lineNum() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_lineNum(obj);
}

int				segy_bin_header::reelNum() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_reelNum(obj);
}

int				segy_bin_header::numTraces() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_numTraces(obj);
}

int				segy_bin_header::numAuxTraces() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_numAuxTraces(obj);
}

int				segy_bin_header::sampleIntUS() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_sampleIntUS(obj);
}

int				segy_bin_header::sampleIntOrigUS() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_sampleIntOrigUS(obj);
}

int				segy_bin_header::numSamples() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_numSamples(obj);
}

int				segy_bin_header::numSamplesOrig() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_numSamplesOrig(obj);
}

unsigned short	segy_bin_header::dataSampleFormat() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_dataSampleFormat(obj);
}

unsigned short	segy_bin_header::fold() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_fold(obj);
}

unsigned short	segy_bin_header::sortCode() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_sortCode(obj);
}

unsigned short	segy_bin_header::vertSumCode() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_vertSumCode(obj);
}

unsigned short	segy_bin_header::sweepFreqStart() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_sweepFreqStart(obj);
}

unsigned short	segy_bin_header::sweepFreqEnd() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_sweepFreqEnd(obj);
}

unsigned short	segy_bin_header::sweepCode() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_sweepCode(obj);
}

unsigned short	segy_bin_header::taperType() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_taperType(obj);
}

unsigned short	segy_bin_header::correlatedTraces() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_correlatedTraces(obj);
}

unsigned short	segy_bin_header::gainRecovered() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_gainRecovered(obj);
}

unsigned short	segy_bin_header::ampRecoveryMethod() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_ampRecoveryMethod(obj);
}

unsigned short	segy_bin_header::unitSystem() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_unitSystem(obj);
}

unsigned short	segy_bin_header::polarity() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_polarity(obj);
}

unsigned short	segy_bin_header::vibPolarityCode() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_vibPolarityCode(obj);
}
unsigned short	segy_bin_header::revisionNum() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_revisionNum(obj);
}

unsigned short	segy_bin_header::fixedTraceLengthFlag() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_fixedTraceLengthFlag(obj);
}

unsigned short	segy_bin_header::numExtendedBlocks() {
	if (obj == nullptr)
		throw runtime_error("Object segy_bin_header is not initialized");
	return cseis_csNativeSegyBinHeader_numExtendedBlocks(obj);
}

pair<string, int> segy_bin_header::get(int index) {
	return fields[index];
}

int segy_bin_header::index_of(const string &name) {
	for (int i = 0; i < fields.size(); ++i)
		if (fields[i].first == name)
			return i;
	return NOT_INDEX;
}

map<string, int> segy_bin_header::to_map() {
	map<string, int> res;
	copy(fields.begin(), fields.end(),
		inserter(res, res.begin()));
	return res;
}

void segy_bin_header::initialize() {
	fields = {
		{"job ID",					jobID()},
		{"job ID",					jobID()},
		{"line Num", 				lineNum()},
		{"reel Num",				reelNum()},
		{"num Traces", 				numTraces()},
		{"num Aux Traces", 			numAuxTraces()},
		{"sample Interval US", 		sampleIntUS()},
		{"sample Interval Orig US",	sampleIntOrigUS()},
		{"num Samples", 			numSamples()},
		{"num Samples Orig", 		numSamplesOrig()},
		{"data Sample Format",		dataSampleFormat()},
		{"fold", 					fold()},
		{"sort Code", 				sortCode()},
		{"vert Sum Code", 			vertSumCode()},
		{"sweep FreqStart", 		sweepFreqStart()},
		{"sweep Freq End", 			sweepFreqEnd()},
		{"sweep Code", 				sweepCode()},
		{"taper Type", 				taperType()},
		{"correlated Traces",		correlatedTraces()},
		{"gain Recovered",			gainRecovered()},
		{"amp Recovery Method",		ampRecoveryMethod()},
		{"unit System", 			unitSystem()},
		{"polarity ", 				polarity()},
		{"vib Polarity Code",		vibPolarityCode()},
		{"revision Num", 			revisionNum()},
		{"fixed Trace Length Flag", fixedTraceLengthFlag()},
		{"num Extended Blocks",		numExtendedBlocks()}
	};
}

#ifdef PYTHON
void py_segy_bin_header_init(py::module &m,
	py::class_<segy_bin_header, shared_ptr<segy_bin_header>> &py_segy_bin_header) {

}
#endif