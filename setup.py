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
        os.path.join(this_dir, 'swig', 'openspell.i')
    ],
    extra_compile_args=['-std=c++11'],
    swig_opts=['-c++'],
)

setup(
    name='openspell',
    version='0.0.1',
    author='Filipp Ozinov',
    description='spell checker',
    url='http://example.com',
    classifiers=[
        # The list of PyPI classifiers
    ],
    ext_modules=[openspell],
    zip_safe=False,
    include_package_data=True,
)