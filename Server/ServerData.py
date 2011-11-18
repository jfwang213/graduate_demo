#!/usr/bin/env python
import sys
sys.path.append('..')
from datetime import datetime

from SVCPacket.src.packetization import packet
from ofdm import ofdm_tx
import struct
from Utils.SocketUtils import RecvFixLen
import time, threading
from socket import *

STARTSEND = 1
STOPSEND = 2
ENDSEND = 3
ASSIGNID = 4
QUIT = 5
class ServerDataChannel(object):
    def __init__(self):
        self.fileName = 'svc.file'
        self.stopSend = True
        self.pack = None
        self.tx = None

    def StartSend(self, width, dqID):
        self.stopSend = False
        print "width %f dqID %d" % (width, dqID)
        self.pack = packet.packet(self.fileName, 400, 177, dqID)
        #wait for client
        sleepNum = 0
        while not self.stopSend and sleepNum < 100:
            time.sleep(0.1)
            sleepNum += 1
        dataWidth = 80
        if width < 1.1:
            dataWidth = 64
        self.tx = ofdm_tx.ofdm_tx('2.45G', 128, dataWidth, 32, 64)

        self.SendWarmUp()
        
        one_packet = self.pack.get_one_packet()
        pktno = 0
        while one_packet and not self.stopSend:
            pktno += 1
            if pktno % 100 == 0:
                print self.stopSend
                print pktno, ' ' ,len(one_packet)
            one_packet = struct.pack("!H", pktno) + one_packet
            self.tx.send_pkt(one_packet)
            #if pktno < 20 or pktno % 3 == 0:
            self.tx.send_pkt(one_packet)
            one_packet = self.pack.get_one_packet()

        self.tx.send_pkt(eof=True)
        self.pack.end()
        del self.pack
        self.pack = None
        self.tx.stop()
        self.tx.wait()
        del self.tx
        self.tx = None


    def StopSend(self):
        self.stopSend = True
            
    def SendWarmUp(self):
        warmUpLen = 20
        one_packet = ' '*398
        one_packet = struct.pack("!H", 0) + one_packet

        for i in range(warmUpLen):
            self.tx.send_pkt(one_packet)
            if self.stopSend:
                break

class ServerData(object):
    def __init__(self):
        self.ConnectServer()
        self.serverDataChannel = ServerDataChannel()
        self.sendThread = None

    def ConnectServer(self):
        port = 12346
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.connect(("219.224.167.185", port))
   
    def ReceiveCommand(self):
        print "waiting for receiving commands"
        while True:
            content = RecvFixLen(self.sock, 2)
            (commandLen, commandType) = struct.unpack("!BB", content[0:2])
            if commandType == STARTSEND: #start
                print "receive startsend command"
                content = RecvFixLen(self.sock, 8)
                (midFreq, FreqWidth) = struct.unpack('!ff', content[0:12])
                if FreqWidth < 1.1:
                    dqID = 1
                else:
                    dqID = 16
                self.sendThread = threading.Thread(target=self.serverDataChannel.StartSend, args=[FreqWidth, dqID])
                self.sendThread.start()
            elif commandType == STOPSEND: #stop
                print "receive stopsend command"
                self.StopSend()
            elif commandType == ENDSEND: #end
                print "receive endsend command"
                self.StopSend()
            elif commandType == ASSIGNID:
                IDData = RecvFixLen(self.sock, 4)
                self.clientID = struct.unpack('!I', IDData)[0]
                print "get clientID %d" % self.clientID

    def QuitServer(self):
        print "quit server clientID %d" % (self.clientID)
        if self.sock:
            content = struct.pack("!BBI", 5, QUIT, self.clientID)
            self.sock.send(content)
        self.sock.close()
        self.sock = None
        self.serverDataChannel.StopSend()

    def StopSend(self):
        self.serverDataChannel.StopSend()
        if self.sendThread:
            self.sendThread.join()
            self.sendThread = None

if __name__ == '__main__':
    try:
        server = ServerData()
        server.ReceiveCommand()
    except KeyboardInterrupt:
        server.QuitServer() 
