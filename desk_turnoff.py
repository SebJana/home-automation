import requests
import subprocess
import platform
from datetime import datetime, time as dtime
import time
import sys

# Shelly IP
SHELLY_IP = "192.168.178.114"
# PC IP WIFI/LAN
PC_IP_WIFI = "192.168.178.112"
PC_IP_LAN = "192.168.178.38"
# Retry interval
RETRY_INTERVAL = 5 * 60 # 5 minutes
# Turn-off/-on time
TURN_OFF_TIME = dtime(22, 0)  # 22:00
TURN_ON_TIME = dtime(8, 0)    # 08:00
# Counter of consecutive unsuccessful PC pings
NOT_REACHED_PING_COUNTER = 0

def is_shelly_on(shelly_ip):
    url = f"http://{shelly_ip}/relay/0"
    response = requests.get(url)
    data = response.json()

    return data["ison"] 

def turn_off_shelly(shelly_ip):
    url = f"http://{shelly_ip}/relay/0?turn=off"
    response = requests.get(url)
    print("Response:", response.text)

def is_reachable(host):
    param = "-n" if platform.system().lower() == "windows" else "-c"
    command = ["ping", param, "1", host]
    try:
        result = subprocess.run(
            command,
            capture_output=True,  
            text=True,            
            timeout=3             
        )
        output = result.stdout.lower()

        if "unreachable" in output or "timed out" in output:
            return False
        return True
    except Exception as e:
        print(f"Error pinging {host}: {e}")
        return False

def is_night_time():
    now = datetime.now().time()
    if TURN_OFF_TIME < TURN_ON_TIME:
        # Normal same-day range (e.g. 20:00–08:00 is invalid)
        return TURN_OFF_TIME <= now < TURN_ON_TIME
    else:
        # Overnight range (e.g. 22:00–08:00)
        return now >= TURN_OFF_TIME or now < TURN_ON_TIME

while True:
    # Check if it's nighttime yet
    if not is_night_time():
        time.sleep(RETRY_INTERVAL)
        continue

    # Check if the PC is in use
    if is_reachable(PC_IP_WIFI) or is_reachable(PC_IP_LAN):
        PC_LAST_REACHED = time.time()
        NOT_REACHED_PING_COUNTER = 0
    else:
        NOT_REACHED_PING_COUNTER += 1
    
    # PC offline for 3 or more iterations
    if NOT_REACHED_PING_COUNTER >= 3:
        if is_shelly_on(SHELLY_IP):
            turn_off_shelly(SHELLY_IP)
        sys.exit()

    time.sleep(RETRY_INTERVAL)
