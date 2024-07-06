
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
