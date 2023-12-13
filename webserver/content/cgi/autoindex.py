#!/usr/bin/env python3

import os
from pathlib import Path


base_dir = Path(__file__).resolve().parent.parent
not_allowed_page_path = base_dir / "error_pages" / "405.html"
internal_error_page_path = base_dir / "error_pages" / "500.html"

def generate_directory_listing(dir_path):
    print("<!DOCTYPE html>")
    print("<html lang='en'>")
    print("<head>")
    print("<meta charset='UTF-8'>")
    print("<title>Directory Listing</title>")
    print("</head>")
    print("<body>")
    print("<h1>Directory Listing of /{}</h1>".format(dir_path.split('/')[-1]))
    print("<ul>")

    try:
        for item in os.listdir(dir_path):
            full_path = os.path.join(dir_path, item)
            if os.path.isdir(full_path):
                item += '/'
            print("<li><a href='{}'>{}</a></li>".format(item, item))
    except Exception as e:
        print("<p>Error: {}</p>".format(e))

    print("</ul>")
    print("</body>")
    print("</html>")


if (os.environ.get("REQUEST_METHOD", None) != "GET"):
    with open(not_allowed_page_path, "r") as f:
        print(f.read())
else:
    try:
        directory_to_list = os.getcwd()
        generate_directory_listing(directory_to_list)
    except Exception as e:
        with open(internal_error_page_path, "r") as f:
            print(f.read())
