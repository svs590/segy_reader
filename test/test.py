#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '../bin')

import seismo_reader as sgr
import matplotlib.pyplot as plt
import numpy as np
from timeit import default_timer as timer

config = sgr.reader_config()
config.filename = 'D:/FullStack_PSTM.segy'
reader = sgr.segy_reader(config)

print(reader.text_header())
print(f'Sample interval: {reader.sample_interval()}')
print(f'Samples count: {reader.samples_count()}')
print(f'Traces count: {reader.traces_count()}')

bin_header = reader.bin_header()
print(bin_header.to_dict())

''' Create and clear header map. Clean map contains only a few required fields '''
new_header_map = sgr.segy_header_map(sgr.header_map_type.STANDARD)
#new_header_map.clear()

''' Set custom header fields '''
''' Note: Inline offset value 100 is incorrect, this will be fixed later '''
new_header_map.set_field("Inline", 100, 4, sgr.seismic_data_type.INT, '')
new_header_map.set_field("Crossline", 20, 4, sgr.seismic_data_type.INT, '')
#new_header_map.set_field("CDP X", 40, 4, sgr.seismic_data_type.INT, '')
#new_header_map.set_field("CDP Y", 44, 4, sgr.seismic_data_type.INT, '')

''' Segy reader can reset header map '''
reader.set_header_map(new_header_map)

''' Also reader returns reference to current header map and you can '''
''' change map without using <set_header_map> method '''
header_map_dict = reader.header_map().to_dict()
header_map_dict['Inline'] = (4, 4, sgr.seismic_data_type.INT, '')
reader.header_map().from_dict(header_map_dict)

for info in reader.header_map().to_dict():
    print(f'{info} \t {header_map_dict[info]}')

''' Trace 300000 '''
trc = reader.trace(300000)
header = trc.header()
#print(f'Trace CDP X: {header.CDP_X()}')
print(f'Trace header: {header.to_dict()}')
print(f'Trace data: {trc.data()}')

''' Exception handling. Try to get nonexistent trace '''
index = 10000000
try:
    trc = reader.trace(index)
except:
    print(f'Can\'t load trace {index}')

''' Plot iline and crossline number first 5000 traces '''
fig, (ax1, ax2) = plt.subplots(2, 1)
fig.subplots_adjust(hspace=0.5)
ax1.set_xlabel('trace')
ax1.set_ylabel('Iline number')
ax2.set_xlabel('trace')
ax2.set_ylabel('Crossline number')

trc_headers = reader.headers(0, 5000)
lsp = np.linspace(0, len(trc_headers), len(trc_headers))
iline_num = np.ndarray(shape=(len(trc_headers),))
xline_num = np.ndarray(shape=(len(trc_headers),))
for i in range(0, len(trc_headers)):
    ''' It may be optimal and faster! '''
    header_dict = trc_headers[i].to_dict()
    iline_num[i] = np.int(header_dict['Inline'])
    xline_num[i] = np.int(header_dict['Crossline'])

ax1.plot(lsp, iline_num)
ax2.plot(lsp, xline_num)
plt.show()

print('\nPreprocessing...')
reader.preprocessing()
print('Preprocessing done')

geom = reader.geometry()
lines = geom.lines()

for i in range(0, len(lines), 100):
    plt.plot(
        [
            lines[i].start_point[0],
            lines[i].end_point[0]
        ],
        [
            lines[i].start_point[1],
            lines[i].end_point[1]
        ]
    )

plt.show()

''' line 370 plot '''
start = timer()
l370 = reader.traces(lines[1370])
end = timer()
print(f'Get line time: {end-start} s')

''' It may be optimal and faster! '''
trc_array = np.zeros((len(l370), reader.samples_count()))

start = timer()
for i in range(0, len(l370)):
    trc_array[i, :] = l370[i].data()
end = timer()
print(f'Build matrix time: {end-start} s')

plt.imshow(trc_array.T, cmap='gray_r', aspect='auto')
plt.show()

reader.close()