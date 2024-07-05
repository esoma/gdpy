
__all__ = ["get_property_default_value"]

from gdpy._script import get_module_script, Property, get_properties as get_properties_
from typing import NamedTuple
from gdpy._varianttype import VariantType
from _gdpy import VariantWrapper
from gdpy.propertyhint import PropertyHint
from gdpy.propertyusageflags import PropertyUsageFlags

def get_property_default_value(
    module_name: str,
    property_name: str
) -> tuple[bool, VariantWrapper]:
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
    hint_string: str
    usage: PropertyUsageFlags
    
def get_properties(module_name: str) -> tuple[PropertyInfo, ...]:
    script = get_module_script(module_name)
    property_info: list[PropertyInfo] = []
    for name, property in get_properties_(script).items():
        property_info.append(PropertyInfo(
            property.type,
            name,
            script.__name__,
            property.hint,
            property.hint_string,
            property.usage,
        ))
    return tuple(property_info)
