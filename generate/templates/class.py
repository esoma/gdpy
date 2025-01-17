
from __future__ import annotations

__all__ = ["{{ name }}"]

from enum import IntEnum, IntFlag
from typing import Final, Any, TYPE_CHECKING
from _gdpy import call_method_bind, class_db_get_method, VariantWrapper
from gdpy._variant import narrow_variant_to, create_variant

{% if annotation_imports %}
if TYPE_CHECKING:
{% for name, module in annotation_imports.items() %}
    from {{ module }} import {{ name }}
{%- endfor %}
{% endif %}

{% for name, module in imports %}
from {{ module }} import {{ name }}
{%- endfor %}

def free_object(*args, **kwargs): pass
def narrow_variant_to_python(thing, *args, **kwargs): return thing
def signal(*args, **kwargs): return object()

{% if inherits %}
from gdpy.{{ inherits.lower() }} import {{ inherits }}
{% endif %}

class {{ name }}
{%- if inherits -%}
    ({{ inherits }})
{%- endif -%}
:
    
{% if not inherits %}
    __gdpy_class_name__ = "{{ name }}"
    
    __gdpy_variant__: VariantWrapper | None = None
    
    def free(self) -> None:
        if self.__gdpy_variant__ is not None:
            free_object(self.__gdpy_variant__)
            self.__gdpy_variant__ = None
        
    def __del__(self) -> None:
        self.free()
        
    def __str__(self) -> None:
        try:
            to_string = self.to_string
        except AttributeError:
            return repr(self)
        return self.to_string()
        
    def __init__(self, *, __gdpy_variant__: VariantWrapper):
        self.__gdpy_variant__ = __gdpy_variant__
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
{% if not method["is_virtual"] %}
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
        *varargs: VariantWrapper,
{%- endif %}
    ) -> (
{%- if "return_value" in method -%}
    {{ godot_type_name_to_python(method["return_value"]["type"]) }}
{%- else -%}
    None
{%- endif -%}
    ):
        method = class_db_get_method("{{ name }}", "{{ method["name"] }}")
        return_variant = call_method_bind(
            method,
            {% if method["is_static"] %}None{% else %}self.__gdpy_variant__{% endif %}, (
{%- for argument in method["arguments"] %}
            create_variant(
                {{ safe_token(argument["name"]) }},
                "{{ godot_type_name_to_variant_target(argument["type"]) }}"
            ),
{%- endfor %}
{%- if method["is_vararg"] %}
            *varargs,
{%- endif %}
        ))
{%- if "return_value" in method %}
{%- with return_type = godot_type_name_to_python(method['return_value']['type']) %}
{%- if return_type in annotation_imports %}
        from {{ annotation_imports[return_type] }} import {{ return_type }}
{%- endif %}
        return narrow_variant_to(
            return_variant,
            {{ return_type }}
        )
{%- endwith %}
{%- endif -%}
{% endif %}
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
{%- if "setter" in property %}
        {{ property["setter"] }},
{%- endif %}
    )
{% endfor %}
