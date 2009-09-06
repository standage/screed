#! /usr/bin/env python
import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

setup(
	name = "screed",
	ext_modules=[
  Extension("screed",
            sources=["python/screed.pyx", "src/dbread_lib.cpp", "src/tools.cpp"],
            include_dirs=["src/"],
            pyrex_cplus=True,
            )
  ],
	cmdclass = {'build_ext': build_ext}
)
