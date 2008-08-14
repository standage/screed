#! /usr/bin/env python

import os
import sys
from distutils.core import setup
from Pyrex.Distutils import build_ext, Extension
from distutils import sysconfig

sysconfig.get_config_vars("CFLAGS") # Set gcc's flags
sysconfig._config_vars["CFLAGS"] = "-fno-strict-aliasing -DNDBUG -g -Wall"

lib_path = os.path.join("..", "CMakeFiles", "DBREAD.dir", "src")
if os.path.exists(os.path.join(lib_path, "dbread.lib.o")):
  lib_path = os.path.join(lib_path, "dbread.lib.o")
elif os.path.exists(os.path.join(lib_path, "dbread.lib.cpp.obj")):
  lib_path = os.path.join(lib_path, "dbread.lib.cpp.obj")
setup(
	name = "_seqdb2",
	ext_modules=[
		Extension("_seqdb2", ["_seqdb2.pyx"], extra_objects=[lib_path], pyrex_cplus=True,)
],
	cmdclass = {'build_ext': build_ext}
)
