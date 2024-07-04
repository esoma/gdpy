
from __future__ import annotations

__all__ = ["{{ name }}"]

from typing import Any
from _gdpy import Variant

class {{ name }}:
    _gdpy_variant: Variant | None = None
    
    def __str__(self) -> None:
        try:
            to_string = self.to_string
        except AttributeError:
            return repr(self)
        return self.to_string()
        
    def __init__(self, *, _gdpy_variant: Variant):
        self._gdpy_variant = _gdpy_variant

{% for method in methods %}
    def {{ method["name"] }}(
        self,
{%- for argument in method["arguments"] %}
        {{ safe_token(argument["name"]) }}: {{ godot_type_name_to_python(argument["type"]) }},
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
        return_variant = self._gdpy_variant.call(
            {{ method["name"] }}, [
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