import unittest
import requests
import integration

URL = "http://localhost:9000/examples"

response = integration.get(URL)

assert response.status_code == 200

assert '<a href="..">..</a>' in response.text
assert '<a href=".">.</a>' in response.text
assert '<a href="hello">hello</a>' in response.text
assert '<a href="hello.html">hello.html</a>' in response.text
assert '<a href="images">images</a>' in response.text
