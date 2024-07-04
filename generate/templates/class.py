
from __future__ import annotations

__all__ = ["{{ name }}"]

from enum import IntEnum, IntFlag
from typing import Final, Any
from _gdpy import call_method_bind, class_db_get_method, Variant
from gdpy._narrow import narrow_variant_to

{% for module, name in imports %}
from {{ module }} import {{ name }}
{%- endfor %}

def free_object(*args, **kwargs): pass
def narrow_variant_to_python(thing, *args, **kwargs): return thing
def signal(*args, **kwargs): return object()

{% if inherits %}
from .{{ inherits.lower() }} import {{ inherits }}
{% endif %}

class {{ name }}
{%- if inherits -%}
    ({{ inherits }})
{%- endif -%}
:
{% if not inherits %}
    _gdpy_variant: Variant | None = None
    
    def free(self) -> None:
        if self._gdpy_variant is not None:
            free_object(self._gdpy_variant)
            self._gdpy_variant = None
        
    def __del__(self) -> None:
        self.free()
        
    def __str__(self) -> None:
        try:
            to_string = self.to_string
        except AttributeError:
            return repr(self)
        return self.to_string()
        
    def __init__(self, *, _gdpy_variant: Variant):
        self._gdpy_variant = _gdpy_variant
{%- endif %}

{% for constant in constants %}
    {{ constant["name"] }}: Final = {{ constant["value"] }}
{%- endfor %}

{% for enum in enums %}
    class {{ enum["name"] }}(
{%- if enum["is_bitfield"] -%}
        IntFlag
{%- else -%}
        IntEnum
{%- endif -%}
    ):
{%- for enum_value in enum["values"] %}
        {{ enum_value["name"] }} = {{ enum_value["value"] }}
{%- endfor %}
{% endfor %}

{%- for method in methods %}
{% if method["is_static"] %}
    @staticmethod
{%- endif %}
    def {{ method["name"] }}(
{%- if not method["is_static"] %}
        self,
{%- endif %}
{%- for argument in method["arguments"] %}
        {{ safe_token(argument["name"]) }}: {{ godot_type_name_to_python(argument["type"]) }}
{%- if "default_value" in argument -%}
        ={{ godot_value_to_python(argument["default_value"]) }}
{%- endif -%},
{%- endfor %}
{%- if method["is_vararg"] %}
        *varargs: Variant,
{%- endif %}
    ) -> (
{%- if "return_value" in method -%}
    {{ godot_type_name_to_python(method["return_value"]["type"]) }}
{%- else -%}
    None
{%- endif -%}
    ):
{%- if not method["is_static"] %}
        if self._gdpy_variant is None:
            raise RuntimeError(f"{self!r} has been free'd")
{%- endif %}
{%- if method["is_static"] %}
        method = class_db_get_method("{{ name }}", "{{ method["name"] }}")
        return_variant = call_method_bind(
            method,
            None, [
{%- for argument in method["arguments"] %}
            {{ safe_token(argument["name"]) }},
{%- endfor %}
{%- if method["is_vararg"] %}
            *varargs,
{%- endif %}
        ])
{% else %}
        return_variant = self._gdpy_variant.call_method(
            "{{ method["name"] }}", [
{%- for argument in method["arguments"] %}
            {{ safe_token(argument["name"]) }},
{%- endfor %}
{%- if method["is_vararg"] %}
            *varargs,
{%- endif %}
        ])
{% endif %}
{%- if "return_value" in method %}
        return narrow_variant_to(
            return_variant,
            {{ godot_type_name_to_python(method['return_value']['type']) }}
        )
{%- endif -%}
{%- endfor %}

{% for signal in signals %}
    @signal
    class {{ signal["name"] }}:
{%- if "arguments" in signal %}
{%- for argument in signal["arguments"] %}
        {{ argument["name"] }}: {{ godot_type_name_to_python(argument["type"]) }}
{%- endfor %}
{%- else %}
        pass
{%- endif %}
{% endfor %}

{% for property in properties %}
    {{ property["name"] }} = property(
        {{ property["getter"] }}, 
        {{ property["setter"] }},
    )
{% endfor %}