
__all__ = ["Property", "get_properties", "get_module_script", "Export", "script", ]

from weakref import WeakValueDictionary, WeakKeyDictionary
from typing import Any
from inspect import get_annotations
from typing import NamedTuple
from typing import Mapping
from typing import Annotated
from gdpy.propertyhint import PropertyHint
from gdpy.propertyusageflags import PropertyUsageFlags
from gdpy._varianttype import VariantType
from gdpy._variant import narrow_variant_to
from _gdpy import VariantWrapper
import importlib


class Property(NamedTuple):
    type: Any
    variant_type: VariantType
    class_name: str
    has_default: bool
    default: Any
    hint: PropertyHint
    hint_string: str
    usage: PropertyUsageFlags


_scripts: WeakValueDictionary[str, Any] = WeakValueDictionary()
_properties: WeakKeyDictionary[Any, Mapping[str, Property]] = WeakKeyDictionary()


def script(obj):
    _scripts[obj.__module__] = obj
    
    properties: dict[str, Property] = {}
    _properties[obj] = properties
    
    for cls in reversed(obj.__mro__):
        try:
            gdpy_class_name = cls.__gdpy_class_name__
        except AttributeError:
            continue
        if gdpy_class_name != obj.__gdpy_class_name__:
            continue
            
        for k, v in get_annotations(cls, eval_str=True).items():
            try:
                default = getattr(obj, k)
                has_default = True
            except AttributeError:
                default = None
                has_default = False
            properties[k] = _create_property(cls, k, v, has_default, default)
            
    for k in dir(obj):
        v = getattr(obj, k)
        if not isinstance(v, property):
            continue
        try:
            return_type = v.fget.__annotations__["return"]
        except KeyError:
            continue
        properties[k] = _create_property(obj, k, return_type, False, None)
    
    return obj
    
    
def _create_property(cls, name, annotation, has_default, default) -> Property:
    export = _no_export
    type = annotation
    if hasattr(annotation, "__metadata__"):
        type = annotation.__args__[0]
        for meta in annotation.__metadata__:
            if meta is Export or isinstance(meta, Export):
                export = meta
    return Property(
        type,
        _type_to_variant_type(type),
        cls.__name__,
        export.has_default,
        export.default,
        export.hint,
        export.hint_string,
        export.usage,
    )
    
    
def get_module_script(module):
    return _scripts[module.__name__]
    
    
def set_property_value(obj: Any, name: str, variant_value: VariantWrapper) -> bool:
    try:
        property = _properties[obj.__class__][name]
    except KeyError:
        return False
    value = narrow_variant_to(variant_value, property.type)
    setattr(obj, name, value)
    return True
    
def get_property_default_value(
    module_name: str,
    property_name: str
) -> VariantWrapper | None:
    script = _scripts[module_name]
    try:
        property = _properties[script][property_name]
    except KeyError:
        return None
    if not property.has_default:
        return None
    return VariantWrapper.create_from_type(
        property.default,
        property.variant_type,
    )
    
def get_property_value(obj: Any, name: str) -> VariantWrapper | None:
    try:
        value = getattr(obj, name)
    except AttributeError:
        return None
    try:
        property = _properties[obj.__class__][name]
    except KeyError:
        return None
    return VariantWrapper.create_from_type(
        value,
        property.variant_type,
    )
    
def get_property_can_revert(obj: Any, name: str) -> bool:
    try:
        v = getattr(obj, name)
    except AttributeError:
        return False
    try:
        property = _properties[obj.__class__][name]
    except KeyError:
        return False
    if not property.has_default:
        return False
    return property.default == v
    
class _PropertyInfo(NamedTuple):
    type: VariantType
    name: str
    class_name: str
    hint: PropertyHint
    hint_string: str
    usage: PropertyUsageFlags
    
def get_properties(module_name: str) -> tuple[_PropertyInfo, ...]:
    script = _scripts[module_name]
    property_info: list[_PropertyInfo] = []
    for name, property in _properties[script].items():
        property_info.append(_PropertyInfo(
            property.variant_type,
            name,
            property.class_name,
            property.hint,
            property.hint_string,
            property.usage,
        ))
    return tuple(property_info)

_no_default = object()
    
class Export:

    hint = PropertyHint.PROPERTY_HINT_NONE
    hint_string = ""
    usage = PropertyUsageFlags.PROPERTY_USAGE_DEFAULT
    has_default = False
    default: Any = None
    
    def __init__(
        self,
        *,
        hint: PropertyHint | None = None,
        hint_string: str | None = None,
        usage: PropertyUsageFlags | None = None,
        default: Any = _no_default,
    ):
        if hint is not None:
            self.hint = hint
        if hint_string is not None:
            self.hint_string = hint_string
        if usage is not None:
            self.usage = usage
        if default is not _no_default:
            self.has_default = True
            self.default = default

_no_export = Export(usage=PropertyUsageFlags.PROPERTY_USAGE_NONE)


_type_variant_type = {
    bool: VariantType.TYPE_BOOL,
    int: VariantType.TYPE_INT,
}
def _type_to_variant_type(t) -> VariantType:
    return _type_variant_type.get(t, VariantType.TYPE_NIL)


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
