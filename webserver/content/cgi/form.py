#!/usr/bin/env python3
import json
import os

content = os.environ.get("CONTENT_BODY", None)
if not content:
    with open("content/error_pages/400.html", "r") as f:
        print(f.read())

try:
    new_image_url_key = content.split("&")[0]
    new_image_url = new_image_url_key.split("=")[1]
    new_icon_key = content.split("&")[1]
    new_icon = new_icon_key.split("=")[1]
    new_name_key = content.split("&")[2]
    new_name = new_name_key.split("=")[1]

    html_content = "<div class='card'>"
    html_content += f"<img src='{new_image_url}' alt='Character 4'>"
    html_content += f"<img src='{new_icon}' class='icon' alt='Icon'>"
    html_content += f"<div class='card-title'>{new_name}</div>"
    html_content += "</div>"
    print(html_content)

    file = open("content/database/characters.html", "a")
    file.write(html_content)
    file.close()

except:
    with open("content/error_pages/400.html", "r") as f:
        print(f.read())


