
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


class Property(NamedTuple):
    type: Any
    variant_type: VariantType
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
    for k, v in get_annotations(obj, eval_str=True).items():
        type = v
        export = _no_export
        if hasattr(v, "__metadata__"):
            type = v.__args__[0]
            for meta in v.__metadata__:
                if meta is Export or isinstance(meta, Export):
                    export = meta
        try:
            default = getattr(obj, k)
            has_default = True
        except AttributeError:
            default = None
            has_default = False
        properties[k] = Property(
            type,
            _type_to_variant_type(type),
            has_default,
            default,
            export.hint,
            export.hint_string,
            export.usage,
        )
    
    return obj

    
def get_module_script(module_name):
    return _scripts[module_name]
    
    
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
    script = get_module_script(module_name)
    try:
        property = _properties[script][property_name]
    except KeyError:
        return None
    if property.has_default:
        return None
    return VariantWrapper.create_from_type(
        property.default,
        property.variant_type,
    )
    
def get_property_value(obj: Any, name: str) -> VariantWrapper:
    try:
        value = getattr(obj, name)
    except AttributeError:
        return VariantWrapper.create_nil()
    try:
        property = _properties[obj.__class__][name]
    except KeyError:
        return VariantWrapper.create_nil()
    return VariantWrapper.create_from_type(
        value,
        property.variant_type,
    )
    
class _PropertyInfo(NamedTuple):
    type: VariantType
    name: str
    class_name: str
    hint: PropertyHint
    hint_string: str
    usage: PropertyUsageFlags
    
def get_properties(module_name: str) -> tuple[_PropertyInfo, ...]:
    script = get_module_script(module_name)
    property_info: list[_PropertyInfo] = []
    for name, property in _properties[script].items():
        property_info.append(_PropertyInfo(
            property.variant_type,
            name,
            script.__name__,
            property.hint,
            property.hint_string,
            property.usage,
        ))
    return tuple(property_info)

    
    
class Export:

    hint = PropertyHint.PROPERTY_HINT_NONE
    hint_string = ""
    usage = PropertyUsageFlags.PROPERTY_USAGE_DEFAULT
    
    def __init__(
        self,
        *,
        hint: PropertyHint | None = None,
        hint_string: str | None = None,
        usage: PropertyUsageFlags | None = None,
    ):
        if hint is not None:
            self.hint = hint
        if hint_string is not None:
            self.hint_string = hint_string
        if usage is not None:
            self.usage = usage

_no_export = Export(usage=PropertyUsageFlags.PROPERTY_USAGE_NONE)


_type_variant_type = {
    bool: VariantType.TYPE_BOOL,
    int: VariantType.TYPE_INT,
}
def _type_to_variant_type(t) -> VariantType:
    return _type_variant_type.get(t, VariantType.TYPE_NIL)
