import os
import re
from distutils.command.build import build
from distutils.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.spawn import find_executable
from distutils.version import LooseVersion
from setuptools import setup
from setuptools.extension import Extension
import subprocess

this_dir = os.path.dirname(os.path.abspath(__file__))

openspell = Extension(
    name='_openspell',
    include_dirs=['.', 'openspell'],
    sources=[
        os.path.join('openspell', 'lang_model.cpp'),
        os.path.join('openspell', 'spell_corrector.cpp'),
        os.path.join('openspell', 'utils.cpp'),
        os.path.join('openspell', 'perfect_hash.cpp'),
        os.path.join('openspell', 'bloom_filter.cpp'),
        os.path.join('contrib', 'cityhash', 'city.cc'),
        os.path.join('contrib', 'phf', 'phf.cc'),
        os.path.join('openspell.i'),
    ],
    extra_compile_args=['-std=c++11', '-O2'],
    swig_opts=['-c++'],
)

class CustomBuild(build):
    def run(self):
        self.run_command('build_ext')
        build.run(self)


class CustomInstall(install):
    def run(self):
        self.run_command('build_ext')
        self.do_egg_install()

class Swig3Ext(build_ext):
    def find_swig(self):
        swigBinary = find_executable('swig3.0') or find_executable('swig')
        assert swigBinary is not None
        assert subprocess.check_output([swigBinary, "-version"]).find('SWIG Version 3') != -1
        return swigBinary

VERSION = '0.0.5'

setup(
    name='openspell',
    version=VERSION,
    author='Filipp Ozinov',
    author_email='fippo@mail.ru',
    url='https://github.com/bakwc/OpenSpell',
    download_url='https://github.com/bakwc/OpenSpell/tarball/' + VERSION,
    description='spell checker',
    long_description='context-based spell checker',
    keywords=['nlp', 'spell', 'spell-checker'],
    classifiers=[
        'Programming Language :: Python :: 2.7',
        'License :: OSI Approved :: MIT License',
    ],
    py_modules=['openspell'],
    ext_modules=[openspell],
    zip_safe=False,
    cmdclass={
        'build': CustomBuild,
        'install': CustomInstall,
        'build_ext': Swig3Ext,
    },
    include_package_data=True,
)
