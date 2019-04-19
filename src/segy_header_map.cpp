#include <string>

#include "segy_header_map.h"
#include "data_types.h"
#include "utils.h"

using namespace std;


/*
    Required fields
*/
const segy_header_map::map_storage
segy_header_map::map_standard_req = {
    { "Inline", {188, 4, seismic_data_type::INT,
        "For 3-D poststack data, this field should be used for the in-line number"} },
    { "Crossline", {192, 4, seismic_data_type::INT,
        "For 3-D poststack data, this field should be used for the cross-line number"} },
    { "CDP X", {180, 4, seismic_data_type::INT,
        "X coordinate of ensemble (CDP) position of this trace "
        "(scalar in Standard TraceHeader bytes 71-72 applies)"} },
    { "CDP Y", {184, 4, seismic_data_type::INT,
        "Y coordinate of ensemble (CDP) position of this trace "
        "(scalar in Standard TraceHeader bytes 71-72 applies)"} },
    { "CDP", {20, 4, seismic_data_type::INT,
        "CDP, CMP, CRP, etc"} },
    { "Src X", {72, 4, seismic_data_type::INT,
        "Source coordinate - X"} },
    { "Src Y", {76, 4, seismic_data_type::INT,
        "Source coordinate - X"} },
    { "Samples count", {114, 2, seismic_data_type::SHORT,
        "Number of samples in this trace"} },
    { "Sample interval", {116, 2, seismic_data_type::SHORT,
        "Sample interval for this trace"} }
};

/*
    Other fields
*/
const segy_header_map::map_storage
segy_header_map::map_standard = {
    { "Trc seq line", {0, 4, seismic_data_type::INT,
        "Trace sequence number within line - Numbers continue to "
        "increase if the sameline continues across multiple SEG-Y files"} },
    { "Trc seq no", {4, 4, seismic_data_type::INT,
        "Trace sequence number within SEG-Y file - Each file starts "
        "with tracesequence one"} },
    { "Orig field no", {8, 4, seismic_data_type::INT,
        "Original field record number"} },
    { "Tcr no", {12, 4, seismic_data_type::INT,
        "Trace number within the original field record"} },
    { "Energy src point", {16, 4, seismic_data_type::INT,
        "Energy source point number - Used when more than one record "
        "occurs at thesame effective surface location"} },
    { "Ensemble trc no", {24, 4, seismic_data_type::INT,
        "Trace number within the ensemble - Each ensemble starts with "
        "trace numberone"} },
    { "Trc id", {28, 2, seismic_data_type::SHORT,
        "Trace identification code"} },
    { "Vert summed trcs", {30, 2, seismic_data_type::SHORT,
        "Number of vertically summed traces yielding this trace. "
        "(1 is one trace, 2 is twosummed traces, etc.)"} },
    { "Hor stacked trcs", {32, 2, seismic_data_type::SHORT,
        "Number of horizontally stacked traces yielding this trace. "
        "(1 is one trace, 2 istwo stacked traces, etc.)"} },
    { "Data use", {34, 2, seismic_data_type::SHORT,
        "Data use: 1 = Production; 2 = Test"} },
    { "Offset", {36, 4, seismic_data_type::INT,
        "Distance from center of the source point to the center of "
        "the receiver group(negative if opposite to direction in which line is shot)"} },
    { "Rec elevation", {40, 4, seismic_data_type::INT,
        "Elevation of receiver group. This is, of course,normally equal to or "
        "lower than the surface elevationat the group location"} },
    { "Src elevation", {44, 4, seismic_data_type::INT,
        "Surface elevation at source location"} },
    { "Src depth", {48, 4, seismic_data_type::INT,
        "Source depth below surface"} },
    { "Rec datum", {52, 4, seismic_data_type::INT,
        "Seismic Datum13 elevation at receiver group"} },
    { "Src datum", {56, 4, seismic_data_type::INT,
        "Seismic Datum elevation at source"} },
    { "Src wheight", {60, 4, seismic_data_type::INT,
        "Water column height at source location (at time ofsource event)"} },
    { "Rec wheight", {64, 4, seismic_data_type::INT,
        "Water column height at receiver group location (attime of recording "
        "of first source event into thatreceiver)"} },
    { "Scalar elevation", {68, 2, seismic_data_type::SHORT,
        "Scalar to be applied to all elevations and depths specified in "
        "Standard TraceHeader bytes 41-68 to give the real value"} },
    { "Scalar coords", {70, 2, seismic_data_type::SHORT,
        "Scalar to be applied to all coordinates specified in Standard "
        "Trace Header bytes73-88 and to bytes Trace Header 181-188 to give "
        "the real value"} },

    { "Group Y", {80, 4, seismic_data_type::INT,
        "Source coordinate - X"} },
    { "Group Y", {84, 4, seismic_data_type::INT,
        "Source coordinate - X"} },
    { "Coord units", {88, 2, seismic_data_type::USHORT,
        "Coordinate units: 1 = Length; 2 = Seconds of arc; 3 = Decimal degrees; "
        "4 = Degrees, minutes, seconds"} },

    { "Weather vel", {90, 2, seismic_data_type::SHORT,
        "Weathering velocity (ft/s or m/s as specified in Binary File Header "
        "bytes 3255-3256)"} },
    { "Subweather vel", {92, 2, seismic_data_type::SHORT,
        "Subweathering velocity. (ft/s or m/s as specified in Binary File Header "
        "bytes 3255-3256)"} },
    { "Src uphole time", {94, 2, seismic_data_type::USHORT,
        "Uphole time at source in milliseconds"} },
    { "Group uphole time", {96, 2, seismic_data_type::USHORT,
        "Uphole time at source in milliseconds"} },
    { "Src stat corr", {98, 2, seismic_data_type::SHORT,
        "Source static correction in milliseconds"} },
    { "Group stat corr", {100, 2, seismic_data_type::SHORT,
        "Group static correction in milliseconds"} },
    { "Total static", {102, 2, seismic_data_type::SHORT,
        "Total static applied in milliseconds (Zero if no static hasbeen applied)"} },
    { "Lag A", {104, 2, seismic_data_type::SHORT,
        "Lag time A - Time in milliseconds between end of 240-bytetrace identification "
        "header and time break."} },
    { "Lag B", {106, 2, seismic_data_type::SHORT,
        "Lag Time B - Time in milliseconds between time break andthe initiation "
        "time of the energy source"} },
    { "Rec delay", {108, 2, seismic_data_type::SHORT,
        "Delay recording time - Time in milliseconds betweeninitiation time of energy "
        "source and the time when recordingof data samples begins"} },
    { "Mute start", {110, 2, seismic_data_type::SHORT,
        "Mute time - Start time in milliseconds"} },
    { "Mute end", {112, 2, seismic_data_type::SHORT,
        "Mute time - Start time in milliseconds"} },
    { "Gain type", {118, 2, seismic_data_type::USHORT,
        "Gain type of field instruments: 1 = fixed; 2 = binary; 3 = floating point"} },
    { "Gain const", {120, 2, seismic_data_type::SHORT,
        "Instrument gain constant"} },
    { "Gain init", {122, 2, seismic_data_type::SHORT,
         "Instrument gain constant"} },
    { "Correlated", {124, 2, seismic_data_type::USHORT,
         "Correlated: 1 = no; 2 = yes"} },
    { "Sweep freq start", {126, 2, seismic_data_type::SHORT,
         "Sweep frequency at start (Hz)"} },
    { "Sweep freq end", {128, 2, seismic_data_type::SHORT,
         "Sweep frequency at end (Hz)"} },
    { "Sweep len", {130, 2, seismic_data_type::SHORT,
         "Sweep length in milliseconds"} },
    { "Sweep type", {132, 2, seismic_data_type::USHORT,
         "Sweep type: 1 = linear; 2 = parabolic; 3 = exponential; 4 = other"} },
    { "Taper len start", {134, 2, seismic_data_type::USHORT,
         "Sweep trace taper length at start in milliseconds"} },
    { "Taper len end", {136, 2, seismic_data_type::USHORT,
         "Sweep trace taper length at end in milliseconds"} },
    { "Taper type", {138, 2, seismic_data_type::USHORT,
         "Taper type: 1 = linear; 2 = cos^2; 3 = other"} },
    { "Alias filter freq", {140, 2, seismic_data_type::USHORT,
         "Alias filter frequency (Hz), if used"} },
    { "Alias filter slope", {142, 2, seismic_data_type::USHORT,
         "Alias filter slope (dB/octave)"} },
    { "Notch filter freq", {144, 2, seismic_data_type::USHORT,
         "Notch filter frequency (Hz), if used"} },
    { "Notch filter slope", {146, 2, seismic_data_type::USHORT,
         "Notch filter slope (dB/octave)"} },
    { "Low-cut freq", {148, 2, seismic_data_type::USHORT,
         "Low-cut frequency (Hz), if used"} },
    { "High-cut freq", {150, 2, seismic_data_type::USHORT,
         "High-cut frequency (Hz), if used"} },
    { "Low-cut slope", {152, 2, seismic_data_type::SHORT,
         "Low-cut slope (dB/octave)"} },
    { "High-cut slope", {154, 2, seismic_data_type::SHORT,
         "High-cut slope (dB/octave)"} },
    { "Year", {156, 2, seismic_data_type::USHORT,
         "Year data recorded"} },
    { "Day", {158, 2, seismic_data_type::USHORT,
         "Day of year"} },
    { "Hour", {160, 2, seismic_data_type::USHORT,
         "Hour of day (24 hour clock)"} },
    { "Minute", {162, 2, seismic_data_type::USHORT,
         "Minute of hour"} },
    { "Second", {164, 2, seismic_data_type::USHORT,
         "Second of minute"} },
    { "Time code", {166, 2, seismic_data_type::USHORT,
        "Time basis code: 1 = Local; 2 = GMT (Greenwich Mean Time); "
        "3 = Other, should be explained in a user defined stanza in the "
        "Extended Textual File Header; 4 = UTC (Coordinated Universal Time); "
        "5 = GPS (Global Positioning System Time)"} },
    { "Trc w factor", {168, 2, seismic_data_type::USHORT,
         "Trace weighting factor"} },
    { "Gph grp roll", {170, 2, seismic_data_type::USHORT,
         "Geophone group number of roll switch position one"} },
    { "Gph grp trc", {172, 2, seismic_data_type::USHORT,
         "Geophone group number of trace number one within original field record"} },
    { "Gph grp lst trc", {174, 2, seismic_data_type::USHORT,
         "Geophone group number of last trace within original field record"} },
    { "Gap size", {176, 2, seismic_data_type::USHORT,
         "Gap size (total number of groups dropped)"} },
    { "Otrav", {178, 2, seismic_data_type::USHORT,
         "Over travel associated with taper at beginning or end of line:"
         "1 = down (or behind); 2 = up (or ahead)"} },   
    { "Shotpoint no", {196, 4, seismic_data_type::INT,
        ""} },
    { "Shotpoint scalar", {200, 2, seismic_data_type::SHORT,
        "Scalar to be applied to the shotpoint number"} },
    { "Trace units", {202, 2, seismic_data_type::SHORT,
        "Trace value measurement unit: -1 = Other; 0 = Unknown; 1 = Pascal (Pa); "
        "2 = Volts (v); 3 = Millivolts (mV); 4 = Amperes (A); 5 = Meters (m); "
        "6 = Meters per second (m/s); 7 = Meters per second squared (m/s2); 8 = Newton (N); "
        "9 = Watt (W"} },
    { "Trans const mant", {204, 4, seismic_data_type::INT,
        "Transduction Constant mantissa. The multiplicative constant used to convert the DataTrace "
        "samples to the Transduction Units"} },
    { "Trans const mant", {208, 2, seismic_data_type::SHORT,
        "Transduction Constant power. The multiplicative constant used to convert the DataTrace "
        "samples to the Transduction Units"} },
    { "Trans units", {210, 2, seismic_data_type::SHORT,
        "Transduction Units - The unit of measurement of the Data Trace samples afterthey "
        "have been multiplied by the Transduction Constant: -1 = Other; 0 = Unknown; 1 = Pascal (Pa); "
        "2 = Volts (v); 3 = Millivolts (mV); 4 = Amperes (A); 5 = Meters (m); 6 = Meters per second (m/s); "
        "7 = Meters per second squared (m/s2); 8 = Newton (N); 9 = Watt (W)"} },
    { "Trc id", {212, 2, seismic_data_type::SHORT,
        "Device/Trace Identifier - The unit number or id number of the device associatedwith the Data Trace"} },
    { "Time scal", {214, 2, seismic_data_type::SHORT,
        "Scalar to be applied to times specified in Trace Header bytes 95-114 to give "
        "thetrue time value in milliseconds"} },
    { "Scr type/orien", {216, 2, seismic_data_type::SHORT,
        "Source Type/Orientation - Defines the type and the orientation of the energysource. "
        "The terms vertical, cross-line and in-line refer to the three axes of anorthogonal "
        "coordinate system. The absolute azimuthal orientation of thecoordinate system axes "
        "should be defined in the CRS.\n-1 to -n = Other; 0 = Unknown; "
        "1 = Vibratory - Vertical orientation; 2 = Vibratory - Cross-line orientation; "
        "3 = Vibratory - In-line orientation; 4 = Impulsive - Vertical orientation; "
        "5 = Impulsive - Cross-line orientation; 6 = Impulsive - In-line orientation; "
        "7 = Distributed Impulsive - Vertical orientation; "
        "8 = Distributed Impulsive - Cross-line orientation; "
        "9 = Distributed Impulsive - In-line orientation"} },

    /*
    { "Scr enrg dir", {218, 6, seismic_data_type::UNKNOWN,
        "Source Energy Direction with respect to the source orientation"} },
    { "Scr measure", {224, 6, seismic_data_type::UNKNOWN,
        "Source Measurement - Describes the source effort used to generate the trace"} },
    */

    { "Src units", {230, 2, seismic_data_type::SHORT,
        "Source Measurement Unit: -1 = Other; 0 = Unknown; 1 = Joule (J); 2 = Kilowatt (kW); "
        "3 = Pascal (Pa); 4 = Bar (Bar); 4 = Bar-meter (Bar-m); 5 = Newton (N); 6 = Kilograms (kg)"} },

};

segy_header_map::segy_header_map() : segy_header_map(header_map_type::STANDARD) {}

segy_header_map::segy_header_map(header_map_type type) {
    f_type = type;
    switch (f_type)
    {
    case header_map_type::CUSTOM:
        f_map = map_standard_req;
        break;
    case header_map_type::STANDARD:
        f_map = map_standard_req;
        for (auto p : map_standard)
            f_map.push_back({ p.first, p.second });
        break;
    default:
        break;
    }
}

segy_header_map::segy_header_map(const segy_header_map &map) {
    f_type = map.f_type;
    f_map = map.f_map;
}

segy_header_map::segy_header_map(std::shared_ptr<segy_header_map> map) {
    f_type = map->f_type;
    f_map = map->f_map;
}

segy_header_map::segy_header_map(std::shared_ptr<seismic_header_map> map) {
    from_map(map->to_map());
}

void segy_header_map::set_field(
    string name,
    int byte_loc,
    int byte_size,
    seismic_data_type type,
    string desc
) {
    int index = contains(name);
    if (index != NOT_INDEX) {
        if (desc.empty())
            f_map[index].second = { byte_loc, byte_size, type, std::get<3>(f_map[index].second) };
        else
            f_map[index].second = { byte_loc, byte_size, type, desc };
    }
    else
        f_map.push_back({ name, { byte_loc, byte_size, type, desc } });
}

void segy_header_map::remove(const string &name) {
    int index = contains(name);
    if (index != NOT_INDEX) {
        if (contains(name, map_standard_req) == NOT_INDEX)
            f_map.erase(f_map.begin() + index);
    }
}

void segy_header_map::clear() {
    f_map = map_standard_req;
    f_type = header_map_type::CUSTOM;
}

int segy_header_map::contains(const string &name) const {
    return contains(name, f_map);
}

seismic_header_map::header_field_info segy_header_map::get_field(const string &name) const {
    int index = contains(name);
    if (index != NOT_INDEX)
        return f_map[index].second;
    else
        throw invalid_argument(
            "segy_header_map: get_field: header map doesn't "
            "contains this field. Use segy_header_map:contains"
        );
}

pair<string, seismic_header_map::header_field_info>
segy_header_map::get_field(int index) const {
    if (index >= 0 && index < f_map.size())
        return f_map[index];
    else
        throw invalid_argument(
            "segy_header_map: get_field: header map doesn't "
            "contains field with index " + to_string(index)
        );
}

size_t segy_header_map::count() const {
    return f_map.size();
}

header_map_type segy_header_map::type() const {
    return f_type;
}

seismic_header_map::map_storage segy_header_map::to_map() const {
    return f_map;
}

void segy_header_map::from_map(const seismic_header_map::map_storage &m) {
    clear();
    for (int i = 0; i < m.size(); ++i) {
        int index = contains(m[i].first);
        if (index != NOT_INDEX)
            f_map[index].second = m[i].second;
        else
            f_map.push_back({ m[i] });
    }
}

int segy_header_map::contains(const string &name, const segy_header_map::map_storage &map) const {
    for (int i = 0; i < f_map.size(); ++i)
        if (map[i].first == name)
            return i;
    return NOT_INDEX;
}

#ifdef PYTHON
void py_segy_header_map_init(py::module &m,
    py::class_<segy_header_map, std::shared_ptr<segy_header_map>> &py_segy_header_map) {

    py_segy_header_map.def(py::init<>());
    py_segy_header_map.def(py::init<header_map_type>(),
        py::arg("map_type")
    );
}
#endif
