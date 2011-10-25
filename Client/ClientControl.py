#!/usr/bin/env python
import sys
sys.path.append('..')
from gmsk.transceiver import transceiver

import time
import struct

class FreqRequest(object):
    def __init__(self, requestID, freqWidth):
        self.requestID = requestID;
        self.freqWidth = freqWidth

    def pack(self):
        content = struct.pack("!BIf", 1, self.requestID, self.freqWidth)
        return content

class ClientControl(object):
    def __init__(self, macAddress):
        self.macAddress = macAddress
        self.tr = transceiver(self.ReceivePacket)
        self.stopFlag = False
        self.requestID = 1

    def ReceivePacket(self, ok, packet):
        if not ok:
            print "receive bad packet"
            return
        (srcMac, dstMac, packetType) = struct.unpack("!IIB", packet[0:9])
        if dstMac != self.macAddress:
            print "receive others' packet"
            return
        if packetType == 2:#freqAssign
            self.DealWithFreqAssign(packet[9:])
        else:
            print "wrong packetType %d" % packetType

    def DealWithFreqAssign(self, payload):
        (reqID, midFreq, freqWidth) = struct.unpack("!Iff", packet[0:12])
        print "receive freq assign packet reqID %d midFreq %f freqWidth %f" % (reqID, midFreq, freqWidth)

    def start(self):
        print "start client"
        self.tr.start()

    def stop(self):
        print "stop client"
        self.tr.send_pkt(eof=True)
        self.tr.stop()
        self.tr.wait()
        
    def sendFreqReq(self):
        freqReq = FreqRequest(self.requestID, 1)
        dstMac = 1 #server macAddress
        sendContent = struct.pack("!II", self.macAddress, dstMac)
        sendContent += freqReq.pack()

        self.tr.send_pkt(sendContent)