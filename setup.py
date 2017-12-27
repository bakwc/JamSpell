import os
from setuptools import setup
from setuptools.extension import Extension

this_dir = os.path.dirname(os.path.abspath(__file__))

openspell = Extension(
    name='_openspell',
    include_dirs=[this_dir],
    sources=[
        os.path.join(this_dir, 'openspell', 'lang_model.cpp'),
        os.path.join(this_dir, 'openspell', 'spell_corrector.cpp'),
        os.path.join(this_dir, 'openspell', 'utils.cpp'),
        os.path.join(this_dir, 'openspell.i'),
    ],
    extra_compile_args=['-std=c++11'],
    swig_opts=['-c++'],
)

VERSION = '0.0.1'

setup(
    name='openspell',
    version='0.0.1',
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
    ext_modules=[openspell],
    py_modules=['openspell'],
    zip_safe=False,
)
