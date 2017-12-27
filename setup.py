import os
from distutils.command.build import build
from setuptools.command.install import install
from setuptools import setup
from setuptools.extension import Extension

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

VERSION = '0.0.4'

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
    cmdclass={'build': CustomBuild, 'install': CustomInstall},
    include_package_data=True,
)
