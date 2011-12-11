#!/usr/bin/env python
import sys
sys.path.append('..')
from ServerControl import ServerControl
from socket import *

from Utils.SocketUtils import RecvFixLen

import struct, threading, select


"""
the Command between server and server web ui
"""
GetFreeDataChannel = 1
GetAllActiveClient = 2
GetOneActiveClient = 3

GiveAllActiveClient = 101
GiveOneActiveClient = 102
GiveFreeDataChannel = 103

End = 254
Error = 255

class ServerBackend(object):
    def __init__(self):
        self.uiSocketServer = socket(AF_INET, SOCK_STREAM)
        self.uiSocketServer.bind(('0.0.0.0', 12347))
        self.uiSocketServer.listen(5)
        self.ctlChannel = ServerControl()
        self.ctlChannel.Start()
        self.stop = False

    def AcceptUISocket(self):
        try:
            while not self.stop:
                rr, rw, err = select.select([self.uiSocketServer], [], [], 1)
                if rr:
                    (uiSocket, addr) = self.uiSocketServer.accept()
                    print "receive one ui ", addr
                    oneThread = threading.Thread(target=self.DealWithUICommand, args=[uiSocket])
                    oneThread.start()
        except KeyboardInterrupt:
            pass 
        self.uiSocketServer.close()
        self.ctlChannel.Stop()

    def Stop(self):
        self.stop = True

    def DealWithUICommand(self, uiSocket):
        while True:
            command = RecvFixLen(uiSocket, 1)
            command = struct.unpack("!B", command)[0]
            if command == GetFreeDataChannel:
                print "get free data channel"
                number = self.ctlChannel.GetFreeDataChannelNumber()
                content = struct.pack("!BB", GiveFreeDataChannel, number)
            elif command == GetAllActiveClient:
                print " get all active client"
                clients = self.ctlChannel.clients
                reqNumber = 0
                content = ''
                for clientMac, client in clients:
                    for reqID, req in client.reqs:
                        reqNumber += 1
                        content += struct.pack("!BI", clientMac, reqID)
                content = struct.pack("!BI", GiveAllActiveClient, reqNumber) + content
            elif command == GetOneActiveClient:
                content = RecvFixLen(uiSocket, 5)
                (mac, reqID) = struct.unpack("!BI", content)
                client = self.ctlChannel.GetOneClient(mac)
                if client != None:
                    req = client.GetOneReq(reqID)
                    if req != None:
                        content = struct.pack("!Bff", mac, req.midFreq, allocFreqWidth)
                    else:
                        content = struct.pack("!B", Error)
                else:
                    content = struct.pack("!B", Error)
            elif command == End:
                break
            else:
                content = struct.pack("!B", Error)
            uiSocket.send(content)


if __name__ == '__main__':
    try:
        server = ServerBackend()
        server.AcceptUISocket()
    except KeyboardInterrupt:
        print "stop server"
        server.Stop()
