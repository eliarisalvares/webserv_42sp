#!/usr/bin/env python3

import datetime
import os

# Print HTTP headers
print("Content-Type: text/html")
print()

# Start HTML document
print("<html><head><title>Server Time and Info</title></head>")
print("<style>")
print("body { background-color: #ffe6f2; font-family: Arial, sans-serif; }")
print("h1, h2 { color: #cc0066; }")
print("p { color: #800040; }")
print("</style>")
print("<body>")

# Print current time
print("<h1>Current Time</h1>")
print("<p>{}</p>".format(datetime.datetime.now()))

# Print some server details
print("<h2>Server Details</h2>")
print("<p>Server Name: {}</p>".format(os.environ.get('SERVER_NAME', 'Unknown')))
print("<p>Server Protocol: {}</p>".format(os.environ.get('SERVER_PROTOCOL', 'Unknown')))
print("<p>Request Method: {}</p>".format(os.environ.get('REQUEST_METHOD', 'Unknown')))
print("<p>Path Info: {}</p>".format(os.environ.get('PATH_INFO', 'Unknown')))
print("<p>Remote Address: {}</p>".format(os.environ.get('REMOTE_ADDR', 'Unknown')))

# End HTML document
print("</body></html>")
