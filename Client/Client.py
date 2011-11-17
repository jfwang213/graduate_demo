#!/usr/bin/env python
import sys
sys.path.append('..')

from ClientData import ClientData
from ClientControl import ClientControl
import threading
import time
from threading import Timer
from Utils.Log import Log

IDLE = 0
WAITASSIGN = 1
WAITDATA = 2
STARTDATA = 3
ENDDATA = 4
TIMERDURATION = 2 #sec

class Client(object):
    def __init__(self):
        self.dataChannel = None
        self.ctlChannel = None
        self.stateLock = threading.Lock()
        self.state = IDLE
        self.log = Log('ClientLog.txt')
        pass

    def startDataChannel(self):
        if self.dataChannel:
            self.endDataChannel()
        if self.ctlChannel:
            self.endCtlChannel()

        self.log.LogStr("start data channel")

        self.dataChannel = ClientData()
        self.dataChannel.start()

    def endDataChannel(self):
        if self.dataChannel:
            self.log.LogStr("stop data channel")
            self.dataChannel.stop()
            self.dataChannel.deconstruction()
            del self.dataChannel
            self.dataChannel = None
        else:
            self.log.LogStr("endDataChannel but dataChannel is none")

    def freqAssignThreadCB(self):
        self.log.LogStr("freqAssignThread call callback")
        self.stateLock.acquire()
        self.log.LogStr("the state is %d" % (self.state))
        if self.state == WAITDATA:
            self.endCtlChannel()
            self.startDataChannel()
            self.state = STARTDATA
        self.stateLock.release()

    def userInteractThreadCB(self):
        while True:
            command = raw_input("command:")
            if command == 'start':
                self.stateLock.acquire()
                if self.state == IDLE:
                    self.startCtlChannel()
                    self.sendCtlChannelRequest()
                    self.state = WAITASSIGN
                self.stateLock.release()
            elif command == 'end':
                self.stateLock.acquire()
                if self.state == WAITASSIGN:
                    self.endCtlChannel()
                    self.ctlChannel.finishFreqReq()
                elif self.state == WAITDATA:
                    self.endCtlChannel()
                    self.ctlChannel.finishFreqReq()
                elif self.state == STARTDATA:
                    self.endDataChannel()
                    self.ctlChannel.finishFreqReq()
                self.state = IDLE
                self.stateLock.release()
            elif command == 'quit':
                self.endAll()
                break
                
    def freqReqTimeoutCB(self):
        self.stateLock.acquire()
        if self.state == WAITASSIGN:
            self.sendCtlChannelRequest()
            self.freqReqTimer = Timer(TIMERDURATION, self.freqReqTimeoutCB)
        self.stateLock.release()

    def receiveDataChannelAssign(self, width): #1 is big 2 is small
        if width == 1:
            self.startDataChannel()
        else:
            self.startDataChannel()

    def startCtlChannel(self):
        if self.dataChannel:
            self.endDataChannel()
        if not self.ctlChannel:
            self.ctlChannel = ClientControl(2, self.receiveDataChannelAssign)

        print "start ctl channel"
        self.ctlChannel.construct()
        self.ctlChannel.start()

    def sendCtlChannelRequest(self):
        if self.ctlChannel:
            print "send control channel request"
            self.ctlChannel.sendFreqReq()

    def endCtlChannel(self):
        print "end ctl channel"
        self.ctlChannel.stop()
        self.ctlChannel.deconstruct()

    def endAll(self):
        if self.ctlChannel:
            self.endCtlChannel()
            self.ctlChannel = None
        if self.dataChannel:
            self.endDataChannel()

    def reqTimeoutCB(self):
        self.timeoutNumber += 1
        if self.state != 1:
            return
        if self.timeoutNumber > 10:
            self.state = 3
        else:
            self.state = 0
        self.dealWithStateMachine()

    def work(self):
        self.startCtlChannel()
        time.sleep(1)
        self.endCtlChannel()
        self.startDataChannel()


if __name__ == "__main__":
    client = Client()
    
    try:
        client.userInteractThreadCB() 
    except KeyboardInterrupt:
        pass
    client.endAll()
