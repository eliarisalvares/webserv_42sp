import unittest
import requests
import integration

URL = "http://localhost:9000/examples"

response = integration.get(URL)

assert response.status_code == 200

assert '<a href="/examples/..">..</a>' in response.text
assert '<a href="/examples/.">.</a>' in response.text
assert '<a href="/examples/hello">hello</a>' in response.text
assert '<a href="/examples/hello.html">hello.html</a>' in response.text
assert '<a href="/examples/images">images</a>' in response.text
