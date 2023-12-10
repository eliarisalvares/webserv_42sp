import unittest
import requests
import integration

URL = "http://localhost:9000/examples"

response = integration.get(URL)

assert response.status_code == 200

assert '<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre><a href=".">.</a>' in response.text
