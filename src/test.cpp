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

	string file = "D:/test/EE_1_2.sgy";

	segy_reader reader(
		file,
		100,
		1110,
		false,
		false,
		false
	);

	auto bh = reader.binHeader();
	bh.to_map();

	cout << reader.charBinHeader() << endl;
	segy_bin_header binHeader = reader.binHeader();
	cout << "LINE_NUM: " << binHeader.reelNum() << endl;
	cout << "LINE_NUM: " << binHeader.correlatedTraces() << endl;

	// Вывод битовой карты хэдеров
	segy_header_map hdrMap = reader.traceHeaderMap();
	int row_index = hdrMap.contains("row");
	int col_index = hdrMap.contains("col");

	cout << row_index << endl;
	cout << col_index << endl;

	//hdrMap.addHeader("row", 5, 4, 1, "");
	//hdrMap.addHeader("col", 17, 4, 1, "");
	int numHeaders = hdrMap.count();
	cout << "num heders in map: " << numHeaders << endl;

	for (int i = 0; i < numHeaders; ++i) {
		segy_header_info headerInfo = hdrMap.headerInfo(i);
		cout << headerInfo.getName() << '\t'
			<< headerInfo.getByteLoc() << '\t'
			<< headerInfo.getByteSize() << '\t'
			<< (int)headerInfo.getInType() << endl;
	}

	cout << reader.tracesCount() << endl;
	cout << reader.headersCount() << endl;

	int n = reader.samplesCount();

	auto start = system_clock::now();
	reader.moveToTrace(0, reader.tracesCount());
	for (int i = 0; i < reader.tracesCount(); ++i) {
		reader.getNextTrace();
	}
	auto end = system_clock::now();

	cout << "Пробег по всем трассам: " << duration_cast<seconds>(end - start).count() << endl;

	cout << "Determine sorting..." << endl;
	reader.determineSorting();
	cout << "Done" << endl;

	//system("pause");
	std::vector<float> iline;

	start = system_clock::now();
	for (int i = 0; i < 100; ++i) {
		//cout << i << endl;
		iline = reader.iline(312 + i);
	}
	end = system_clock::now();
	cout << "100 Line time: " << duration_cast<milliseconds>(end - start).count() << endl;

	ofstream ilinefile("iline.dat");
	for (int i = 0; i < iline.size(); ++i) {
		int x = i / n;
		int y = n - (i % n);
		ilinefile << x << '\t' << y << '\t' << iline[i] << endl;
	}
	ilinefile.close();

	auto t = reader.getTrace(591908);

	/*ofstream fout("trace.dat");
	for (int i = 0; i < t.size(); ++i)
		fout << t[i] << endl;
	fout.close();*/

	system("pause");
	return 0;
}


#endif
