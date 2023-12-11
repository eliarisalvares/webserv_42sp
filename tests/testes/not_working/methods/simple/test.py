import unittest
import requests
import integration

# URL = "http://localhost:9000/"
# response = integration.get(URL)
# assert response.status_code == 200
# assert "root default" in response.text

URL = "http://localhost:9000/wizards.html"
URL = "http://localhost:9000"
data = {
  "newName": "teste",
  "newIconUrl": "https%3A%2F%2Fstorage.googleapis.com%2Fpod_public%2F1300%2F180935.jpg",
  "newImageUrl": "%2Fassets%2Fpotion.png",
}
# data = "newName=teste&newIconUrl=%2Fassets%2Fball.png&newIconUrl=%2Fassets%2Fpotion.png&newIconUrl=%2Fassets%2Fspell.png&newImageUrl=https%3A%2F%2Fstorage.googleapis.com%2Fpai-images%2F41345478f518418aac43a85e2156f2b6.jpeg&newImageUrl=https%3A%2F%2Fstorage.googleapis.com%2Fpod_public%2F1300%2F180941.jpg&newImageUrl=https%3A%2F%2Fstorage.googleapis.com%2Fpod_public%2F1300%2F180935.jpg&newImageUrl=https%3A%2F%2Fstorage.googleapis.com%2Fpai-images%2Fb162c70c5b224030b1dd4753b6fa317a.jpeg&newIconUrl=https%3A%2F%2Fstorage.googleapis.com%2Fpod_public%2F1300%2F180935.jpg&newImageUrl=%2Fassets%2Fpotion.png"
response = integration.post(URL, data=data)
assert response.status_code == 200
# assert "root default" in response.text

# URL = "http://localhost:9000/"
# response = integration.delete(URL)
# assert response.status_code == 200
# assert "root default" in response.text

