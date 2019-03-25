#ifndef __SEISMIC_GEOMETRY
#define __SEISMIC_GEOMETRY

#include <string>
#include <vector>

#include "seismic_data_provider.h"
#include "segy_reader.h"

class seismic_line_info {
public:
	enum class seismic_line_type { iline, xline, none };
protected:
	std::string											name;
	std::pair<float, float>								start;
	std::pair<float, float>								end;
	int													line_no;
	seismic_line_type									type;
	std::vector<int>									traces_indexes;

	bool by_bounds = false;
	std::vector<std::pair<int, int>> bounds;
	int start_trace = NOT_INDEX;
	int end_trace = NOT_INDEX;
	int sequential_traces_count = 0;

public:
	seismic_line_info() {}
	seismic_line_info(std::string name,
		std::pair<float, float> start,
		std::pair<float, float> end,
		int line_no,
		seismic_line_type type
	) {
		this->name = name;
		this->start = start;
		this->end = end;
		this->line_no;
		this->type = type;
	}

	std::pair<float, float>		get_start()	{ return start; }
	std::pair<float, float>		get_end()	{ return end; }
	std::string					get_name()	{ return name; }

	friend class segy_reader;

protected:
	void add_trace_index(int index) {
		traces_indexes.push_back(index);
	}
};

class seismic_geometry_info {
	std::vector<seismic_line_info> lines;
public:
	seismic_geometry_info() {}
	seismic_geometry_info(std::vector<seismic_line_info> lines) {
		this->lines = lines;
	}

	std::vector<seismic_line_info> get_lines() { return lines; }

	friend class segy_reader;

protected:

};

#endif
