#!/Python/python 
import cgitb
cgitb.enable(1,'log')

f = open('log.txt', 'a+')
f.write('Test.\r\n')
f.close()
