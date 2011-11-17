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
class ClientData(object):

    def __init__(self):
        log.log_start(1) 
        self.unpacket = unpacket(0)
        self.svcProcess = None
        #self.startSVCPlayer()
        self.okPacketNum = 0
        
        time.sleep(2)
        self.initNetwork()
        
        self.initOfdmRx()

        self.pktNoSet = set([0])
        self.lastOkPktno = 0
        self.stopFeed = False
        print 'init client ok!'

        #init log correct receive packet
        self.correctReceivePacketLog = file("correctReceivePacket.log", "w")
        self.log = file('client.log', 'w')
    def startSVCPlayer(self):
        filePath = '../error-conceal/Libs/SVC/bin/svc'
        self.svcProcess = subprocess.Popen([filePath, '-network', '-layer', '16'])
    
    def stopSVCPlayer(self):
        if self.svcProcess:
            self.svcProcess.kill()
            self.svcProcess = None

    def initNetwork(self):
        port = 12345
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))

        self.feedThread = threading.Thread(target = self.feedPlayer)


    def rxCallBack(self, ok, payload):
        (pktno, ) = struct.unpack('!H', payload[0:2])
        if ok:
            if pktno in self.pktNoSet:
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
        self.ofdm_rx = ofdm_rx('2.45G', 128, 80, 32, 32, self.rxCallBack)

    def feedPlayer(self):
        while not self.stopFeed:
            nal = self.unpacket.get_one_nal()
            if nal == '':
                print "get one blank"
                time.sleep(0.1)
            elif nal == None:
                break;
            else:
                self.sock.send(struct.pack('!I', len(nal)) + nal)
        self.sock.send(struct.pack('!I', 0))
        self.sock.close()
        print "end of feed player"

    def start(self):
        self.ofdm_rx.start()
        self.feedThread.start()
        
    def stop(self):
        #stop ofdm rx
        self.ofdm_rx.stop()
        self.ofdm_rx.wait()
        #stop feed and player
        self.stopFeed = True
        self.unpacket.rtp_end = True
        self.stopSVCPlayer()
        self.correctReceivePacketLog.close()
        self.log.close()

    def deconstruction(self):
        print "deconstruction of ClientData"
        del self.ofdm_rx

