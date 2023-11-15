#!/usr/bin/env python3

import datetime

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>Current time</h1>")
print("<p>{}</p>".format(datetime.datetime.now()))
print("</body></html>")
