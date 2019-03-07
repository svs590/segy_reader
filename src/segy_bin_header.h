#ifndef SEGY_BIN_HEADER
#define SEGY_BIN_HEADER

class segy_bin_header {
	void *obj;
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
};

#endif
