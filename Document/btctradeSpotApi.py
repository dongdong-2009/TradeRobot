import requests

class btctradeSpotApi:
    def __init__(self,url):
        self.__url=url

    def ticker(self,symbol=''):
        resource='/api/ticker?coin='
        params=''
        if symbol:
            resource=resource+'%(symbol)s' %{'symbol':symbol}
            method=self.__url+resource
        r=requests.get(method)
        # 顺利的话
        if r.status_code==200:
            return r.text
        else:
            print('the btctrade has something wrong with internet')


# 基本信息
url='http://api.btctrade.com'
btctrade=btctradeSpotApi(url)
r=btctrade.ticker('btc')

print(r)
print(len(r))
# 把json字符串转为字典
import json
a=json.loads(r)
print(type(a))
print(a['last'])

