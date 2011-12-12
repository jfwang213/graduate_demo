

from socket import *


def recvFixedLen(tcpSocket,length):
	data  = ''
	tLen = length
	while tLen > 0:
		tempData = tcpSocket.recv(tLen)
		if len(tempData) == 0:
			print 'receive error'
			return ''
		data = data + tempData
		tLen = tLen - len(tempData)
	return data