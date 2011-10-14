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
class Client:
    def __init__(self):
        
        self.unpacket = unpacket()
        self.startSVCPlayer()
        self.okPacketNum = 0
        
        time.sleep(2)
        self.initNetwork()
        
        self.initOfdmRx()

        self.ofdm_rx.wait() 
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
            self.okPacketNum += 1
            if pktno % 100 == 0:
                print pktno, " is ok! all ok packet number is ", self.okPacketNum
                
            #self.unpacket.put_one_rtp(payload[2:])
        
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
