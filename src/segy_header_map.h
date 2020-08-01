#pragma once

#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

#include <boost/preprocessor.hpp>

#include "seismic_header_map.h"
#include "segy_reader.h"
#include "segy_trace_header.h"

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif


class segy_header_map : public seismic_header_map {

/**
  * Segy bin header fieds tuple like (name, type_out, byte_offset, 
  *     byte_size, required_flag, brief_description) 
  * where
  *     name                - field name
  *     type_out            - type that use to store value in runtime
  *     byte_offset         - first byte of field (see segy doc.)
  *     required_flag       - is field required for reading file
  *     brief_description   - human readable description
  */
#define SyHM_FIELDS                                                         \
    ((inline,               seismic_data_type::INT,     188,    4,  1,      \
        "For 3-D poststack data, this field should be used for the "        \
        "in-line number"                                                    \
    ))                                                                      \
    ((crossline,            seismic_data_type::INT,     192,    4,  1,      \
        "For 3-D poststack data, this field should be used for the "        \
        "cross-line number"                                                 \
    ))                                                                      \
    ((CDP_X,                seismic_data_type::INT,     180,    4,  1,      \
        "X coordinate of ensemble (CDP) position of this trace "            \
        "(scalar in Standard TraceHeader bytes 71-72 applies)"              \
    ))                                                                      \
    ((CDP_Y,                seismic_data_type::INT,     184,    4,  1,      \
        "Y coordinate of ensemble (CDP) position of this trace "            \
        "(scalar in Standard TraceHeader bytes 71-72 applies)"              \
    ))                                                                      \
    ((CDP,                  seismic_data_type::INT,     20,     4,  1,      \
        "CDP, CMP, CRP, etc"                                                \
    ))                                                                      \
    ((src_X,                seismic_data_type::INT,     72,     4,  1,      \
        "Source coordinate - X"                                             \
    ))                                                                      \
    ((src_Y,                seismic_data_type::INT,     76,     4,  1,      \
        "Source coordinate - Y"                                             \
    ))                                                                      \
    ((samples_count,        seismic_data_type::SHORT,   114,    2,  1,      \
        "Number of samples in this trace"                                   \
    ))                                                                      \
    ((sample_interval,      seismic_data_type::SHORT,   116,    2,  1,      \
        "Sample interval for this trace"                                    \
    ))                                                                      \
    ((scalar_coords,        seismic_data_type::SHORT,   70,     2,  1,      \
        "Scalar to be applied to all coordinates specified in Standard "    \
        "Trace Header bytes73-88 and to bytes Trace Header 181-188 to give "\
        "the real value"                                                    \
    ))                                                                      \
    ((trace_units,          seismic_data_type::SHORT,   202,    2,  1,      \
        "Trace value measurement unit: -1 = Other; 0 = Unknown; "           \
        "1 = Pascal (Pa); 2 = Volts (v); 3 = Millivolts (mV); "             \
        "4 = Amperes (A); 5 = Meters (m); 6 = Meters per second (m/s);"     \
        "7 = Meters per second squared (m/s2); 8 = Newton (N); "            \
        "9 = Watt (W)"                                                      \
    ))                                                                      \
                                                                            \
    ((trc_seq_line,         seismic_data_type::INT,     0,      4,  0,      \
        "Trace sequence number within line - Numbers continue to "          \
        "increase if the sameline continues across multiple SEG-Y files"    \
    ))                                                                      \
    ((trc_seq_no,           seismic_data_type::INT,     4,      4,  0,      \
        "Trace sequence number within SEG-Y file - Each file starts "       \
        "with tracesequence one"                                            \
    ))                                                                      \
    ((orig_field_no,        seismic_data_type::INT,     8,      4,  0,      \
        "Original field record number"                                      \
    ))                                                                      \
    ((tcr_no,               seismic_data_type::INT,     12,     4,  0,      \
        "Trace number within the original field record"                     \
    ))                                                                      \
    ((energy_src_point,     seismic_data_type::INT,     16,     4,  0,      \
        "Energy source point number - Used when more than one record "      \
        "occurs at thesame effective surface location"                      \
    ))                                                                      \
    ((ensemble_trc_no,      seismic_data_type::INT,     24,     4,  0,      \
        "Trace number within the ensemble - Each ensemble starts with "     \
        "trace numberone"                                                   \
    ))                                                                      \
    ((trc_id,               seismic_data_type::SHORT,   28,     2,  0,      \
        "Trace identification code"                                         \
    ))                                                                      \
    ((vert_summed_trcs,     seismic_data_type::SHORT,   30,     2,  0,      \
        "Number of vertically summed traces yielding this trace. "          \
        "(1 is one trace, 2 is twosummed traces, etc.)"                     \
    ))                                                                      \
    ((hor_stacked_trcs,     seismic_data_type::SHORT,   32,     2,  0,      \
        "Number of horizontally stacked traces yielding this trace. "       \
        "(1 is one trace, 2 istwo stacked traces, etc.)"                    \
    ))                                                                      \
    ((data_use,             seismic_data_type::SHORT,   34,     2,  0,      \
        "Data use: 1 = Production; 2 = Test"                                \
    ))                                                                      \
    ((offset,               seismic_data_type::INT,     36,     4,  0,      \
        "Distance from center of the source point to the center of "        \
        "the receiver group(negative if opposite to direction in which "    \
        "line is shot)"                                                     \
    ))                                                                      \
    ((rec_elevation,        seismic_data_type::INT,     40,     4,  0,      \
        "Elevation of receiver group. This is, of course,normally equal to "\
        "or lower than the surface elevationat the group location"          \
    ))                                                                      \
    ((src_elevation,        seismic_data_type::INT,     44,     4,  0,      \
        "Surface elevation at source location"                              \
    ))                                                                      \
    ((src_depth,            seismic_data_type::INT,     48,     4,  0,      \
        "Source depth below surface"                                        \
    ))                                                                      \
    ((rec_datum,            seismic_data_type::INT,     52,     4,  0,      \
        "Seismic Datum13 elevation at receiver group"                       \
    ))                                                                      \
    ((src_datum,            seismic_data_type::INT,     56,     4,  0,      \
        "Seismic Datum elevation at source"                                 \
    ))                                                                      \
    ((src_wheight,          seismic_data_type::INT,     60,     4,  0,      \
        "Water column height at source location (at time ofsource event)"   \
    ))                                                                      \
    ((rec_wheight,          seismic_data_type::INT,     64,     4,  0,      \
        "Water column height at receiver group location (attime of "        \
        "recording of first source event into thatreceiver)"                \
    ))                                                                      \
    ((scalar_elevation,     seismic_data_type::SHORT,   68,     2,  0,      \
        "Scalar to be applied to all elevations and depths specified in "   \
        "Standard TraceHeader bytes 41-68 to give the real value"           \
    ))                                                                      \
                                                                            \
    ((group_X,              seismic_data_type::INT,     80,     4,  0,      \
        "Source coordinate - X"                                             \
    ))                                                                      \
    ((group_Y,              seismic_data_type::INT,     84,     4,  0,      \
        "Source coordinate - Y"                                             \
    ))                                                                      \
    ((coord_units,          seismic_data_type::USHORT,  88,     2,  0,      \
        "Coordinate units: 1 = Length; 2 = Seconds of arc; "                \
        "3 = Decimal degrees; 4 = Degrees, minutes, seconds"                \
    ))                                                                      \
                                                                            \
    ((weather_vel,          seismic_data_type::SHORT,   90,     2,  0,      \
        "Weathering velocity (ft/s or m/s as specified in Binary File "     \
        "Header bytes 3255-3256)"                                           \
    ))                                                                      \
    ((subweather_vel,       seismic_data_type::SHORT,   92,     2,  0,      \
        "Subweathering velocity. (ft/s or m/s as specified in Binary "      \
        "File Header bytes 3255-3256)"                                      \
    ))                                                                      \
    ((src_uphole_time,      seismic_data_type::USHORT,  94,     2,  0,      \
        "Uphole time at source in milliseconds"                             \
    ))                                                                      \
    ((group_uphole_time,    seismic_data_type::USHORT,  96,     2,  0,      \
        "Uphole time at source in milliseconds"                             \
    ))                                                                      \
    ((src_stat_corr,        seismic_data_type::SHORT,   98,     2,  0,      \
        "Source static correction in milliseconds"                          \
    ))                                                                      \
    ((group_stat_corr,      seismic_data_type::SHORT,   100,    2,  0,      \
        "Group static correction in milliseconds"                           \
    ))                                                                      \
    ((total_static,         seismic_data_type::SHORT,   102,    2,  0,      \
        "Total static applied in milliseconds (Zero if no static hasbeen "  \
        "applied)"                                                          \
    ))                                                                      \
    ((lag_A,                seismic_data_type::SHORT,   104,    2,  0,      \
        "Lag time A - Time in milliseconds between end of 240-bytetrace "   \
        "identification header and time break."                             \
    ))                                                                      \
    ((lag_B,                seismic_data_type::SHORT,   106,    2,  0,      \
        "Lag Time B - Time in milliseconds between time break andthe "      \
        "initiation time of the energy source"                              \
    ))                                                                      \
    ((rec_delay,            seismic_data_type::SHORT,   108,    2,  0,      \
        "Delay recording time - Time in milliseconds betweeninitiation time"\
        " of energy source and the time when recordingof data samples "     \
        "begins"                                                            \
    ))                                                                      \
    ((mute_start,           seismic_data_type::SHORT,   110,    2,  0,      \
        "Mute time - Start time in milliseconds"                            \
    ))                                                                      \
    ((mute_end,             seismic_data_type::SHORT,   112,    2,  0,      \
        "Mute time - Start time in milliseconds"                            \
    ))                                                                      \
    ((gain_type,            seismic_data_type::USHORT,  118,    2,  0,      \
        "Gain type of field instruments: 1 = fixed; 2 = binary; "           \
        "3 = floating point"                                                \
    ))                                                                      \
    ((gain_const,           seismic_data_type::SHORT,   120,    2,  0,      \
        "Instrument gain constant"                                          \
    ))                                                                      \
    ((gain_init,            seismic_data_type::SHORT,   122,    2,  0,      \
        "Instrument gain constant"                                          \
    ))                                                                      \
    ((correlated,           seismic_data_type::USHORT,  124,    2,  0,      \
        "Correlated: 1 = no; 2 = yes"                                       \
    ))                                                                      \
    ((sweep_freq_start,     seismic_data_type::SHORT,   126,    2,  0,      \
        "Sweep frequency at start (Hz)"                                     \
    ))                                                                      \
    ((sweep_freq_end,       seismic_data_type::SHORT,   128,    2,  0,      \
        "Sweep frequency at end (Hz)"                                       \
    ))                                                                      \
    ((sweep_len,            seismic_data_type::SHORT,   130,    2,  0,      \
        "Sweep length in milliseconds"                                      \
    ))                                                                      \
    ((sweep_type,           seismic_data_type::USHORT,  132,    2,  0,      \
        "Sweep type: 1 = linear; 2 = parabolic; 3 = exponential; 4 = other" \
    ))                                                                      \
    ((taper_len_start,      seismic_data_type::USHORT,  134,    2,  0,      \
        "Sweep trace taper length at start in milliseconds"                 \
    ))                                                                      \
    ((taper_len_end,        seismic_data_type::USHORT,  136,    2,  0,      \
        "Sweep trace taper length at end in milliseconds"                   \
    ))                                                                      \
    ((taper_type,           seismic_data_type::USHORT,  138,    2,  0,      \
        "Taper type: 1 = linear; 2 = cos^2; 3 = other"                      \
    ))                                                                      \
    ((alias_filter_freq,    seismic_data_type::USHORT,  140,     2,  0,     \
        "Alias filter frequency (Hz), if used"                              \
    ))                                                                      \
    ((alias_filter_slope,   seismic_data_type::USHORT,  142,    2,  0,      \
        "Alias filter slope (dB/octave)"                                    \
    ))                                                                      \
    ((notch_filter_freq,    seismic_data_type::USHORT,  144,    2,  0,      \
        "Notch filter frequency (Hz), if used"                              \
    ))                                                                      \
    ((notch_filter_slope,   seismic_data_type::USHORT,  146,    2,  0,      \
        "Notch filter slope (dB/octave)"                                    \
    ))                                                                      \
    ((low_cut_freq,         seismic_data_type::USHORT,  148,    2,  0,      \
        "Low-cut frequency (Hz), if used"                                   \
    ))                                                                      \
    ((high_cut_freq,        seismic_data_type::USHORT,  150,    2,  0,      \
        "High-cut frequency (Hz), if used"                                  \
    ))                                                                      \
    ((low_cut_slope,        seismic_data_type::SHORT,   152,    2,  0,      \
        "Low-cut slope (dB/octave)"                                         \
    ))                                                                      \
    ((high_cut_slope,       seismic_data_type::SHORT,   154,    2,  0,      \
        "High-cut slope (dB/octave)"                                        \
    ))                                                                      \
    ((year,                 seismic_data_type::USHORT,  156,    2,  0,      \
        "Year data recorded"                                                \
    ))                                                                      \
    ((day,                  seismic_data_type::USHORT,  158,    2,  0,      \
        "Day of year data recorded"                                         \
    ))                                                                      \
    ((hour,                 seismic_data_type::USHORT,  160,    2,  0,      \
        "Hour of day (24 hour clock) data recorded"                         \
    ))                                                                      \
    ((minute,               seismic_data_type::USHORT,  162,    2,  0,      \
        "Minute of hour data recorded"                                      \
    ))                                                                      \
    ((second,               seismic_data_type::USHORT,  164,    2,  0,      \
        "Second of minute data recorded"                                    \
    ))                                                                      \
    ((time_code,            seismic_data_type::USHORT,  166,    2,  0,      \
        "Time basis code: 1 = Local; 2 = GMT (Greenwich Mean Time); "       \
        "3 = Other, should be explained in a user defined stanza in the "   \
        "Extended Textual File Header; 4 = UTC (Coordinated Universal "     \
        "Time); 5 = GPS (Global Positioning System Time)"                   \
    ))                                                                      \
    ((trc_w_factor,         seismic_data_type::USHORT,  168,    2,  0,      \
        "Trace weighting factor"                                            \
    ))                                                                      \
    ((gph_grp_roll,         seismic_data_type::USHORT,  170,    2,  0,      \
        "Geophone group number of roll switch position one"                 \
    ))                                                                      \
    ((gph_grp_trc,          seismic_data_type::USHORT,  172,    2,  0,      \
        "Geophone group number of trace number one within original field "  \
        "record"                                                            \
    ))                                                                      \
    ((gph_grp_lst_trc,      seismic_data_type::USHORT,  174,    2,  0,      \
        "Geophone group number of last trace within original field record"  \
    ))                                                                      \
    ((gap_size,             seismic_data_type::USHORT,  176,    2,  0,      \
        "Gap size (total number of groups dropped)"                         \
    ))                                                                      \
    ((otrav,                seismic_data_type::USHORT,  178,    2,  0,      \
        "Over travel associated with taper at beginning or end of line:"    \
        "1 = down (or behind); 2 = up (or ahead)"                           \
    ))                                                                      \
    ((dhotpoint_no,         seismic_data_type::INT,     196,    4,  0,      \
        ""                                                                  \
    ))                                                                      \
    ((shotpoint_scalar,     seismic_data_type::SHORT,   200,    2,  0,      \
        "Scalar to be applied to the shotpoint number",                     \
    ))                                                                      \
    ((trans_const_mant,     seismic_data_type::INT,     204,    4,  0,      \
        "Transduction Constant mantissa"                                    \
    ))                                                                      \
    ((trans_const_pow,      seismic_data_type::SHORT,   208,    2,  0,      \
        "Transduction Constant power"                                       \
    ))                                                                      \
    ((trans_units,          seismic_data_type::SHORT,   210,    2,  0,      \
        "Transduction Units: -1 = Other; 0 = Unknown; 1 = Pascal (Pa); "    \
        "2 = Volts (v); 3 = Millivolts (mV); 4 = Amperes (A); "             \
        "5 = Meters (m); 6 = Meters per second (m/s); "                     \
        "7 = Meters per second squared (m/s2); 8 = Newton (N); 9 = Watt (W)"\
    ))                                                                      \
    ((trc_id,               seismic_data_type::SHORT,   212,    2,  0,      \
        "Device/Trace Identifier - The unit number or id number "           \
        "of the device associatedwith the Data Trace"                       \
    ))                                                                      \
    ((time_scal,           seismic_data_type::SHORT,    214,    2,  0,      \
        "Scalar to be applied to times specified in Trace Header bytes "    \
        "95-114 to give the1 time value in milliseconds"                    \
    ))                                                                      \
    ((scr_type_orien,      seismic_data_type::SHORT,    216,    2,  0,      \
        "Source Type/Orientation - Defines the type and the orientation of "\
        "the energysource. The terms vertical, cross-line and in-line "     \
        "refer to the three axes of anorthogonal coordinate system. "       \
        "The absolute azimuthal orientation of thecoordinate system axes "  \
        "should be defined in the CRS.\n-1 to -n = Other; 0 = Unknown; "    \
        "1 = Vibratory - Vertical orientation; "                            \
        "2 = Vibratory - Cross-line orientation; "                          \
        "3 = Vibratory - In-line orientation; "                             \
        "4 = Impulsive - Vertical orientation; "                            \
        "5 = Impulsive - Cross-line orientation; "                          \
        "6 = Impulsive - In-line orientation; "                             \
        "7 = Distributed Impulsive - Vertical orientation; "                \
        "8 = Distributed Impulsive - Cross-line orientation; "              \
        "9 = Distributed Impulsive - In-line orientation"                   \
    ))                                                                      \
    ((src_units,           seismic_data_type::SHORT,    230,    2,  0,      \
        "Source Measurement Unit: -1 = Other; 0 = Unknown; 1 = Joule (J); " \
        "2 = Kilowatt (kW); 3 = Pascal (Pa); 4 = Bar (Bar); "               \
        "4 = Bar-meter (Bar-m); 5 = Newton (N); 6 = Kilograms (kg)"         \
    ))                                                                      \

    static storage_t                    m_fields_required;
    bool                                m_update_hash;
    size_t                              m_hash;

public:
    segy_header_map();
	segy_header_map(header_map_type type);
    segy_header_map(const map_storage_t &map);
	segy_header_map(const segy_header_map &map);
	segy_header_map(std::shared_ptr<segy_header_map> map);
    segy_header_map(std::shared_ptr<seismic_header_map> map);

    virtual void set(
        std::string         name,
        header_field_t      val
    );

    virtual void set(
        std::string         name,
        header_field_t      val,
        std::string         desc
    );

    virtual void set(
        std::string         name,
        int                 byte_loc,
        int                 byte_size,
        seismic_data_type   type
    );

    virtual void set(
        std::string         name,
        int                 byte_loc,
        int                 byte_size,
        seismic_data_type   type,
        std::string         desc
    );

    virtual void                        remove(
        const std::string &name
    );

    virtual void                        clear();

    virtual bool                        contains(
        const std::string &name
    ) const;

    virtual header_field_t              get(
        const std::string &name
    ) const;

    virtual std::pair<std::string, header_field_t>
                                        get(int index) const;

    virtual size_t                      count() const;
    virtual header_map_type             type() const;

    // Map kind of { name : (data_type, byte_position, byte_size, required_flag) }
    virtual map_storage_t               to_map() const;

    virtual void                        from_map(
        const map_storage_t &m
    );

    virtual 
    std::map<std::string, std::string>  fields_descr();

	virtual object_type                 type_id() {
         return object_type::SEGY_HEADERMAP; 
    }

	friend class segy_reader;
	friend class segy_trace_header;

    virtual bool                        operator==(
        std::shared_ptr<seismic_header_map> map
    );
    bool                                operator==(
        std::shared_ptr<segy_header_map> map
    );
    bool                                operator==(
        segy_header_map &map
    );

private:
    int                                 contains(
        const std::string &name, 
        const seismic_header_map::storage_t &map
    ) const;

    void                                fill_required();
    void                                fill_standard();

    void                                calc_hash();
};

#ifdef PYTHON
void py_segy_header_map_init(
    py::module &m,
	py::class_<segy_header_map, std::shared_ptr<segy_header_map>> &py_segy_header_map
);
#endif
