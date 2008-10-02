#! /usr/bin/env python

import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

sysconfig.get_config_vars("CFLAGS") # Set gcc's flags
sysconfig._config_vars["CFLAGS"] = "-fno-strict-aliasing -DNDBUG -g -Wall"

c_header_path = os.path.join("..", "src")
lib_path = os.path.join("..", "CMakeFiles", "DBREAD.dir", "src")
if os.path.exists(os.path.join(lib_path, "dbread_lib.o")):
  lib_path = os.path.join(lib_path, "dbread_lib.o")
elif os.path.exists(os.path.join(lib_path, "dbread_lib.cpp.obj")):
  lib_path = os.path.join(lib_path, "dbread_lib.cpp.obj")
setup(
	name = "seqdb2",
        py_modules = ['seqdb2'],
	ext_modules=[
		Extension("seqdb2", ["seqdb2.pyx"], extra_objects=[lib_path], pyrex_cplus=True, include_dirs=[c_header_path])
],
	cmdclass = {'build_ext': build_ext}
)
