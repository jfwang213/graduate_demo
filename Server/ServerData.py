#!/usr/bin/env python
import sys
sys.path.append('..')
from datetime import datetime

from SVCPacket.src.packetization import packet
from SVCPacket.src.utils import log
from ofdm import ofdm_tx
import struct

from socket import *

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
        self.tx = ofdm_tx.ofdm_tx('2.4G', 128, 80, 32, 64)
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
        self.sock.connect(("127.0.0.1", port))
   
    def RecvFixLen(length):
        res = ''
        while length > 0:
            content = self.sock.recv(length);
            length -= len(content)
            res += content

    def ReceiveCommand(self):
        while True:
            content = RecvFixLen(2)
            (commandLen, commandType) = struct.unpack("!BB", content[0:2])
            if commandType == 1: #start
                content = RecvFixLen(8)
                (midFreq, FreqWidth) = struct.unpack('!ff', content[0:8])
                self.serverDataChannel.startSend()
            elif commandType == 2: #stop
                self.serverDataChannel.stopSend()
            elif commandType == 3: #end
                break

if __name__ == '__main__':
    server = ServerData()
    server.ReceiveCommand()
