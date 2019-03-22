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

	// Вывод битовой карты хэдеров
	auto hdrMap = reader->header_map();
	int row_index = hdrMap->contains("row");
	int col_index = hdrMap->contains("col");

	cout << row_index << endl;
	cout << col_index << endl;

	hdrMap->add_field("row", 8, 4, seismic_data_type::INT, "");
	hdrMap->add_field("col", 20, 4, seismic_data_type::INT, "");
	int numHeaders = hdrMap->count();
	cout << "num heders in map: " << numHeaders << endl;

	//for (int i = 0; i < numHeaders; ++i) {
	//	segy_traceheader_field headerInfo = hdrMap.headerInfo(i);
	//	cout << headerInfo.getName() << '\t'
	//		<< headerInfo.getByteLoc() << '\t'
	//		<< headerInfo.getByteSize() << '\t'
	//		<< (int)headerInfo.getInType() << endl;
	//}

	//cout << reader.tracesCount() << endl;
	//cout << reader.headersCount() << endl;

	int n = reader->samples_count();

	auto start = system_clock::now();
	////reader.moveToTrace(0, reader.tracesCount());
	////for (int i = 0; i < reader.tracesCount(); ++i) {
	////	reader.getNextTrace();
	////}
	auto end = system_clock::now();

	////cout << "Пробег по всем трассам: " << duration_cast<seconds>(end - start).count() << endl;

	start = system_clock::now();
	cout << "Determine sorting..." << endl;
	reader->preprocessing();
	cout << "Done" << endl;
	end = system_clock::now();
	cout << "Sorting time: " << duration_cast<milliseconds>(end - start).count() << endl;

	//system("pause");
	std::vector<segy_trace> iline;
	auto segyreader = dynamic_pointer_cast<segy_reader>(reader);

	start = system_clock::now();
	for (int i = 0; i < 100; ++i) {
		cout << i << endl;
		iline = segyreader->iline(640 + i);
	}
	//iline = reader.iline(16);
	end = system_clock::now();
	cout << "100 Line time: " << duration_cast<milliseconds>(end - start).count() << endl;

	ofstream ilinefile("iline.dat");
	for (int i = 0; i < iline.size(); ++i) {
		int x = i;
		for (int j = 0; j < iline[i].get_data().size(); ++j) {
			int y = n - (i % n);
			ilinefile << x << '\t' << y << '\t' << iline[i].get_data()[j] << endl;
		}
	}
	ilinefile.close();

	auto t = reader->get_trace(591908);
	auto th = t->get_header();
	for (int i = 0; i < th->count(); ++i)
		cout << th->name(i) << '\t' << th->get<float>(i) << endl;

	cout << endl << t->get_data().size() << endl;

	/*ofstream fout("trace.dat");
	for (int i = 0; i < t.size(); ++i)
		fout << t[i] << endl;
	fout.close();*/

	system("pause");
	return 0;
}


#endif
