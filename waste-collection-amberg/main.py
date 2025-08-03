from fastapi import FastAPI
import re

app = FastAPI()

@app.get("/")
def ping():
    return {"message" : "Hello from the Waste Collection Amberg API"}

def is_valid_code(code):
    pattern = r'^[A-E][1-4]$'

    if re.match(pattern, code):
        return True
    else:
        return False