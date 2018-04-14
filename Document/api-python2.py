#!/usr/bin/env python
# -*- coding:utf-8 -*-
import hashlib
import requests
import time
import urllib
import hmac
from collections import OrderedDict

# coin
coin = 'btc'
public_key = 'XXXXX'
private_key = 'XXXXX'

# send requests
def request(method,params):
	Od = OrderedDict()
	Od['coin'] = coin
	Od['key'] = public_key
	Od['nonce'] = long(time.time())
	Od['version'] = 2

	for i in params:
		Od[i] = params[i]

	Od['signature'] = signature(Od)
	response = requests.post('http://api.btctrade.com/api/'+method,data=Od)
	if response.status_code == 200:
		print(response.text)

# create signature
def signature(params):
	payload = urllib.urlencode(params)
	md5prikey = hashlib.md5(private_key).hexdigest()
	sign = urllib.quote(hmac.new(md5prikey, payload, digestmod=hashlib.sha256).hexdigest())
	return sign

# get balance
def balance():
	request('balance',{})


balance()


