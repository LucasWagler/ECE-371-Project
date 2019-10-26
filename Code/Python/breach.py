#Alex Jasper
#10/26/19
#This program logs a breach of security with a timestamp and message
# and connects to the IFTTT web hook to call a specified phone number.
#It is intended to run during a security breach.

import datetime
import logging
import requests

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

#notify user of breach
requests.post("https://maker.ifttt.com/trigger/breached/with/key/Qzhe5UWU5AUSnoBumev4U")
