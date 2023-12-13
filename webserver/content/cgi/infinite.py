# test infinite loop

import time

while True:
    print("Content-Type: text/html\n")
    print("<html><body><h1>It works!</h1></body></html>")
    time.sleep(1)

