from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext

__version__ = "0.1.2"

ext_modules = [
    Pybind11Extension(
        'aminfo',
        ['src/aminfo.cpp'],
    ),
]

setup(
    name="aminfo",
    version=__version__,
    author="ddm-j",
    ext_modules=ext_modules,
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)