#ifndef __SEISMIC_GEOMETRY
#define __SEISMIC_GEOMETRY

#include <string>
#include <vector>

#include "utils.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif

class segy_reader;

class seismic_line_info {
public:
	enum class seismic_line_type { iline, xline, none };
protected:
	std::string											f_name;
	std::pair<float, float>								f_start_point;
	std::pair<float, float>								f_end_point;
	int													f_line_no;
	seismic_line_type									f_type;
	std::vector<int>									f_traces_indexes;

	bool                                                f_by_bounds = false;
	std::vector<std::pair<int, int>>                    f_bounds;
	int                                                 f_start_trace = NOT_INDEX;
	int                                                 f_end_trace = NOT_INDEX;
	int                                                 f_sequential_traces_count = 0;
    int                                                 f_traces_count = 0;

public:
	seismic_line_info() {}
	seismic_line_info(std::string name,
		std::pair<float, float> start,
		std::pair<float, float> end,
		int line_no,
		seismic_line_type type
	) {
		this->f_name = name;
		this->f_start_point = start;
		this->f_end_point = end;
		this->f_line_no;
		this->f_type = type;
	}

	std::pair<float, float>		start_point()	{ return f_start_point; }
	std::pair<float, float>		end_point()	{ return f_end_point; }
	std::string					name()	{ return f_name; }

	friend class segy_reader;

protected:
	void add_trace_index(int index) {
		f_traces_indexes.push_back(index);
	}
};

class seismic_geometry_info {
public:
    enum class seismic_geometry_type { g2D, g3D, unknown };
private:
	std::vector<seismic_line_info> f_ilines;
    std::vector<seismic_line_info> f_crosslines;
    seismic_geometry_type f_gtype = seismic_geometry_type::unknown;
public:
	seismic_geometry_info() {}
	seismic_geometry_info(
        const std::vector<seismic_line_info> &ilines, 
        const std::vector<seismic_line_info> &crosslines,
        seismic_geometry_type gtype
    ) {
		f_ilines = ilines;
        f_crosslines = crosslines;
        f_gtype = gtype;
	}

    seismic_geometry_type geometry_type() { return f_gtype; }

	std::vector<seismic_line_info> lines() { 
        std::vector<seismic_line_info> res(f_ilines.size() + f_crosslines.size());
        std::copy(f_ilines.begin(), f_ilines.end(), res.begin());
        std::copy(f_crosslines.begin(), f_crosslines.end(), res.begin() + f_ilines.size());
        return res;
    }

    std::vector<seismic_line_info> lines(seismic_line_info::seismic_line_type line_type) {
        switch (line_type)
        {
        case seismic_line_info::seismic_line_type::iline:
            return f_ilines;
            break;
        case seismic_line_info::seismic_line_type::xline:
            return f_crosslines;
            break;
        case seismic_line_info::seismic_line_type::none:
            return {};
            break;
        default:
            break;
        }
    }

	friend class segy_reader;

protected:

};

#ifdef PYTHON
void py_seismic_geometry_info_init(py::module &m);
void py_seismic_line_info_init(py::module &m);
#endif

#endif
