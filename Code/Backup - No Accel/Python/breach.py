import datetime
import logging
import requests
import socket
import sys
import time
import pywifi
from pywifi import const

AP = 'ATTA5qYQs2'
ESP = 'AJ'

wifi = pywifi.PyWiFi()
iface = wifi.interfaces()[0]
profile = pywifi.Profile()
print("Initializing: Please place your Security System and press the reset button.\nYou have 15 seconds.")
time.sleep(15)
print("System Armed")

while(1):
    reconnect = False
    #Open socket to ESP
    try:
        soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        soc.connect(('192.168.0.222', 80))
        msg = soc.recv(5)
        out = msg.decode("utf-8")
        if(out == "Moved"):
            print(out)
            reconnect = True
            soc.close()
            iface.disconnect()
            time.sleep(0.25)
            assert iface.status() in\
                [const.IFACE_DISCONNECTED, const.IFACE_INACTIVE]
            profile.ssid = AP
            profile.auth = const.AUTH_ALG_OPEN
            profile.akm.append(const.AKM_TYPE_NONE)
            profile.cipher = const.CIPHER_TYPE_CCMP
            profile.key = '12345678'
            profile = iface.add_network_profile(profile)
            iface.connect(profile)
            print("Connected to " + str(AP))
            time.sleep(5)
            #notify user of breach
            requests.post("https://maker.ifttt.com/trigger/breached/with/key/Qzhe5UWU5AUSnoBumev4U")        
    except socket.error as e:
        print("Error: %s" %(e))
    finally:
        if(reconnect):
            #Setup date for timestamp and logger object
            date =  datetime.datetime.now()
            logger = logging.getLogger("Security-Breach")
            handler = logging.FileHandler('breach.log')
            handler.setLevel(logging.WARNING)
            format = logging.Formatter("[%(levelname)s] " + str(date) + ": %(message)s")
            handler.setFormatter(format)
            logger.addHandler(handler)
            #log breach
            logger.warning("A breach was detected!")        
            #Disconnect from AP and reconnect to ESP
            iface.disconnect()
            time.sleep(0.25)
            assert iface.status() in\
                [const.IFACE_DISCONNECTED, const.IFACE_INACTIVE]
            profile.ssid = ESP
            profile.auth = const.AUTH_ALG_OPEN
            profile.akm.append(const.AKM_TYPE_NONE)
            profile = iface.add_network_profile(profile)
            iface.connect(profile)
            print("Connected to " + str(ESP))
            time.sleep(3)
