import sys
sys.path.append('..')
from gmsk.transceiver import transceiver

import struct
from socket import *
from collections import deque 
import threading, select
from Utils.SocketUtils import RecvFixLen


"""
    the command between server control and server data client.
"""
STARTSEND = 1
STOPSEND = 2
ENDSEND = 3
ASSIGNID = 4
QUIT = 5

"""
    the command between server control and user client.
"""
FreqReqCom = 1
FreqAssignCom = 2
FreqReleaseCom = 3
FreqReleaseACKCom = 4

class FreqAssign(object):
    def __init__(self, reqID, midFreq, freqWidth):
        self.reqID = reqID
        self.midFreq = midFreq
        self.freqWidth = freqWidth

    def pack(self):
        content = struct.pack("!BIff", FreqAssignCom, self.reqID, self.midFreq, self.freqWidth)
        return content


class ClientAllRequest(object):
    """
    One Customer has several request
    """
    def __init__(self, macAddr):
        self.reqs = {} #requests of clients
        self.macAddr = macAddr

    def PutOneReq(self, clientReq):
        if clientReq.reqID in self.reqs:
            print "requestID ", clientReq.reqID, "is already exist. Update It"
        self.reqs[clientReq.reqID] = clientReq

    def GetOneReq(self, reqID):
        if not reqID in self.reqs:
            print "requestID %d is not exist" % reqID
            return None
        return self.reqs[reqID]

    def RemoveOneReq(self, reqID):
        if reqID in self.reqs:
            del self.reqs[reqID]

class ClientRequest(object):
    def __init__(self, macAddress, reqID, freqWidth):
        self.macAddress = macAddress
        self.reqID = reqID
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
        #self.ratio = 1
        self.ratio = 0.5 
        self.tr = transceiver(self.ReceivePacket)
        self.serverDataChannelFree = []
        self.serverDataChannelActive = {} #{mac: socket}
        self.serverSocket = socket(AF_INET, SOCK_STREAM)
        self.serverSocket.bind(("0.0.0.0", 12346))
        self.serverSocket.listen(5)
        self.notQuit = True

        self.nextClientID = 0
        
        self.channelRecvThread = threading.Thread(target=self.RecvDataChannelThreadCB)
        self.channelRecvThread.start()

    def RecvDataChannelThreadCB(self):
        while self.notQuit:
            rlist, wlist, xlist = select.select(self.serverDataChannelFree, [], [], 1)
            if rlist:
                for soc in rlist:
                    self.DealWithDataChannelCommand(soc)

    def DealWithDataChannelCommand(self, clientSock):
        data = RecvFixLen(clientSock, 2)
        command = struct.unpack("!BB", data)[1]
        if command == QUIT:
            clientID = RecvFixLen(clientSock, 4)
            clientID = struct.unpack('!I', clientID)[0]
            self.serverDataChannelFree.remove(clientSock)
            print "client %d quit free num is %d" %(clientID, len(self.serverDataChannelFree))

    def AddOneDataChannel(self, channelSock):
        content = struct.pack("!BBI", 5, ASSIGNID, self.nextClientID)
        self.nextClientID += 1
        channelSock.send(content)
        self.serverDataChannelFree.append(channelSock)
        print "add one client free nuber is %d" % (len(self.serverDataChannelFree))

    def WaitForDataChannel(self):
        while self.notQuit:
            rr,rw,err = select.select([self.serverSocket],[],[], 1)
            if rr:
                (channel, addr) = self.serverSocket.accept()
                print "accept one channel", addr
                self.AddOneDataChannel(channel)

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
        if packetType == FreqReqCom:
            self.DealWithFreqRequest(srcMac, packet[9:])
        elif packetType == FreqReleaseCom:
            self.DealWithFreqRelease(srcMac, packet[9:])

    def DealWithFreqRelease(self, srcMac, payload):
        (reqID) = struct.unpack("!I", payload[0:4])[0]
        client = self.GetOneClient(srcMac)
        if client != None:
            clientReq = client.GetOneReq(reqID)
            if clientReq != None:
                dataChannel = clientReq.dataChannel
                self.EndDataChannel(dataChannel)
                # free data channel
                self.serverDataChannelFree.append(dataChannel)
                client.RemoveOneReq(reqID)
        # if the release has been received before, still send the release ack 
        sendContent = struct.pack("!IIBI", self.macAddress, srcMac, FreqReleaseACKCom, reqID)
        self.tr.send_pkt(sendContent)

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
            dataChannel = self.serverDataChannelFree.pop()
            if self.ratio > 0.7:
                freqWidth = 1.3
            else:
                freqWidth = 1
            clientReq.PutResult(2450, freqWidth, dataChannel)
            client.PutOneReq(clientReq)

        # if it is not the first request, end the server data channel
        self.EndDataChannel(clientReq.dataChannel)

        freqAssign = FreqAssign(reqID, clientReq.midFreq, clientReq.allocFreqWidth)
        sendContent = struct.pack("!II", self.macAddress, srcMac)
        sendContent += freqAssign.pack()
        self.tr.send_pkt(sendContent)

        # start the data channel
        self.StartDataChannel(clientReq.dataChannel, clientReq.allocFreqWidth)

    def StartDataChannel(self, channelConn, freqWidth):
        print "start data channel"
        content = struct.pack("!BB", 9, STARTSEND) #len commandType:start
        content += struct.pack("!ff", 2.45, freqWidth) #midFreq freqWidth
        channelConn.send(content)
        
    def StopDataChannel(self, channelConn):
        content = struct.pack("!BB", 1, STOPSEND)
        channelConn.send(content)

    def EndDataChannel(self, channelConn):
        print "end data channel"
        content = struct.pack("!BB", 1, ENDSEND)
        channelConn.send(content)

    def Start(self):
        print "Start Server"
        self.tr.start()
        self.listenDataChannelThread = threading.Thread(target=self.WaitForDataChannel, args = ())
        self.listenDataChannelThread.start()

    def Stop(self):
        print "Stop Server"
        self.tr.send_pkt(eof=True)
        self.tr.stop()
        self.tr.wait()
        self.notQuit = False
        self.listenDataChannelThread.join()

