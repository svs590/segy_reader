#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '../bin')

import segy_reader as sgr
import matplotlib.pyplot as plt

reader = sgr.segy_reader('D:/FullStack_PSTM.segy', 1, 1110, False, False, False)

print(reader.bin_header_str)
print(f'Sample interval: {reader.sample_interval}')
print(f'Samples count: {reader.samples_count}')
print(f'Traces count: {reader.traces_count}')

bin_header = reader.bin_header
print(bin_header.to_dict())

header_map = reader.header_map

#header_map.add("row", 5, 4, 1, "Test")
#header_map.add("col", 17, 4, 1, "Test")

header_map_dict = header_map.to_dict()
for info in header_map.to_dict():
    print(f'{info} \t {header_map_dict[info]}')

print('\nDetermining how segy file is sorted...')
reader.sort()
print('\nDone')

#Trace #300000
reader.move(300000, 1000)
trc = reader.get_trace()
header = trc.header
print(header.to_dict())
print(trc.data)


#iline #370 plot
il = reader.iline(370)
plt.imshow(il.T, cmap='gray_r', aspect='auto')
plt.show()

#xline #100 plot
xl = reader.xline(100)
plt.imshow(xl.T, cmap='gray_r', aspect='auto')
plt.show()

reader.close_file()