#!/usr/bin/env python
import sys
sys.path.append("..")
import time

from SVCPacket.src.packetization import packet, unpacket
from SVCPacket.src.utils import log

import subprocess
import threading
import struct
from socket import *

class TestServer:
    def __init__(self, client):
        self.fileName = "../Server/svc.file"
        log.log_start(1)
        self.pack = packet.packet(self.fileName, 400, 177, 0)
        self.client = client

    def startSend(self):
        lossFile = file("lostPacketNo.txt")
        lineContent = lossFile.readline()
        if lineContent:
            lossPktNo = int(lineContent)
        else:
            lossPktNo = -1
        one_packet = self.pack.get_one_packet()
        curPktNo = 1
        while one_packet:
            if curPktNo == lossPktNo:
                #remove it
                print "remove pktno ", curPktNo
                lineContent = lossFile.readline()
                if lineContent:
                    lossPktNo = int(lineContent)
                else:
                    lossPktNo = -1
            else:
                #deliver it to client
                self.client.receive_one_packet(one_packet)
            curPktNo += 1
            one_packet = self.pack.get_one_packet()
            time.sleep(0.1)


class TestClient:
    def __init__(self):
       
        log.log_start(1) 
        self.unpacket = unpacket.unpacket(0)
        #self.startSVCPlayer()
        self.okPacketNum = 0
        
        time.sleep(2)

        self.pktNoSet = set([0])
        self.lastOkPktno = 0
        print 'init client ok!'

    def startSVCPlayer(self):
        filePath = '../error-conceal/Libs/SVC/bin/svc'
        svcProcess = subprocess.Popen([filePath, '-network', '-layer', '16'])


    def initNetwork(self):
        port = 12345
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))

        self.feedThread = threading.Thread(target = self.feedPlayer)

    def receive_one_packet(self,packet):
        self.unpacket.put_one_rtp(packet)

    def feedPlayer(self):
        while True:
            nal = self.unpacket.get_one_nal()
            if nal == None:
                time.sleep(0.1)
            else:
                self.sock.send(struct.pack('!I', len(nal)) + nal)
    def start(self):
        self.initNetwork()
        self.feedThread.start()

if __name__ == '__main__':
    client = TestClient()
    server = TestServer(client)
    client.start()
    server.startSend()
