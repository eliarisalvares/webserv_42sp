#!/usr/bin/env python3
import json
import os
import uuid

content = os.environ.get("CONTENT_BODY", None)
if not content:
    print("Content-Type: application/json")
    print()
    print(json.dumps({"success": False, "error": "No content provided."}))
    exit(0)

new_image_url_key = content.split("&")[0]
new_image_url = new_image_url_key.split("=")[1]
new_icon_key = content.split("&")[1]
new_icon = new_icon_key.split("=")[1]
new_name_key = content.split("&")[2]
new_name = new_name_key.split("=")[1]

print("<div class='card'>")
print(f"<img src='{new_image_url}' alt='Character 4'>")
print(f"<img src='{new_icon}' class='icon' alt='Icon'>")
print(f"<div class='card-title'>{new_name}</div>")
print("</div>")

