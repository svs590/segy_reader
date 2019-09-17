import os
if os.name == 'posix':
    import os
    import ctypes
    init_folder = os.path.dirname(__file__)
    ctypes.cdll.LoadLibrary(init_folder + '/libboost_filesystem.so.1.71.0')

from .seismo_reader import *

