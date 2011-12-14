#!/usr/bin/env python
import sys
sys.path.append("..")
import time

from SVCPacket.src.packetization import packet, unpacket
from Utils.Log import Log
import subprocess
import threading
import struct
from socket import *

class TestServer:
    def __init__(self, client, dqID):
        self.dqID = dqID
        self.fileName = "../Server/svc.file"
        self.pack = packet.packet(self.fileName, 400, 177, dqID)
        self.client = client
        self.serverLog = Log("serverLog.txt")


    def startSend(self):
        lossFile = file("lost_" + str(self.dqID) + ".txt")
        lineContent = lossFile.readline()
        if lineContent:
            lossPktNo = int(lineContent)
        else:
            lossPktNo = -1
        one_packet = self.pack.get_one_packet()
        curPktNo = 1
        self.bEnd = False
        self.bPause = False
        while one_packet and not self.bEnd:
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
            self.serverLog.LogStr("pktNo %d len %d" % (curPktNo, len(one_packet)))
            curPktNo += 1
            one_packet = self.pack.get_one_packet()
            while self.bPause and not self.bEnd:
                time.sleep(0.1)
            time.sleep(0.01)
    def stopSend(self):
        self.bEnd = True

    def pauseSend(self):
        self.bPause = True

    def resumeSend(self):
        self.bPause = False

class TestClient:
    def __init__(self, dqID):
        
        self.dqID = dqID
        self.unpacket = unpacket.unpacket()
        self.startSVCPlayer()
        self.okPacketNum = 0
        
        time.sleep(2)

        self.pktNoSet = set([0])
        self.lastOkPktno = 0
        print 'init client ok!'
        self.bEnd = False

    def startSVCPlayer(self):
        filePath = '../error-conceal/Libs/SVC/bin/svc'
        FNULL = open('/dev/null', 'w')
        self.svcProcess = subprocess.Popen(args=[filePath, '-network', '-layer', str(self.dqID)], stdout=FNULL)

    def stopSVCPlayer(self):
        if self.svcProcess:
            self.svcProcess.kill()
            self.svcProcess = None

    def initNetwork(self):
        port = 12345
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))


    def receive_one_packet(self,packet):
        self.unpacket.put_one_rtp(packet)

    def feedPlayer(self):
        while not self.bEnd:
            if self.dqID == 16:
                time.sleep(0.01)
            elif self.dqID == 1:
                time.sleep(0.015)
            else:
                time.sleep(0.02)
            nal = self.unpacket.get_one_nal()
            if nal == '':
                time.sleep(0.1)
            elif nal == None:
                self.sock.close()
                break; 
            else:
                self.sock.send(struct.pack('!I', len(nal)) + nal)
                pass
    def start(self):
        self.feedThread = threading.Thread(target = self.feedPlayer)
        self.initNetwork()
        self.feedThread.start()

    def stop(self):
        self.bEnd = True
        self.sock.close()


class FakeClientData(object):
    def __init__(self, dqID):
        self.client = TestClient(dqID)
        self.server = TestServer(self.client, dqID)

    def StartTestClient(self):
        self.client.start()
        self.serverThread = threading.Thread(target = self.server.startSend)
        self.serverThread.start()

    def EndTestClient(self):
        self.client.stop()
        self.client.stopSVCPlayer()
        self.server.stopSend()

    def PauseTestClient(self):
        self.server.pauseSend()

    def ResumeTestClient(self):
        self.server.resumeSend()

if __name__ == '__main__':
    dqID = 16
    client = TestClient(dqID)
    server = TestServer(client, dqID)
    client.start()
    server.startSend()
