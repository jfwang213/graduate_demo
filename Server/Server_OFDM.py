#!/usr/bin/env python
import sys
sys.path.append('..')
from datetime import datetime

from SVCPacket.src.packetization import packet
from ofdm import ofdm_tx
import struct
class ServerData:
    def __init__(self):
        self.fileName = 'svc.file'
        print datetime.now()
        print self.fileName
        self.pack = packet.packet(self.fileName, 400, 177)
        self.tx = ofdm_tx.ofdm_tx('2.4G', 128, 80, 32, 64)

    def startSend(self):
        one_packet = self.pack.get_one_packet()
        pktno = 0
        while one_packet:
            pktno += 1
            if pktno % 100 == 0:
                print pktno, ' ' ,len(one_packet)
            one_packet = struct.pack("!H", pktno) + one_packet
            self.tx.send_pkt(one_packet)
            if pktno < 20:
                self.tx.send_pkt(one_packet)
            one_packet = self.pack.get_one_packet()

        self.tx.send_pkt(eof=True)
        print datetime.now()
    
    def sendWarmUp(self):
        warmUpLen = 20
        one_packet = ' '*398
        one_packet = struct.pack("!H", 0) + one_packet

        for i in range(warmUpLen):
            self.tx.send_pkt(one_packet)



if __name__ == '__main__':
    server = ServerData()
    server.sendWarmUp()
    server.startSend()
