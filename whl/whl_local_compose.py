"""
Создаем whl для библиотеки seismo_reader
"""
import os
from setuptools import setup, find_packages, findall
from setuptools.dist import Distribution
import datetime
import glob
import shutil


class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(foo):
        return True


binaries_required = {'posix' : ['seismo_reader.so'], 'nt' : ['seismo_reader.pyd']}

vf = open('version.txt', 'r')

__version__ = vf.read()
__lib_name__ = 'seismo_reader'  # название директории
__dist_dir__ = os.path.dirname(os.path.abspath(__file__))  # директория, в которую сгенерируется whl архив
__mask__ = 'seismo_reader*.whl'  # маска файла, по которой будут отлавливаться старые либы

if os.name == 'posix':
    __bin_dir__ = '../bin/'
elif os.name == 'nt':
    __bin_dir__ = '../bin/'
else:
    print('Current OS is not supported')
    exit(1)

binaries = binaries_required[os.name]


# папка, в которой находится setup.py
def get_cur_dir():
    return os.getcwd()


# чтобы устанавливалось, надо находится в cur_dir
def change_dir():
    cur_dir = os.path.realpath(os.path.dirname(__file__))
    print("==> Changing working directory to '%s'..." % cur_dir)
    os.chdir(cur_dir)
    assert os.getcwd() == cur_dir


# проверяем
def check_files():
    for binfile in binaries:
        assert os.path.exists(os.path.join(get_cur_dir(), __bin_dir__, binfile))


def copy_files(bin_data):
    for bin_file in bin_data:
        shutil.copy(bin_file, __lib_name__ + '/')

# относительные пути всех файлов в папке
def collect_files(target_dir):
    # ищем все файлы в папке
    parent_dir = os.path.dirname(target_dir)
    files = findall(target_dir)
    # делаем пути относительными
    return [os.path.relpath(r, parent_dir) for r in files]


# определяем пути к dll-файлам
def collect_bin_files(dir):
    # копируем все бинарники для всех архитектур (фильтруются отдельной командой)
    bin_folder = os.path.join(get_cur_dir(), dir)
    all_files = collect_files(bin_folder)
    if (dir == ''):
        bin_folder = ''
    bin_files = []
    for file in all_files:
        if file.endswith('.dll'):
            bin_files.append(os.path.join(bin_folder, os.path.basename(file)))
        if file.endswith('.pyd'):
            bin_files.append(os.path.join(bin_folder, os.path.basename(file)))
        if file.endswith('.so'):
            bin_files.append(os.path.join(bin_folder, os.path.basename(file)))
    print("==> Collecting %d binary files..." % len(bin_files))
    for bin_file in bin_files:
        print("   ==> '%s'" % bin_file)
    return bin_files


# определяем список устанавливаемых python-пакетов
def collect_packages():
    # получаем рекурсивно все пакеты,
    packages = find_packages(
        include=[__lib_name__ + "*"]  # __lib_name__
    )
    print("==> Collecting %d packages..." % len(packages))
    for package in packages:
        print("   ==> '%s'" % package)
    return packages


change_dir()
# считываем актуальную версию
# read_actual_version()
# проверяем, что все необходимые файлы сгенерированы
check_files()
copy_files(collect_bin_files(__bin_dir__))

old_file_whl = glob.glob(__mask__)
if len(old_file_whl) > 0:
    os.remove(os.path.join(get_cur_dir(), old_file_whl[0]))  # удаляем старый файл, если он есть

# производим установку
setup(
    # название и версия
    name=__lib_name__,
    version=__version__,
    #plat_name='nt',
    # список пакетов, которые должны быть установлены
    packages=collect_packages(),
    # файлы ресурсов и бинарные файлы
    package_data={__lib_name__: collect_bin_files('')},

    # хранить в site-packages в виде архива нельзя, т.к. _c_wrap обращается к __file__
    zip_safe=False,
    script_args=['bdist_wheel',
                 '--dist-dir='+__dist_dir__,
                 ],
    distclass=BinaryDistribution
)

# удаление лишней директории, которая мешает обновлению версий при обновлении интерпритатора
shutil.rmtree(os.path.join(get_cur_dir(), __lib_name__ + '.egg-info'))
# удаление лишней директории, возникающей при сборке библиотеки
shutil.rmtree(os.path.join(get_cur_dir(), 'build'))
