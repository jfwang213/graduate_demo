#!/usr/bin/env python
from ClientData import ClientData
from ClientControl import ClientControl

class Client(object):
    def __init__(self):
        self.dataChannel = None
        self.ctlChannel = None
        pass
    def startDataChannel(self):
        if self.dataChannel:
            self.endDataChannel()
        if self.ctlChannel:
            self.endCtlChannel()

        print "start data channel"
        self.dataChannel = ClientData()
        self.dataChannel.start()

    def endDataChannel(self):
        if self.dataChannel:
            print "stop data channel"
            self.dataChannel.stop()
            self.dataChannel.deconstruction()
            del self.dataChannel
            self.dataChannel = None

    def receiveDataChannelAssign(self, width): #1 is big 2 is small
        if width == 1:
            self.startDataChannel()
        else:
            self.startDataChannel()
    def startCtlChannel(self):
        if self.dataChannel:
            self.endDataChannel()
        if self.ctlChannel:
            self.endCtlChannel()
        print "start ctl channel"
        self.ctlChannel = ClientControl(2, self.receiveDataChannelAssign)
        self.ctlChannel.start()

    def sendCtlChannelRequest(self):
        if self.ctlChannel:
            print "send control channel request"
            self.ctlChannel.sendFreqReq()

    def endCtlChannel(self):
        if self.ctlChannel:
            print "end ctl channel"
            self.ctlChannel.stop()
            self.ctlChannel.deconstruct()
            del self.ctlChannel
            self.ctlChannel = None

    def endAll(self):
        if self.ctlChannel:
            self.endCtlChannel()
        if self.dataChannel:
            self.endDataChannel()


if __name__ == "__main__":
    client = Client()
    
    try:
        while True:
            command = raw_input("input the command:\n")
            if command == "stopdata":
                client.endDataChannel()
            elif command == "startctl":
                client.startCtlChannel()
            elif command == "startdata":
                client.startDataChannel()
            elif command == "stopctl":
                client.endCtlChannel()
            elif command == "end":
                break
            elif command == "sendreq":
                client.sendCtlChannelRequest()

    except KeyboardInterrupt:
        pass
    client.endAll()
