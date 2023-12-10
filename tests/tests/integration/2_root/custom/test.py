import unittest
import requests
import integration

URL_1 = "http://localhost:9000/index.html"

def check(URL):
	response = integration.get(URL)
	assert response.status_code == 200
	assert "root path" in response.text

check(URL_1)
