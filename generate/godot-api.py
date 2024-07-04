
import json
from jinja2 import Environment, FileSystemLoader
from pathlib import Path

python_keyword_safe = {
    "class": "cls",
    "pass": "pass_",
}
def safe_token(n):
    return python_keyword_safe.get(n, n)

godot_type_python = {
    "int": "int",
    "String": "str",
    "bool": "bool",
    "float": "float",
    "Variant": "Variant",
}
def godot_type_name_to_python(t):
    return godot_type_python.get(t, "Any")

env = Environment(
    loader=FileSystemLoader(Path(__file__).parent / "templates"),
)
env.globals["godot_type_name_to_python"] = godot_type_name_to_python
env.globals["safe_token"] = safe_token
class_template = env.get_template("class.py")
enum_template = env.get_template("enum.py")


with open("vendor/godot-cpp/gdextension/extension_api.json", "r") as f:
    godot_api = json.load(f)


for enum in godot_api["global_enums"]:
    with open(Path("build") / f"{enum['name'].lower()}.py", "w") as f:
        f.write(enum_template.render(**enum))

for cls in godot_api["classes"]:
    references = set()
    for method in cls.get("methods", []):
        for argument in method.get("arguments", []):
            references.add(argument["type"])
        try:
            references.add(method["return_value"]["type"])
        except KeyError:
            pass
    references -= set(godot_type_python.keys())
    original_godot_type_python = godot_type_python
    godot_type_python = dict(godot_type_python)
    imports = set()
    for reference in references:
        name = reference.split("::")[-1]
        parts = name.split(".")
        godot_type_python[reference] = name
        if parts[0] != cls["name"]:
            module = f"gdpy.{parts[0].lower()}"
            imports.add((module, parts[0]))
    
    with open(Path("build") / f"{cls['name'].lower()}.py", "w") as f:
        f.write(class_template.render(
            **({"inherits": None} | cls),
            imports=imports
        ))
    godot_type_python = original_godot_type_python
