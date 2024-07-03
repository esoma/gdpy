
import json
from jinja2 import Environment, FileSystemLoader
from pathlib import Path

python_keyword_safe = {
    "class": "cls",
}
def safe_token(n):
    return python_keyword_safe.get(n, n)

godot_type_python = {}
def godot_type_name_to_python(t):
    return godot_type_python.get(t, "Any")

env = Environment(
    loader=FileSystemLoader(Path(__file__).parent / "templates"),
)
env.globals["godot_type_name_to_python"] = godot_type_name_to_python
env.globals["safe_token"] = safe_token
class_template = env.get_template("class.py")


with open("vendor/godot-cpp/gdextension/extension_api.json", "r") as f:
    godot_api = json.load(f)

for cls in godot_api["classes"]:
    with open(Path("build") / f"{cls['name'].lower()}.py", "w") as f:
        f.write(class_template.render(**({"inherits": None} | cls)))

