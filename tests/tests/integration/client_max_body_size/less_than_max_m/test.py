import unittest
import requests
import integration

URL = "http://localhost:9000"

response = integration.post(URL, integration.text_lorem_ipsum_1732b)

assert response.status_code == 200

