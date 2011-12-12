#!/usr/bin/env python
import sys
sys.path.append('..')
from ServerControl import ServerControl
from socket import *

from Utils.SocketUtils import RecvFixLen

import struct, threading, select
from ClientGenerate import Client
from EstServer import EstServer

"""
the Command between server and server web ui
"""
GetFreeDataChannel = 1
GetAllActiveClient = 2
GetOneActiveClient = 3
GetEstParam = 4
SetClientParam = 5

GiveAllActiveClient = 101
GiveOneActiveClient = 102
GiveFreeDataChannel = 103
GiveEstParam = 104


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
    
    def StartClientGenerate(self):
        self.clientGenerate = Client() 
        self.clientThread = threading.Thread(target = self.clientGenerate.work, args=())
        self.clientThread.start()
        
    def StartEstServer(self):
        self.estServer = EstServer() 
        self.estServerThread = threading.Thread(target = self.estServer.listen, args=())
        self.estServerThread.start()

    def StopEstServer(self):
        self.estServer.stop()
        self.estServerThread.join()


    def StopClientGenerate(self):
        self.clientGenerate.stop()
        self.clientThread.join()

    def SetClientType(self, typeID):
        if typeID == 1:
            self.clientGenerate.SetParam(3, 7.2)
        elif typeID == 2:
            self.clientGenerate.SetParam(3.3, 7.2)
        else:
            self.clientGenerate.SetParam(3.6, 9.6)

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
            elif command == GetEstParam:
                (averageServiceTime, inputrate) = (self.estServer.est_serviceTime, 1/self.est_lambda)
                content = struct.pack("!Bff", GiveEstParam, averageServiceTime, inputrate)
            elif command == SetClientParam:
                content = RecvFixLen(uiSocket, 1)
                typeID = struct.unpack("!B", content)
                self.setClientType(typeID)
                content == ''
            elif command == End:
                break
            else:
                content = struct.pack("!B", Error)
            if len(content) > 0:
                uiSocket.send(content)


if __name__ == '__main__':
    try:
        server = ServerBackend()
        server.AcceptUISocket()
    except KeyboardInterrupt:
        print "stop server"
        server.Stop()
