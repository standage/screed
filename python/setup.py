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
hlib_path = os.path.join("..", "CMakeFiles", "HASH.dir", "src")
if os.path.exists(os.path.join(rlib_path, "dbread_lib.o")):
  rlib_path = os.path.join(rlib_path, "dbread_lib.o")
elif os.path.exists(os.path.join(rlib_path, "dbread_lib.obj")):
  rlib_path = os.path.join(rlib_path, "dbread_lib.obj")

if os.path.exists(os.path.join(hlib_path, "hashlib.o")):
  hlib_path = os.path.join(hlib_path, "hashlib.o")
elif os.path.exists(os.path.join(hlib_path, "hashlib.obj")):
  hlib_path = os.path.join(hlib_path, "hashlib.obj")
setup(
	name = "seqdb2",
        py_modules = ['seqdb2'],
	ext_modules=[
		Extension("seqdb2", ["seqdb2.pyx"], extra_objects=[rlib_path, hlib_path], pyrex_cplus=True, include_dirs=[c_header_path])
],
	cmdclass = {'build_ext': build_ext}
)
