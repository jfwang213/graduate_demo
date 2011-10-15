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
        self.startSVCPlayer()
        self.okPacketNum = 0
        
        time.sleep(2)
        self.initNetwork()
        
        self.initOfdmRx()

        self.ofdm_rx.wait() 

        self.pktNoSet = set([0])
        print 'init client ok!'
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
            self.pktNoSet.add(pktno)
            if pktno < 20 or pktno % 100 == 0:
                print pktno, " is ok! all ok packet number is ", self.okPacketNum, "length is ", len(payload) - 2
                
            self.unpacket.put_one_rtp(payload[2:])
        
    def initOfdmRx(self):
        self.ofdm_rx = ofdm_rx('2.4G', 128, 80, 32, 32, self.rxCallBack)

    def feedPlayer(self):
        while True:
            nal = self.unpacket.get_one_nal()
            if nal == None:
                time.sleep(0.1)
            else:
                self.sock.send(struct.pack('!I', len(nal)) + nal)
    def start(self):
        self.ofdm_rx.start()
        self.feedThread.start()
        self.ofdm_rx.wait()



if __name__ == "__main__":
    client = Client()
    client.start()
