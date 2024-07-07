
from __future__ import annotations

__all__ = ["{{ name }}"]

from typing import Any
from _gdpy import VariantWrapper
from gdpy._variant import narrow_variant_to

class {{ name }}:
    __gdpy_variant__: VariantWrapper | None = None
    
    def __str__(self) -> None:
        try:
            to_string = self.to_string
        except AttributeError:
            return repr(self)
        return self.to_string()
        
    def __init__(self, *args, __gdpy_variant__: VariantWrapper = None, **kwargs):
        self.__gdpy_variant__ = __gdpy_variant__

{% for method in methods %}
    def {{ method["name"] }}(
        self,
{%- for argument in method["arguments"] %}
        {{ safe_token(argument["name"]) }}: {{ godot_type_name_to_python(argument["type"]) }},
{%- endfor %}
{%- if method["is_vararg"] %}
        *varargs: VariantWrapper,
{%- endif %}
    ) -> (
{%- if "return_value" in method -%}
    {{ godot_type_name_to_python(method["return_value"]["type"]) }}
{%- else -%}
    None
{%- endif -%}
    ):
        return_variant = self.__gdpy_variant__.call_method(
            "{{ method["name"] }}", [
{%- for argument in method["arguments"] %}
            {{ safe_token(argument["name"]) }},
{%- endfor %}
{%- if method["is_vararg"] %}
            *varargs,
{%- endif %}
        ])
{%- if "return_type" in method %}
        return narrow_variant_to(
            return_variant,
            {{ godot_type_name_to_python(method['return_type']) }}
        )
{%- endif %}
{% endfor %}