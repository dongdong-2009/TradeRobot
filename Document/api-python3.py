#!/usr/bin/env python
# -*- coding:utf-8 -*-
import hashlib
import requests
import time
import urllib
import hmac
import urllib.parse
from collections import OrderedDict

# coin
coin = 'btc'
public_key = 'XXX'
private_key = 'XXX'

# send requests
def request(method,params):
	Od = OrderedDict()
	Od['coin'] = coin
	Od['key'] = public_key
	Od['nonce'] = int(time.time())
	Od['version'] = 2

	for i in params:
		Od[i] = params[i]

	Od['signature'] = signature(Od)
	response = requests.post('http://api.btctrade.com/api/'+method,data=Od)
	if response.status_code == 200:
		print(response.text)

# create signature
def signature(params):
	# 修改过了，加了个parse：因为python3
	payload = urllib.parse.urlencode(params)
	# 修改过了，一定要encode一下，因为python3
	pl=payload.encode()
	# 修改过了，加了个encode: 因为python3
	pk=private_key.encode()

	md5prikey = hashlib.md5(pk).hexdigest()
	# 加了一个encode，因为python3
	md5prikey=md5prikey.encode()

	# 修改过了，加了个parse：因为python3
	sign = urllib.parse.quote(hmac.new(md5prikey, pl, digestmod=hashlib.sha256).hexdigest())
	return sign

# get balance
def balance():
	request('balance',{})


balance()


