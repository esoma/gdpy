
__all__ = ["narrow_variant_to"]

from _gdpy import VariantWrapper
from typing import Any
from enum import IntEnum

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
    instance = cls(_gdpy_variant=variant)
    return instance