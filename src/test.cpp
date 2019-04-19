#ifdef SEGY_READER_TEST

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "segy_reader.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"

using namespace std;
using namespace std::chrono;

int main() {

	wstring file = L"F:/FullStack_PSTM.segy";

    segy_reader_config config;
    config.filename = L"D:/FullStack_PSTM.segy";

	auto reader = shared_ptr<seismic_data_provider>(
		new segy_reader(config)
		);
    
    cout << "Traces count: " << reader->traces_count() << endl;

	cout << reader->text_header() << endl;

	auto bh = reader->bin_header();
    auto bh_map = bh->to_map();

    for (auto m_pair : bh_map) {
        cout << m_pair.first << '\t';
        auto variant = m_pair.second;
        
        if (holds_alternative<int>(variant))
            cout << get<int>(variant) << endl;
        else if (holds_alternative<short>(variant))
            cout << get<short>(variant) << endl;
        else if (holds_alternative<unsigned short>(variant))
            cout << get<unsigned short>(variant) << endl;
        else if (holds_alternative<float>(variant))
            cout << get<float>(variant) << endl;
        else if (holds_alternative<double>(variant))
            cout << get<double>(variant) << endl;
    }
    cout << endl;
	
	auto new_map = shared_ptr<segy_header_map>(
		    new segy_header_map(header_map_type::STANDARD)
		);
    auto map_dict = new_map->to_map();
    for (auto field : map_dict) {
        cout << field.first << '\t';
        cout << get<0>(field.second) << '\t';
        cout << get<1>(field.second) << '\t';
        cout << (int)get<2>(field.second) << endl;
    }
    cout << endl;

	new_map->clear();
    new_map->set_field("Inline", 4, 4, seismic_data_type::INT, "");
    new_map->set_field("Crossline", 20, 4, seismic_data_type::INT, "");
    new_map->set_field("CDP X", 72, 4, seismic_data_type::INT, "");
    new_map->set_field("CDP Y", 76, 4, seismic_data_type::INT, "");
    map_dict = new_map->to_map();
    for (auto field : map_dict) {
        cout << field.first << '\t';
        cout << get<0>(field.second) << '\t';
        cout << get<1>(field.second) << '\t';
        cout << (int)get<2>(field.second) << endl;
    }

	reader->set_header_map(new_map);

    //auto trc_header = dynamic_pointer_cast<segy_trace_header>(reader->header(0));
    //cout << get<int>(trc_header->CDP_X()) << '\t' << get<int>(trc_header->crossline()) << endl << get<short>(trc_header->samples_count()) << endl;

    auto trace = reader->trace(300);
    trace = reader->trace(300);
    trace = reader->trace(300);
    trace = reader->trace(300);
    auto trc_data = get<Eigen::Matrix<float, -1, 1>>(trace->data());
    //cout << trc_data << endl;
    cout << trc_data.rows() << ' ' << trc_data.minCoeff() << ' ' << trc_data.maxCoeff() << endl;

    ofstream trcfile("trace.dat");
    for (int i = 0; i < trc_data.rows(); ++i) {
        trcfile << i << '\t' << trc_data(i) << endl;
    }
    trcfile.close();

    /*
	auto trace0 = reader->trace(0);
	auto header0 = trace0->header();

	

    */
	cout << "Preprocessing..." << endl;
	auto f_start_point = system_clock::now();
	reader->preprocessing();
	auto f_end_point = system_clock::now();
	cout << "Done" << endl;
	cout << "Preprocessing time: " << duration_cast<milliseconds>(f_end_point - f_start_point).count() << endl;

    /*
	auto segyreader = dynamic_pointer_cast<segy_reader>(reader);

	auto iline_headers = segyreader->headers(segyreader->geometry()->lines()[1200]);

	start = system_clock::now();
	for (int i = 0; i < 100; ++i) {
		cout << i << endl;
		auto iline = segyreader->traces(segyreader->geometry()->lines()[i]);
	}
	end = system_clock::now();
	cout << "100 Line time: " << duration_cast<milliseconds>(end - start).count() << endl;
    */

    int n = reader->samples_count();
	auto iline = reader->traces(reader->geometry()->lines()[0]);
	//ofstream ilinefile("iline.dat");
	for (int i = 0; i < iline.size(); ++i) {
		int x = i;
		auto &data = get<Eigen::Matrix<float, -1, 1>>(iline[i]->data());
		//for (int j = 0; j < data.rows(); ++j) {
		//	int y = n - j;
		//	ilinefile << x << '\t' << y << '\t' << data(j) << endl;
		//}
	}
	//ilinefile.close();
	
    /*
	auto t = reader->trace(100000);
	auto th = t->header();
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
	auto trace_data = t->data();
	for (int i = 0; i < trace_data.loaded_trc_count(); ++i)
		fout << trace_data[i] << endl;
	fout.close();
	*/
	system("pause");
	return 0;
}


#endif
