#!/usr/bin/env python
import sys
sys.path.append('..')
from datetime import datetime

from SVCPacket.src.packetization import packet
from SVCPacket.src.utils import log
from ofdm import ofdm_tx
import struct
from Utils.SocketUtils import RecvFixLen

from socket import *

STARTSEND = 1
STOPSEND = 2
ENDSEND = 3
ASSIGNID = 4
QUIT = 5
class ServerDataChannel:
    def __init__(self):
        self.fileName = 'svc.file'
        log.log_start(1)
        self.stopSend = True
        self.pack = None
        self.tx = None

    def startSend(self):
        self.stopSend = False
        self.sendWarmUp()
        self.pack = packet.packet(self.fileName, 400, 177, 0)
        self.tx = ofdm_tx.ofdm_tx('2.45G', 128, 80, 32, 64)
        one_packet = self.pack.get_one_packet()
        pktno = 0
        while one_packet and not self.stopSend:
            pktno += 1
            if pktno % 100 == 0:
                print pktno, ' ' ,len(one_packet)
            one_packet = struct.pack("!H", pktno) + one_packet
            self.tx.send_pkt(one_packet)
            if pktno < 20:
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


    def stopSend(self):
        self.stopSend = True
            
    def sendWarmUp(self):
        warmUpLen = 20
        one_packet = ' '*398
        one_packet = struct.pack("!H", 0) + one_packet

        for i in range(warmUpLen):
            self.tx.send_pkt(one_packet)

class ServerData(object):
    def __init__(self):
        self.ConnectServer()
        self.serverDataChannel = ServerDataChannel()

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
                content = RecvFixLen(self.sock, 8)
                (midFreq, FreqWidth) = struct.unpack('!ff', content[0:8])
                self.serverDataChannel.startSend()
            elif commandType == STOPSEND: #stop
                self.serverDataChannel.stopSend()
            elif commandType == ENDSEND: #end
                self.serverDataChannel.stopSend()
            elif commandType == ASSIGNID:
                IDData = RecvFixLen(self.sock, 4)
                self.clientID = struct.unpack('!I', IDData)[0]

    def QuitServer(self):
        print "quit server"
        if self.sock:
            content = struct.pack("!BBI", 5, QUIT, self.clientID)
            self.sock.send(content)
        self.sock.close()
        self.sock = None
            

if __name__ == '__main__':
    try:
        server = ServerData()
        server.ReceiveCommand()
    except KeyboardInterrupt:
        server.QuitServer() 
