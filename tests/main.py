#!/usr/bin/python2

import requests
import json

def u(tokens = []):
    return "/".join(tokens)

def getUrl(subUrl):
    base = "{}://{}:{}".format("http", "127.0.0.1", 8080)
    if subUrl is not None:
    	base += "/" + subUrl
    return base 

def do(method, subUrl, data = None, comment = None):
    u = getUrl(subUrl)
    print "Making {0} request to {1} {2} data.\n{3}".format(method, u, "without" if data is None else "with " + json.dumps(data), "\t\t" + comment if comment is not None else "") 
    resp = requests.request(method, u) if data is None else requests.request(method, u, data = json.dumps(data)) 
    dumpResp(resp)

def dumpResp(resp):
    print "Code : {0}".format(resp.status_code)
    print "Data:\n{0}".format(resp.text)

def post(subUrl, data, comment = None):
    do("POST", subUrl, data, comment)
    
def get(subUrl = None, comment = None):
    do("GET", subUrl, None, comment)
    
def put(subUrl, data, comment = None):
    do("PUT", subUrl, data, comment)
    
def options(subUrl, data, comment = None):
    do("OPTIONS", subUrl, data, comment)
    
def patch(subUrl, data, comment = None):
    do("PATCH", subUrl, data, comment)
    
get(comment = "Requesting databases list")


db = "testDB"
collection = "testCollection"

put(db, "Creating db")
get(comment = "Requesting databases list")


put(u([db, collection]), "Creating collection")

patch(u([db, collection]), { "name" : "Eduard", "company" : "CMP1" })

doc = {}
doc["name"] = "Chuck"
doc["company"] = "ACME"

patch(u([db, collection]), doc)
get(u([db, collection, "1"]))
