#!/usr/bin/env python
import sys
sys.path.append('..')

import time
import subprocess
import threading
import struct
from socket import *
from ofdm.ofdm_rx import ofdm_rx

from SVCPacket.src.packetization.unpacket import unpacket
from SVCPacket.src.utils import log
class Client:
    def __init__(self):
       
        log.log_start(1) 
        self.unpacket = unpacket(0)
        #self.startSVCPlayer()
        self.okPacketNum = 0
        
        time.sleep(2)
        self.initNetwork()
        
        self.initOfdmRx()

        self.pktNoSet = set([0])
        self.lastOkPktno = 0
        print 'init client ok!'

        self.ofdm_rx.wait() 

        #init log correct receive packet
        self.correctReceivePacketLog = file("correctReceivePacket.log", "w")
        self.log = file('client.log', 'w')
    def startSVCPlayer(self):
        filePath = '../error-conceal/Libs/SVC/bin/svc'
        svcProcess = subprocess.Popen([filePath, '-network', '-layer', '16'])


    def initNetwork(self):
        port = 12345
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))

        self.feedThread = threading.Thread(target = self.feedPlayer)


    def rxCallBack(self, ok, payload):
        (pktno, ) = struct.unpack('!H', payload[0:2]);
        if ok:
            if pktno in self.pktNoSet and pktno < 20:
                return 
            self.okPacketNum += 1
            if (pktno < self.lastOkPktno):
                self.log.write("pkt order is not right after " + str(self.lastOkPktno) + " receive " + str(pktno) + '\n')
                return 
            for i in range(pktno - self.lastOkPktno - 1):
                self.correctReceivePacketLog.write(str(i + self.lastOkPktno + 1) + '\n')
            self.pktNoSet.add(pktno)
            self.unpacket.put_one_rtp(payload[2:])
            self.lastOkPktno = pktno
        
    def initOfdmRx(self):
        self.ofdm_rx = ofdm_rx('2.4G', 128, 80, 32, 32, self.rxCallBack)

    def feedPlayer(self):
        while True:
            nal = self.unpacket.get_one_nal()
            if nal == '':
                time.sleep(0.1)
            elif nal == None:
                self.sock.close()
                break;
            else:
                self.sock.send(struct.pack('!I', len(nal)) + nal)
    def start(self):
        self.ofdm_rx.start()
        self.feedThread.start()
        self.ofdm_rx.wait()
        self.correctReceivePacketLog.close()
        self.log.close()



if __name__ == "__main__":
    client = Client()
    client.start()
