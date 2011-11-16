import sys
sys.path.append('..')
from gmsk.transceiver import transceiver

import struct
from socket import *
from collections import deque 

class FreqAssign(object):
    def __init__(self, requestID, midFreq, freqWidth):
        self.requestID = requestID
        self.midFreq = midFreq
        self.freqWidth = freqWidth

    def pack(self):
        content = struct.pack("!BIff", 2, self.requestID, self.midFreq, self.freqWidth)
        return content


class ClientAllRequest(object):
"""
One Customer has several request
"""
    def __init__(self, macAddr):
        self.reqs = {} #requests of clients
        self.macAddr = macAddr

    def PutOneReq(self, clientReq):
        if clientReq.requestID in self.reqs:
            print "requestID ", clientReq.requestID, "is already exist. Update It"
        self.reqs[clientReq.requestID] = clientReq

    def GetOneReq(self, requestID):
        if not requestID in self.reqs:
            print "requestID %d is not exist" % requestID
            return None
        return self.reqs[requestID]

class ClientRequest(object):
    def __init__(self, macAddress, requestID, freqWidth):
        self.macAddress = macAddress
        self.requestID = requestID
        self.reqFreqWidth = freqWidth
        self.midFreq = 0
        self.allocFreqWidth = 0

    def PutResult(self, midFreq, freqWidth, dataChannel):
        self.midFreq = midFreq
        self.allocFreqWidth = freqWidth
        self.dataChannel = dataChannel

class ServerControl(object):
    def __init__(self):
        self.macAddress = 1 #server mac address
        self.clients = {}
        self.ratio = 0.5
        
        self.tr = transceiver(self.ReceivePacket)
        self.serverDataChannelFree = deque()
        self.serverDataChannelActive = {} #{mac: socket}
        self.serverSocket = socket(AF_INET, SOCK_STREAM)
        self.serverSocket.bind(("127.0.0.1", 12346)

    def WaitForDataChannel(self):
        while True:
            (channel, addr) = self.serverSocket.accept()
            print "accept one channel"
            self.serverDataChannelFree.append(channel)

    def GetOneClient(self, macAddress):
        if not macAddress in self.clients:
            print "can't not find client with mac address %d" % macAddress
            return None
        return self.clients[macAddress]

    def PutOneClient(self, macAddress, client):
        if macAddress in self.clients:
            print "macAddress %d already exists" % macAddress
        self.clients[macAddress] = client

    def ReceivePacket(self, ok ,packet):
        if not ok:
            print "receive bad packet"
            return 
        (srcMac, dstMac, packetType) = struct.unpack("!IIB", packet[0:9])
        if dstMac != self.macAddress:
            print "receive others' packet address is %d" % dstMac
            return
        if packetType == 1:
            self.DealWithFreqRequest(srcMac, packet[9:])

    def DealWithFreqRelease(self, srcMac, payload):
        (reqID) = struct.unpack("!I", payload[0:4])
        client = self.GetOneClient(srcMac)
        if client != None:
            clientReq = client.GetOneReq(reqID)
            if clientReq != None:
                dataChannel = clientReq.dataChannel
                self.EndDataChannel(dataChannel)
                # free data channel
                self.serverDataChannelFreq.append(dataChannel)

    def DealWithFreqRequest(self, srcMac, payload):
        print "receive freq request packet"
        (reqID, freqWidth) = struct.unpack("!If", payload[0:8])
        client = self.GetOneClient(srcMac)
        if client == None:
            client = ClientAllRequest(srcMac)
            self.PutOneClient(srcMac, client)
        clientReq = client.GetOneReq(reqID)
        if clientReq == None:
            clientReq = ClientRequest(srcMac, reqID, freqWidth)
            if len(self.serverDataChannelFree) == 0:
                raise Exception("no free channel")
            dataChannel = self.serverDataChannelFree.popleft()
            clientReq.PutResult(2450, freqWidth * self.ratio, dataChannel)
            client.PutOneReq(clientReq)

        # if it is not the first request, end the server data channel
        self.EndDataChannel(self.serverDataChannelActive[srcMac])

        freqAssign = FreqAssign(reqID, clientReq.midFreq, clientReq.allocFreqWidth)
        sendContent = struct.pack("!II", self.macAddress, srcMac)
        sendContent += freqAssign.pack()
        self.tr.send_pkt(sendContent)

        # start the data channel
        self.StartDataChannel(self.serverDataChannelActive[srcMac])

    def StartDataChannel(self, channelConn):
        content = struct.pack("!BB", 9, 1) #len commandType:start
        content += struct.pack("!ff", 0, 0) #midFreq freqWidth
        channelConn.send(content)

    def StopDataChannel(self, channelConn):
        content = struct.pack("!BB", 1, 2)
        channelConn.send(content)

    def EndDataChannel(self, channelConn):
        content = struct.pack("!BB", 1, 3)
        channelConn.send(content)

    def RestartDataChannel(self, channelConn):
        content = struct.pack("!BB", 1, 4)
        channelConn.send(content)

    def Start(self):
        print "Start Server"
        self.tr.start()

    def Stop(self):
        print "Stop Server"
        self.tr.send_pkt(eof=False)
        self.tr.stop()
        self.tr.wait()


