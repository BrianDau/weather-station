def convert_condition(raw_cond):
	if "rain" in raw_cond or "shower" in raw_cond or "drizzle" in raw_cond:
		return "rain"
	elif "storm" in raw_cond:
		return "thunderstorms"
	elif "sun" in raw_cond or "clear" in raw_cond or "fair" in raw_cond:
		return "sunny"
	elif "snow" in raw_cond or "sleet" in raw_cond:
		return "snow"
	elif "partly" in raw_cond:
		return "partly cloudy"
	elif "cloud" in raw_cond or "fog" in raw_cond or "mist" in raw_cond:
		return "cloudy"
	else:
		return "sunny" 

