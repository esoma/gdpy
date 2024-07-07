
__all__ = ["narrow_variant_to"]

from _gdpy import VariantWrapper
from typing import Any
from enum import IntEnum
from types import GenericAlias

class _InvalidConversion(RuntimeError):
    pass
    
def create_variant(obj: Any, target: str) -> VariantWrapper:
    if obj is None:
        return VariantWrapper.create_nil()
    try:
        method = getattr(VariantWrapper, f"create_{target}")
    except AttributeError:
        raise RuntimeError(f"unable to convert to {target}")
    return method(obj)

def narrow_variant_to(variant: VariantWrapper, target: Any) -> Any:
    if variant is None:
        return None
        
    try:
        is_int_enum = issubclass(target, IntEnum)
    except TypeError:
        is_int_enum = False
    if is_int_enum:
        return target(variant.narrow_int())
        
    if isinstance(target, GenericAlias):
        if target.__origin__ is list:
            print("list of", target.__args__[0])
            return variant.narrow_list(
                lambda i: narrow_variant_to(i, target.__args__[0])
            )
        
    target = str(target.__name__)
    if target == "Variant":
        target = variant.get_target()
    try:
        return _narrow_variant_to_primitive(variant, target)
    except _InvalidConversion:
        pass
    try:
        return _narrow_variant_to_class(variant, target)
    except _InvalidConversion:
        pass
    raise RuntimeError(f"{target!r} is not a known target for Variants")
    
def _narrow_variant_to_primitive(variant: VariantWrapper, target: Any) -> Any:
    try:
        narrow = getattr(variant, f"narrow_{target}")
    except AttributeError:
        raise _InvalidConversion()
    return narrow()
    
def _narrow_variant_to_class(variant: VariantWrapper, target: Any) -> Any:
    try:
        module = __import__(f"gdpy.{target.lower()}", fromlist=[""])
    except ImportError:
        raise _InvalidConversion()
    cls = getattr(module, target)
    instance = cls(__gdpy_variant__=variant)
    return instance