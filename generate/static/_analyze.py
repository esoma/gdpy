
__all__ = ["get_property_default_value"]

from _gdpy import Variant
from gdpy._script import get_module_script, Property, get_properties as get_properties_
from typing import NamedTuple
from gdpy._varianttype import VariantType
from gdpy.propertyhint import PropertyHint
from gdpy.propertyusageflags import PropertyUsageFlags

def get_property_default_value(
    module_name: str,
    property_name: str
) -> tuple[bool, Variant]:
    script = get_module_script(module_name)
    try:
        property = get_properties_(script)[property_name]
    except KeyError:
        return (False, None)
    return (property.has_default, property.default)
    
class PropertyInfo(NamedTuple):
    type: VariantType
    name: str
    class_name: str
    hint: PropertyHint
    usage: PropertyUsageFlags
    
def get_properties(module_name: str) -> tuple[PropertyInfo, ...]:
    script = get_module_script(module_name)
    property_info: list[PropertyInfo] = []
    for name, property in get_properties_(script).items():
        property_info.append(PropertyInfo(
            VariantType.TYPE_BOOL,
            name,
            script.__name__,
            PropertyHint.PROPERTY_HINT_NONE,
            PropertyUsageFlags.PROPERTY_USAGE_NONE,
        ))
    print(property_info)
    return tuple(property_info)

    