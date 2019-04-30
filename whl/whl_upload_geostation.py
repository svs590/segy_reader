"""
Скрипт загружает файл py_stochastic_simulations*.whl из папки с этим скриптом на сервер
"""
import os
import glob
import shutil

__libname__ = 'seismo_reader'

__server_dir__ = "\\\\10.205.103.50\pub\\rex\\required\pypi_repo\\" + __libname__    # папка на сервере
__script_dir__ = os.path.dirname(os.path.abspath(__file__))     # папка этого скрипта
__mask__ = __libname__ + '*.whl'  # маска файла

print(__server_dir__)

os.chdir(__script_dir__)  # переходим в папку скрипта
new_file_whl = glob.glob(__mask__)  # ищем нужный новый файл

assert len(new_file_whl) > 0  # проверяем нашелся ли файл

os.chdir(__server_dir__)  # переходим в папку сервера
assert os.getcwd() == __server_dir__  # проверяем успешно ли перешли в папку сервера

shutil.copy2(os.path.join(__script_dir__, new_file_whl[0]), __server_dir__)  # копируем новый файл на сервер
