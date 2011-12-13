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
GetServerInfoCom = 6
GetClientGenerateInfoCom = 7

GiveAllActiveClient = 101
GiveOneActiveClient = 102
GiveFreeDataChannel = 103
GiveEstParam = 104
GiveServerInfoCom = 105
GiveClientGenerateInfoCom = 106

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
        self.typeID = -1

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
        self.StopClientGenerate()
        self.StopEstServer()
    
    def StartClientGenerate(self):
        print 'start client generate'
        self.clientGenerate = Client() 
        self.clientThread = threading.Thread(target = self.clientGenerate.work, args=())
        self.clientThread.start()

        self.SetClientType(1)
        
    def StartEstServer(self):
        print 'start est server'
        self.estServer = EstServer() 
        self.estServerThread = threading.Thread(target = self.estServer.listen, args=())
        self.estServerThread.start()

    def StopEstServer(self):
        print 'stop est server'
        self.estServer.stop()
        self.estServerThread.join()


    def StopClientGenerate(self):
        print 'stop client generate'
        self.clientGenerate.stop()
        self.clientThread.join()

    def SetClientType(self, typeID):
        if self.typeID == typeID:
            return
        self.typeID = typeID
        self.ctlChannel.typeID = typeID
        if typeID == 1:
            self.clientGenerate.SetParam(3, 62)
        elif typeID == 2:
            self.clientGenerate.SetParam(3.3, 72)
        else:
            self.clientGenerate.SetParam(3.6, 95)

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
                print clients
                reqNumber = 0
                content = ''
                for clientMac, client in clients.items():
                    for reqID, req in client.reqs.items():
                        reqNumber += 1
                        content += struct.pack("!BIf", clientMac, reqID, req.allocFreqWidth)
                content = struct.pack("!BI", GiveAllActiveClient, reqNumber) + content
            elif command == GetOneActiveClient:
                print "Get One Active Client"
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
            elif command == GetClientGenerateInfoCom:
                print 'get client generate info'
                (averageSerTime, inputrate) = (1.0/self.clientGenerate.serviceTimeLamb, self.clientGenerate.inputTimeLamb)
                content = struct.pack("!Bff", GiveClientGenerateInfoCom, averageSerTime, inputrate)
            elif command == GetEstParam:
                print "Get est param"
                (averageServiceTime, inputrate) = (self.estServer.est_serviceTime, self.estServer.est_lambda)
                content = struct.pack("!Bff", GiveEstParam, averageServiceTime, inputrate)

            elif command == SetClientParam:
                print "Set client param"
                content = RecvFixLen(uiSocket, 1)
                typeID = struct.unpack("!B", content)[0]
                self.SetClientType(typeID)
                content = ''

            elif command == GetServerInfoCom:
                print "get server info"
                (maxFreq, freeFreq, nowSatis, activeNum, servedNum) = self.estServer.GetServerInfo()

                content = struct.pack("!BfffIII", GiveServerInfoCom, maxFreq, freeFreq, nowSatis, activeNum, servedNum, self.typeID)

            elif command == End:
                break
            else:
                content = struct.pack("!B", Error)
            if len(content) > 0:
                uiSocket.send(content)


if __name__ == '__main__':
    try:
        server = ServerBackend()
        server.StartEstServer()
        server.StartClientGenerate()
        server.AcceptUISocket()
    except KeyboardInterrupt:
        pass
    print "stop server"
    server.Stop()
