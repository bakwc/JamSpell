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
        os.path.join('openspell.i'),
    ],
    extra_compile_args=['-std=c++11'],
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

class Build_Ext_find_swig3(build_ext):
    def find_swig(self):
        return get_swig_executable()

def get_swig_executable():
    # stolen from https://github.com/FEniCS/ffc/blob/master/setup.py
    "Get SWIG executable"

    # Find SWIG executable
    swig_executable = None
    swig_minimum_version = "3.0.2"
    for executable in ["swig", "swig3.0"]:
        swig_executable = find_executable(executable)
        if swig_executable is not None:
            # Check that SWIG version is ok
            output = subprocess.check_output([swig_executable, "-version"]).decode('utf-8')
            swig_version = re.findall(r"SWIG Version ([0-9.]+)", output)[0]
            if LooseVersion(swig_version) >= LooseVersion(swig_minimum_version):
                break
            swig_executable = None
    if swig_executable is None:
        raise OSError("Unable to find SWIG version %s or higher." % swig_minimum_version)
    print("Found SWIG: %s (version %s)" % (swig_executable, swig_version))
    return swig_executable

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
        'build_ext': Build_Ext_find_swig3,
    },
    include_package_data=True,
)
