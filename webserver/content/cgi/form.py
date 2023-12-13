#!/usr/bin/env python3
from pathlib import Path
import os


base_dir = Path(__file__).resolve().parent.parent
database_path = base_dir / "database" / "characters.csv"
error_page_path = base_dir / "error_pages" / "400.html"
not_allowed_page_path = base_dir / "error_pages" / "405.html"
internal_error_page_path = base_dir / "error_pages" / "500.html"

if (os.environ.get("REQUEST_METHOD", None) != "POST"):
    with open(not_allowed_page_path, "r") as f:
        print(f.read())
else:
    os.chmod(database_path, 0o777)
    content = os.environ.get("QUERY_STRING", None)
    if content is None:
        with open(error_page_path, "r") as f:
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

        with open(database_path, "a") as file:
            file.write("\n")
            file.write(html_content)
            file.close()

    except Exception as e:
        with open(internal_error_page_path, "r") as f:
            print(f.read())


