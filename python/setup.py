#! /usr/bin/env python

import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

sysconfig.get_config_vars("CFLAGS") # Set gcc's flags
sysconfig._config_vars["CFLAGS"] = "-fno-strict-aliasing -DNDBUG -g -Wall"

c_header_path = os.path.join("..", "src")
rlib_path = os.path.join("..", "CMakeFiles", "DBREAD.dir", "src")
tlib_path = os.path.join("..", "CMakeFiles", "TOOLS.dir", "src")
if os.path.exists(os.path.join(rlib_path, "dbread_lib.o")):
  rlib_path = os.path.join(rlib_path, "dbread_lib.o")
elif os.path.exists(os.path.join(rlib_path, "dbread_lib.obj")):
  rlib_path = os.path.join(rlib_path, "dbread_lib.obj")

if os.path.exists(os.path.join(tlib_path, "tools.o")):
  tlib_path = os.path.join(tlib_path, "tools.o")
elif os.path.exists(os.path.join(tlib_path, "tools.obj")):
  tlib_path = os.path.join(tlib_path, "tools.obj")
setup(
	name = "screed",
        py_modules = ['screed'],
	ext_modules=[
		Extension("screed", ["screed.pyx"], extra_objects=[rlib_path, tlib_path], pyrex_cplus=True, include_dirs=[c_header_path])
],
	cmdclass = {'build_ext': build_ext}
)
