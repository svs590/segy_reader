#include "segy_bin_header.h"
#include "utils.h"

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

segy_bin_header::segy_bin_header(const void *obj) {
	this->obj = const_cast<void*>(obj);
}
segy_bin_header& segy_bin_header::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	return *this;
}
segy_bin_header::~segy_bin_header() {

}

int				segy_bin_header::jobID() {
	return cseis_csNativeSegyBinHeader_jobID(obj);
}
int				segy_bin_header::lineNum() {
	return cseis_csNativeSegyBinHeader_lineNum(obj);
}
int				segy_bin_header::reelNum() {
	return cseis_csNativeSegyBinHeader_reelNum(obj);
}
int				segy_bin_header::numTraces() {
	return cseis_csNativeSegyBinHeader_numTraces(obj);
}
int				segy_bin_header::numAuxTraces() {
	return cseis_csNativeSegyBinHeader_numAuxTraces(obj);
}
int				segy_bin_header::sampleIntUS() {
	return cseis_csNativeSegyBinHeader_sampleIntUS(obj);
}
int				segy_bin_header::sampleIntOrigUS() {
	return cseis_csNativeSegyBinHeader_sampleIntOrigUS(obj);
}
int				segy_bin_header::numSamples() {
	return cseis_csNativeSegyBinHeader_numSamples(obj);
}
int				segy_bin_header::numSamplesOrig() {
	return cseis_csNativeSegyBinHeader_numSamplesOrig(obj);
}
unsigned short	segy_bin_header::dataSampleFormat() {
	return cseis_csNativeSegyBinHeader_dataSampleFormat(obj);
}
unsigned short	segy_bin_header::fold() {
	return cseis_csNativeSegyBinHeader_fold(obj);
}
unsigned short	segy_bin_header::sortCode() {
	return cseis_csNativeSegyBinHeader_sortCode(obj);
}
unsigned short	segy_bin_header::vertSumCode() {
	return cseis_csNativeSegyBinHeader_vertSumCode(obj);
}
unsigned short	segy_bin_header::sweepFreqStart() {
	return cseis_csNativeSegyBinHeader_sweepFreqStart(obj);
}
unsigned short	segy_bin_header::sweepFreqEnd() {
	return cseis_csNativeSegyBinHeader_sweepFreqEnd(obj);
}
unsigned short	segy_bin_header::sweepCode() {
	return cseis_csNativeSegyBinHeader_sweepCode(obj);
}
unsigned short	segy_bin_header::taperType() {
	return cseis_csNativeSegyBinHeader_taperType(obj);
}
unsigned short	segy_bin_header::correlatedTraces() {
	return cseis_csNativeSegyBinHeader_correlatedTraces(obj);
}
unsigned short	segy_bin_header::gainRecovered() {
	return cseis_csNativeSegyBinHeader_gainRecovered(obj);
}
unsigned short	segy_bin_header::ampRecoveryMethod() {
	return cseis_csNativeSegyBinHeader_ampRecoveryMethod(obj);
}
unsigned short	segy_bin_header::unitSystem() {
	return cseis_csNativeSegyBinHeader_unitSystem(obj);
}
unsigned short	segy_bin_header::polarity() {
	return cseis_csNativeSegyBinHeader_polarity(obj);
}
unsigned short	segy_bin_header::vibPolarityCode() {
	return cseis_csNativeSegyBinHeader_vibPolarityCode(obj);
}
unsigned short	segy_bin_header::revisionNum() {
	return cseis_csNativeSegyBinHeader_revisionNum(obj);
}
unsigned short	segy_bin_header::fixedTraceLengthFlag() {
	return cseis_csNativeSegyBinHeader_fixedTraceLengthFlag(obj);
}
unsigned short	segy_bin_header::numExtendedBlocks() {
	return cseis_csNativeSegyBinHeader_numExtendedBlocks(obj);
}