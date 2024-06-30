#!/usr/bin/env python
import os
import sys
from pathlib import Path

name = "gdpy"
build_dir = "build"

env = SConscript("vendor/godot-cpp/SConstruct")
env.Append(CPPPATH=["src/"])
    
if env["platform"] == "macos":
    library_name = f"lib{name}.{env['platform']}.{env['target']}"
    library_name = f"{build_dir}/{library_name}.framework/{library_name}"
else:
    library_name = f"{build_dir}/lib{name}{env['suffix']}{env['SHLIBSUFFIX']}"

library = env.SharedLibrary(library_name, source=Glob("src/*.cpp"))
Default(library)
