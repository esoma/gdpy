
__all__ = ["import_script"]

import importlib

def import_script(name, reload):
    module_name = _name_to_module_name(name)
    module = __import__(module_name, fromlist=[""])
    if reload:
        importlib.reload(module)
    return module
    
def _name_to_module_name(name):
    if name.startswith("res://"):
        name = "gdres/" + name.removeprefix("res://")
    name = name.removesuffix(".py")
    return name.replace("/", ".")