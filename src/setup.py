#! /usr/bin/env python

import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

sysconfig.get_config_vars("CFLAGS") # Set gcc's flags
sysconfig._config_vars["CFLAGS"] = "-fno-strict-aliasing -DNDBUG -g -Wall"

setup(
	name = "_seqdb2",
	ext_modules=[
		Extension("_seqdb2", ["_seqdb2.pyx"], extra_objects=["../CMakeFiles/DBREAD.dir/src/dbread.lib.o"], pyrex_cplus=True,)
		],
	cmdclass = {'build_ext': build_ext}
)
