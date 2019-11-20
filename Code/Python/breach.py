#Alex Jasper
#10/26/19
#This program creates and connects a socket to a specified IP and port.
#If a proper message is set, the program logs a breach of security with a timestamp/message
#and connects to the IFTTT web hook to call a specified phone number.
#It is intended to run during a security breach.

import datetime
import logging
import requests
import socket
import sys
import time
import pywifi
from pywifi import const

while(1):
    try:
        soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        soc.connect(('192.168.0.222', 80))
        msg = soc.recv(2048)
        out = msg.decode("utf-8")
        if out == "Moved":
            print(out)
            try:
                wifi = pywifi.PyWiFi()
                iface = wifi.interfaces()[0]
                iface.disconnect()
                time.sleep(0.25)
                assert iface.status() in\
                       [const.IFACE_DISCONNECTED, const.IFACE_INACTIVE]
                profile = pywifi.Profile()
                profile.ssid = 'Thunder_Devices'
                profile.auth = const.AUTH_ALG_OPEN
                profile.akm.append(const.AKM_TYPE_WPA2)
                profile.cipher = const.CIPHER_TYPE_CCMP
                profile.key = '12345678'

                wifi = pywifi.PyWiFi()
                iface = wifi.interfaces()[0]
                profile = iface.add_network_profile(profile)
                iface.connect(profile)

                #notify user of breach
                requests.post("https://maker.ifttt.com/trigger/breached/with/key/Qzhe5UWU5AUSnoBumev4U")
                
            finally:
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

    except socket.error as e:
        print("Error: %s" %(e))
    finally:
        wifi = pywifi.PyWiFi()
        iface = wifi.interfaces()[0]
        iface.disconnect()
        time.sleep(0.25)
        assert iface.status() in\
               [const.IFACE_DISCONNECTED, const.IFACE_INACTIVE]
        profile = pywifi.Profile()
        profile.ssid = 'AJ'
        profile.auth = const.AUTH_ALG_OPEN
        profile.akm.append(const.AKM_TYPE_NONE)
        #profile.cipher = const.CIPHER_TYPE_CCMP
        #profile.key = '87654321'
        wifi = pywifi.PyWiFi()
        iface = wifi.interfaces()[0]
        profile = iface.add_network_profile(profile)
        iface.connect(profile)
        soc.close()
