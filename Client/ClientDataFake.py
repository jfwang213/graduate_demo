#!/usr/bin/env python
import sys
sys.path.append('..')

import time
import subprocess
import threading
import struct
from socket import *
from ofdm.ofdm_rx import ofdm_rx

from threading import Timer
from TestClient import FakeClientData

IDLE = 1
SENDDATA = 2
PAUSE = 3
END = 4
TimerDuration = 2


NoCheck = 0

class ClientDataFake(object):

    def __init__(self, dataWidth, dqID):
        self.dqID = dqID
        
        if not NoCheck:
            self.initOfdmRx(dataWidth)

        self.lastOkPktno = 0

        self.fakeClientData = FakeClientData(dqID)

        print 'init client ok!'

        #init log correct receive packet
        #self.correctReceivePacketLog = file("correctReceivePacket.log", "w")
        self.log = file('client.log', 'w')
       
        self.state = IDLE
        self.stateLock = threading.Lock()


    def TimerCB(self):
        self.stateLock.acquire()
        print "check timer callback"
        if self.state == SENDDATA:
            nowTime = time.clock()
            if self.lastRecvTime < nowTime - 5:
                self.state = PAUSE
                print "change to PAUSE state"
                self.fakeClientData.PauseTestClient()
        if self.state != END:
            self.checkTimer = Timer(TimerDuration, self.TimerCB)
            self.checkTimer.start()
        else:
            self.checkTimer = None
        self.stateLock.release()
        
    def rxCallBack(self, ok, payload):
        (pktno, ) = struct.unpack('!H', payload[0:2])
        if not NoCheck:
            self.stateLock.acquire()
            self.lastRecvTime = time.clock()
            #print "receive one packet %f" % self.lastRecvTime
            if self.state == IDLE:
                self.state = SENDDATA
                self.checkTimer = Timer(TimerDuration, self.TimerCB)
                self.checkTimer.start()
                self.fakeClientData.StartTestClient()
            elif self.state == PAUSE:
                self.state = SENDDATA
                self.fakeClientData.ResumeTestClient()
            self.stateLock.release()

        if ok:
            if (pktno < self.lastOkPktno):
                self.log.write("pkt order is not right after " + str(self.lastOkPktno) + " receive " + str(pktno) + '\n')
                return 
            for i in range(pktno - self.lastOkPktno - 1):
                pass
                #self.correctReceivePacketLog.write(str(i + self.lastOkPktno + 1) + '\n')

                
        
    def initOfdmRx(self, dataWidth):
        print dataWidth
        self.ofdm_rx = ofdm_rx('2.5G', 128, dataWidth, 32, 32, self.rxCallBack)

    def start(self):
        if not NoCheck:
            self.ofdm_rx.start()
        if NoCheck:
            time.sleep(7)
            self.fakeClientData.StartTestClient()

        
    def stop(self):
        #stop ofdm rx
        if not NoCheck:
            self.ofdm_rx.stop()
            self.ofdm_rx.wait()

        self.log.close()
        self.stateLock.acquire()
        if self.state != IDLE or NoCheck:
            self.fakeClientData.EndTestClient()
        self.state = END
        self.stateLock.release()
        time.sleep(2)
        #self.correctReceivePacketLog.close()

    def deconstruction(self):
        print "deconstruction of ClientData"
        if not NoCheck:
            del self.ofdm_rx

