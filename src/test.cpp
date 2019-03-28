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
		new segy_reader(file, header_map_type::STANDARD)
		);

	cout << reader->text_header() << endl;
	
	auto new_map = shared_ptr<segy_header_map>(
		new segy_header_map(header_map_type::STANDARD)
		);
	new_map->clear();
	reader->set_header_map(new_map);

	auto trace0 = reader->get_trace(0);
	auto header0 = trace0->get_header();

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
