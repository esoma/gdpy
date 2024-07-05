
__all__ = ["Property", "get_properties", "get_module_script", "Export", "script", ]

from weakref import WeakValueDictionary, WeakKeyDictionary
from typing import Any
from inspect import get_annotations
from typing import NamedTuple
from typing import Mapping


class Property(NamedTuple):
    has_default: bool
    default: Any


_scripts: WeakValueDictionary[str, Any] = WeakValueDictionary()
_properties: WeakKeyDictionary[Any, Mapping[str, Property]] = WeakKeyDictionary()


def script(obj):
    _scripts[obj.__module__] = obj
    
    properties: dict[str, Property] = {}
    _properties[obj] = properties
    for k, v in get_annotations(obj, eval_str=True).items():
        try:
            default = getattr(obj, k)
            has_default = True
        except AttributeError:
            default = None
            has_default = False
        properties[k] = Property(has_default, default)
    
    return obj

    
def get_module_script(module_name):
    return _scripts[module_name]
    
def get_properties(obj: Any) -> Mapping[str, Property]:
    return _properties[obj]
    
class Export:
    pass