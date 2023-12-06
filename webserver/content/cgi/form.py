#!/usr/bin/env python3
import cgi
import html
import json

form = cgi.FieldStorage()

with open("form.txt", "w") as f:
    f.write("form: " + str(form) + "\n")

new_name = html.escape(form.getvalue('newName', ''))
new_info = html.escape(form.getvalue('newInfo', ''))
new_image_url = html.escape(form.getvalue('newImageUrl', ''))

with open("form.txt", "a") as f:
     f.write("newName: " + new_name + "\n")
     f.write("newInfo: " + new_info + "\n")
     f.write("newImageUrl: " + new_image_url + "\n")

response = {}
if new_name and new_info and new_image_url:
    response["success"] = True
    response["name"] = new_name
    response["info"] = new_info
    response["imageUrl"] = new_image_url
    # Add any additional processing here
    with open("form.txt", "a") as f:
        f.write("success: " + str(response["success"]) + "\n")
        f.write("name: " + response["name"] + "\n")
        f.write("info: " + response["info"] + "\n")
        f.write("imageUrl: " + response["imageUrl"] + "\n")
else:
    response["success"] = False
    response["error"] = "Please provide all the details."

print("Content-Type: application/json")
print()
print(json.dumps(response))