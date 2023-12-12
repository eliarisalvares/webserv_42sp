#!/usr/bin/env python3

import os

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

directory_to_list = os.getcwd()
generate_directory_listing(directory_to_list)
