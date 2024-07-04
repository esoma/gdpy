
__all__ = ["{{ name }}"]

from enum import IntEnum, IntFlag

class {{ name }}(
{%- if is_bitfield -%}
    IntFlag
{%- else -%}
    IntEnum
{%- endif -%}
):
{%- for enum_value in values %}
    {{ enum_value["name"] }} = {{ enum_value["value"] }}
{%- endfor %}