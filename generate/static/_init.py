
from importlib.abc import Loader, MetaPathFinder
from importlib.machinery import ModuleSpec
from pathlib import Path
import types
import sys
from gdpy.fileaccess import FileAccess


class GodotLoader(Loader):

    def __init__(self, file_path):
        self.file_path = file_path
    
    def create_module(self, spec):
        module = types.ModuleType(spec.name)
        if self.file_path is not None:
            module.__file__ = self.file_path
        return module
        
    def exec_module(self, module):
        if self.file_path is None:
            return module
        file = FileAccess.open(self.file_path, FileAccess.ModeFlags.READ)
        try:
            exec(file.get_as_text(), module.__dict__, module.__dict__)
        finally:
            file.close()
        return module


class GdPyMetaPathFinder:
    
    def find_spec(self, fullname, path, target=None):
        parts = fullname.split(".")
        if parts[0] != "gdres":
            return None
        file_path = None
        if len(parts) == 1:
            is_package = True
        else:
            path = Path(*parts[1:])
            if path.is_dir():
                is_package = True
                file_path = f"res://{path}"
            else:
                py_path = Path(str(path) + ".py")
                if py_path.is_file():
                    is_package = False
                    file_path = f"res://{py_path}"
                else:
                    return None
        return ModuleSpec(
            fullname,
            GodotLoader(file_path),
            is_package=is_package
        )
        

sys.meta_path.append(GdPyMetaPathFinder())
