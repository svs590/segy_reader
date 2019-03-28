#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '../bin')

import segy_reader as sgr
import matplotlib.pyplot as plt
import numpy as np

reader = sgr.segy_reader('D:/FullStack_PSTM.segy', sgr.header_map_type.STANDARD)

print(reader.text_header())
print(f'Sample interval: {reader.sample_interval()}')
print(f'Samples count: {reader.samples_count()}')
print(f'Traces count: {reader.traces_count()}')

bin_header = reader.bin_header()
print(bin_header.to_dict())

new_header_map = sgr.segy_header_map(sgr.header_map_type.STANDARD)
new_header_map.clear()

''' Set custom iline and xline header fields '''
#new_header_map.set_field("row", 5, 4, sgr.seismic_data_type.INT)
#new_header_map.set_field("col", 17, 4, sgr.seismic_data_type.INT)

reader.set_header_map(new_header_map)

header_map_dict = reader.header_map().to_dict()
for info in header_map_dict:
    print(f'{info} \t {header_map_dict[info]}')

print('\nPreprocessing...')
reader.preprocessing()
print('Preprocessing done')

geom = reader.get_geometry()
lines = geom.get_lines()

for i in range(0, len(lines), 50):
    plt.plot([lines[i].start[0], lines[i].end[0]], [lines[i].start[1], lines[i].end[1]])

plt.show()

''' Trace 300000 '''
trc = reader.get_trace(300000)
header = trc.get_header()
print(f'Trace header: {header.to_dict()}')
print(f'Trace data: {trc.get_data()}')

''' line 370 plot '''
l370 = reader.get_traces(lines[370])

''' It may be optimal and faster! '''
trc_array = l370[0].get_data()
for i in range(1, len(l370)):
    trc_array = np.column_stack((trc_array, l370[i].get_data()))

plt.imshow(trc_array, cmap='gray_r', aspect='auto')
plt.show()

reader.close()