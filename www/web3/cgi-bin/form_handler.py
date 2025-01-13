#!/usr/bin/python3

import os

# Print the header for a successful CGI script response
print("Content-Type: text/html")
print()

# Get the form data

name = os.getenv("name")
age = os.getenv("age")

# Output the data received
print(f"<h2>Received Data:</h2>")
print(f"<p>Name: {name}</p>")
print(f"<p>Age: {age}</p>")
