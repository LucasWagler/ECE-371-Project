#Alex Jasper
#10/26/19
#This program connects to the IFTTT web hook to call a specified phone number.
#It is intended to run during a security breach.
import requests

def notify():
    requests.post("https://maker.ifttt.com/trigger/breached/with/key/Qzhe5UWU5AUSnoBumev4U")

notify()