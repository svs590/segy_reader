#ifndef __SEGY_BIN_HEADER
#define __SEGY_BIN_HEADER

#include <map>
#include <string>
#include <vector>

#include "seismic_abstract_header.h"

class segy_bin_header : public seismic_abstract_header {
	void *obj = nullptr;
	std::vector<std::pair<std::string, int>> fields;
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

#endif
