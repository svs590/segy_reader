#ifdef SEGY_READER_TEST

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "segy_reader.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"
#include "segy_header_info.h"

using namespace std;
using namespace std::chrono;

int main() {

	string file = "D:/FullStack_PSTM.segy";

	auto reader = shared_ptr<seismic_data_provider>(
			new segy_reader(
			file,
			1,
			1110,
			false,
			false,
			false)
		);

	auto bh = reader->bin_header();
	bh->to_map();

	cout << reader->text_header() << endl;
	auto binHeader = dynamic_pointer_cast<segy_bin_header>(reader->bin_header());
	cout << "LINE_NUM: " << binHeader->reelNum() << endl;
	cout << "LINE_NUM: " << binHeader->correlatedTraces() << endl;

	// Âûגמה בטעמגמי ךאנעû ץ‎הונמג
	auto hdrMap = reader->header_map();
	int row_index = hdrMap->contains("row");
	int col_index = hdrMap->contains("col");

	cout << row_index << endl;
	cout << col_index << endl;

	//hdrMap->add_field("row", 8, 4, seismic_data_type::INT, "");
	//hdrMap->add_field("col", 20, 4, seismic_data_type::INT, "");
	int numHeaders = hdrMap->count();
	cout << "num heders in map: " << numHeaders << endl;
	cout << "traces count: " << reader->traces_count() << endl;

	for (int i = 0; i < numHeaders; ++i) {
		auto headerInfo = hdrMap->get_field(i);
		cout << headerInfo->name() << '\t'
			<< headerInfo->byte_loc() << '\t'
			<< headerInfo->byte_size() << '\t'
			<< (int)headerInfo->type_out() << endl;
	}

	int n = reader->samples_count();

	cout << "Preprocessing..." << endl;
	auto start = system_clock::now();
	reader->preprocessing();
	auto end = system_clock::now();
	cout << "Done" << endl;
	cout << "Preprocessing time: " << duration_cast<milliseconds>(end - start).count() << endl;

	auto segyreader = dynamic_pointer_cast<segy_reader>(reader);

	auto iline_headers = segyreader->get_headers(segyreader->get_geometry()->get_lines()[1200]);

	start = system_clock::now();
	for (int i = 0; i < 100; ++i) {
		cout << i << endl;
		auto iline = segyreader->get_traces(segyreader->get_geometry()->get_lines()[i]);
	}
	end = system_clock::now();
	cout << "100 Line time: " << duration_cast<milliseconds>(end - start).count() << endl;

	auto iline = segyreader->get_traces(segyreader->get_geometry()->get_lines()[0]);
	ofstream ilinefile("iline.dat");
	for (int i = 0; i < iline.size(); ++i) {
		int x = i;
		auto data = iline[i]->get_data();
		for (int j = 0; j < data.size(); ++j) {
			int y = n - j;
			ilinefile << x << '\t' << y << '\t' << data[j] << endl;
		}
	}
	ilinefile.close();
	
	auto t = reader->get_trace(100000);
	auto th = t->get_header();
	for (int i = 0; i < th->count(); ++i) {
		cout << th->name(i) << '\t';
		int ibuf;
		float fbuf;
		double dbuf;
		auto hdr_val = th->get(i);
		switch (hdr_val.second)
		{
		case seismic_data_type::INT:
			ibuf = any_cast<int>(hdr_val.first);
			cout << ibuf << endl;
			break;
		case seismic_data_type::FLOAT:
			fbuf = any_cast<float>(hdr_val.first);
			cout << fbuf << endl;
			break;
		case seismic_data_type::DOUBLE:
			dbuf = any_cast<double>(hdr_val.first);
			cout << dbuf << endl;
			break;
		default:
			break;
		}
	}

	ofstream fout("trace.dat");
	auto trace_data = t->get_data();
	for (int i = 0; i < trace_data.size(); ++i)
		fout << trace_data[i] << endl;
	fout.close();
	
	system("pause");
	return 0;
}


#endif
