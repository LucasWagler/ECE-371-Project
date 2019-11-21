#Alex Jasper
#10/26/19
#This program connects to the IFTTT web hook to call a specified phone number.
#It is intended to run during a security breach.

#In powershell run the following commands:
#   python -m pip install --upgrade pip
#   python -m pip install requests
#   python -m pip install --upgrade ndg-httpsclient

import requests

requests.post("https://maker.ifttt.com/trigger/breached/with/key/Qzhe5UWU5AUSnoBumev4U")
