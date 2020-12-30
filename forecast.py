import requests
from pprint import pprint
import math
import conditionconverter
import configparser

config = configparser.RawConfigParser()
config.read('config.txt')

config_details = dict(config.items('CONFIG'))

r = requests.get('https://api.openweathermap.org/data/2.5/onecall?lat=38.9&lon=-77.27&exclude=minutely,hourly,alerts&appid='+config_details['openweatherid']+'&units=imperial')
#print(r.status_code)
#pprint(r.json())

weather_data = r.json()

currentdate = weather_data['current']['sunset']
dailies = weather_data['daily']
for daily in dailies:
	if daily['dt'] > currentdate:
		tomorrow = daily
		break

temp = daily['temp']['max']
condition = daily['weather'][0]['description']

print('\nTemperature: ',math.trunc(temp))
print('\nConditions: ',condition)

# Convert condition to known value for device
conv_cond = conditionconverter.convert_condition(condition)
print('\nConverted condition: ',conv_cond)

postrequest = requests.post('https://io.adafruit.com/api/v2/webhooks/feed/'+config_details['iofeed']+'/raw', data = conv_cond+'; '+str(math.trunc(temp)))

print('\nIO Status: ',postrequest.status_code)
print('\nIO response: ',postrequest.text)
