#Alex Jasper
#10/26/19
#This program logs a breach of security with a timestamp and message.

import datetime
import logging

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