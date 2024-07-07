
import json
from jinja2 import Environment, FileSystemLoader
from pathlib import Path
from argparse import ArgumentParser
import shutil

arg_parser = ArgumentParser()
arg_parser.add_argument("-b", "--build", default="build")
args = arg_parser.parse_args()

build_dir = Path(args.build) / "gdpy"
build_dir.mkdir(parents=True, exist_ok=True)

python_keyword_safe = {
    "class": "cls",
    "pass": "pass_",
    "from": "from_",
    "with": "with_",
    "global": "global_",
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
    
godot_type_name_to_variant_target = {
}
def godot_type_name_to_variant_target(t):
    if t.startswith("enum::"):
        return "int"
    if t.startswith("bitfield::"):
        return "int"
    return t
    
godot_value_python = {
    "null": None,
    "false": False,
    "true": True,
}
def godot_value_to_python(t):
    try:
        return godot_value_python[t]
    except KeyError:
        return t

env = Environment(
    loader=FileSystemLoader(Path(__file__).parent / "templates"),
)
env.globals["godot_value_to_python"] = godot_value_to_python
env.globals["godot_type_name_to_python"] = godot_type_name_to_python
env.globals["safe_token"] = safe_token
env.globals["godot_type_name_to_variant_target"] = godot_type_name_to_variant_target
builtin_template = env.get_template("builtin.py")
class_template = env.get_template("class.py")
enum_template = env.get_template("enum.py")


with open("vendor/godot-cpp/gdextension/extension_api.json", "r") as f:
    godot_api = json.load(f)

for enum in godot_api["global_enums"]:
    private = ""
    if enum["name"].startswith("Variant."):
        private = "_"
        enum["name"] = "Variant" + enum["name"].removeprefix("Variant.")
        
    with open(build_dir / f"{private}{enum['name'].lower()}.py", "w") as f:
        f.write(enum_template.render(**enum))
        
for builtin in godot_api["builtin_classes"]:
    with open(build_dir / f"{builtin['name'].lower()}.py", "w") as f:
        f.write(builtin_template.render(**builtin))

for cls in godot_api["classes"]:
    annotation_references = set()
    references = set()
    for method in cls.get("methods", []):
        for argument in method.get("arguments", []):
            if "default_value" in argument:
                references.add(argument["type"])
            else:
                annotation_references.add(argument["type"])
        try:
            annotation_references.add(method["return_value"]["type"])
        except KeyError:
            pass
    references -= set(godot_type_python.keys())
    annotation_references -= set(godot_type_python.keys())
    original_godot_type_python = godot_type_python
    godot_type_python = dict(godot_type_python)
    imports = set()
    annotation_imports = set()
    for reference in references:
        name = reference.split("::")[-1]
        parts = name.split(".")
        godot_type_python[reference] = name
        if parts[0] != cls["name"]:
            module = f"gdpy.{parts[0].lower()}"
            imports.add((parts[0], module))
    for reference in annotation_references:
        name = reference.split("::")[-1]
        parts = name.split(".")
        godot_type_python[reference] = name
        if parts[0] != cls["name"]:
            module = f"gdpy.{parts[0].lower()}"
            annotation_imports.add((parts[0], module))
    annotation_imports -= imports
    annotation_imports = dict(annotation_imports)
    
    #
    method_names = {m["name"] for m in cls.get("methods", [])}
    for property in cls.get("properties", []):
        if "setter" not in property:
            continue
        if property["setter"] not in method_names:
            property["setter"] = f"{cls['inherits']}.{property['setter']}"

    with open(build_dir / f"{cls['name'].lower()}.py", "w") as f:
        f.write(class_template.render(
            **({"inherits": None} | cls),
            imports=imports,
            annotation_imports=annotation_imports,
        ))
    godot_type_python = original_godot_type_python

for static_file in (Path(__file__).parent / "static").iterdir():
    shutil.copy(static_file, build_dir / static_file.name)
