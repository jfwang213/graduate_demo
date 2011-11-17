#!/usr/bin/env python
import sys
sys.path.append('..')
from gmsk.transceiver import transceiver

import time
import struct

FreqRequest = 1
FreqAssign = 2
FreqRelease = 3
FreqReleaseACK = 4
class FreqRequest(object):
    def __init__(self, requestID, freqWidth):
        self.requestID = requestID;
        self.freqWidth = freqWidth

    def pack(self):
        content = struct.pack("!BIf", FreqRequest, self.requestID, self.freqWidth)
        return content

class ClientControl(object):
    def __init__(self, macAddress, channelAssignCB, freqReleaseACKCB):
        self.macAddress = macAddress
        self.requestID = 1
        self.channelAssignCB = channelAssignCB
        self.freqReleaseACKCB = freqReleaseACKCB

    def __del__(self):
        print "delete client control class"
        if self.tr:
            del self.tr
        
    def ReceivePacket(self, ok, packet):
        if not ok:
            print "receive bad packet"
            return
        (srcMac, dstMac, packetType) = struct.unpack("!IIB", packet[0:9])
        if dstMac != self.macAddress:
            print "receive others' packet"
            return
        if packetType == FreqAssign:#freqAssign
            reqID = struct.unpack("!I", packet[9:13])
            if reqID == self.requestID:
                self.DealWithFreqAssign(packet[9:])
            else:
                print "wrong request id for freq assign packet"
        elif packetType == FreqReleaseACK:
            reqID = struct.unpack("!I", packet[9:13])
            if reqID == self.requestID:
                self.freqReleaseACKCB(packet[9:])
            else:
                print "wrong request id for release ack packet"
        else:
            print "wrong packetType %d" % packetType

    def DealWithFreqAssign(self, payload):
        (reqID, midFreq, freqWidth) = struct.unpack("!Iff", payload[0:12])
        print "receive freq assign packet reqID %d midFreq %f freqWidth %f" % (reqID, midFreq, freqWidth)
        self.channelAssignCB(1)

    def start(self):
        print "start client control"
        if self.tr:
            self.tr.start()

    def stop(self):
        print "stop client control"
        if self.tr:
            self.tr.send_pkt(eof=True)
            self.tr.stop()
            self.tr.wait()

    def construct(self):
        self.tr = transceiver(self.ReceivePacket)

    def deconstruct(self):
        print "deconstruct client control"
        if self.tr:
            del self.tr
            self.tr = None
        
    def sendFreqReq(self):
        freqReq = FreqRequest(self.requestID, 1)
        dstMac = 1 #server macAddress
        sendContent = struct.pack("!II", self.macAddress, dstMac)
        sendContent += freqReq.pack()

        self.tr.send_pkt(sendContent)

    def sendFreqRelease(self):
        sendContent = struct.pack("!II", self.macAddress, dstMac)
        sendContent += struct.pack("!BI", FreqRelease, self.requestID)

        self.tr.send_pkt(sendContent)
        
    def finishFreqReq(self):
        self.requestID += 1
