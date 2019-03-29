#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '../bin')

import segy_reader as sgr
import matplotlib.pyplot as plt
import numpy as np
from timeit import default_timer as timer

reader = sgr.segy_reader('D:/FullStack_PSTM.segy', sgr.header_map_type.STANDARD)

print(reader.text_header())
print(f'Sample interval: {reader.sample_interval()}')
print(f'Samples count: {reader.samples_count()}')
print(f'Traces count: {reader.traces_count()}')

bin_header = reader.bin_header()
print(bin_header.to_dict())

new_header_map = sgr.segy_header_map(sgr.header_map_type.STANDARD)
new_header_map.clear()

''' Set custom header fields '''
#new_header_map.set_field("row", 8, 4, sgr.seismic_data_type.INT)
#new_header_map.set_field("col", 20, 4, sgr.seismic_data_type.INT)
#new_header_map.set_field("bin_x", 40, 4, sgr.seismic_data_type.INT)
#new_header_map.set_field("bin_y", 44, 4, sgr.seismic_data_type.INT)
#new_header_map.set_field("type", 28, 2, sgr.seismic_data_type.INT)

reader.set_header_map(new_header_map)

header_map_dict = reader.header_map().to_dict()
for info in header_map_dict:
    print(f'{info} \t {header_map_dict[info]}')

''' Trace 300000 '''
trc = reader.get_trace(300000)
header = trc.get_header()
print(f'Trace header: {header.to_dict()}')
print(f'Trace data: {trc.get_data()}')

''' Plot iline and crossline number first 5000 traces '''
fig, (ax1, ax2) = plt.subplots(2, 1)
fig.subplots_adjust(hspace=0.5)
ax1.set_xlabel('trace')
ax1.set_ylabel('Iline number')
ax2.set_xlabel('trace')
ax2.set_ylabel('Crossline number')

trc_headers = reader.get_headers(0, 5000)
lsp = np.linspace(0, len(trc_headers), len(trc_headers))
iline_num = np.ndarray(shape=(len(trc_headers),))
xline_num = np.ndarray(shape=(len(trc_headers),))
for i in range(0, len(trc_headers)):
    ''' It may be optimal and faster! '''
    header_dict = trc_headers[i].to_dict()
    iline_num[i] = np.int(header_dict['row'])
    xline_num[i] = np.int(header_dict['col'])

ax1.plot(lsp, iline_num)
ax2.plot(lsp, xline_num)
plt.show()

print('\nPreprocessing...')
reader.preprocessing()
print('Preprocessing done')

geom = reader.get_geometry()
lines = geom.get_lines()

for i in range(0, len(lines), 100):
    plt.plot([lines[i].start[0], lines[i].end[0]], [lines[i].start[1], lines[i].end[1]])

plt.show()

''' line 370 plot '''
start = timer()
l370 = reader.get_traces(lines[1370])
end = timer()
print(f'Get line time: {end-start} s')

''' It may be optimal and faster! '''
trc_array = np.zeros((len(l370), reader.samples_count()))

start = timer()
for i in range(0, len(l370)):
    trc_array[i, :] = l370[i].get_data()
end = timer()
print(f'Build matrix time: {end-start} s')

plt.imshow(trc_array.T, cmap='gray_r', aspect='auto')
plt.show()

reader.close()