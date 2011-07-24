#!/usr/bin/env python
import sys
sys.path.append('..')

from SVCPacket.src.packetization import packet
from ofdm import ofdm_tx
class ServerData:
    def __init__(self):
        self.fileName = '/root/gnuradio/graduate_demo/Server/svc.file'
        print self.fileName
        self.pack = packet.packet(self.fileName, 400, 177)
        self.tx = ofdm_tx.ofdm_tx('2.4G', 128, 80, 32)


    def startSend(self):
        one_packet = self.pack.get_one_packet()
        pktno = 0
        while one_packet:
            print pktno, ' ' ,len(one_packet)
            pktno += 1
            self.tx.send_pkt(one_packet)
            one_packet = self.pack.get_one_packet()
        self.tx.send_pkt(eof=True)




if __name__ == '__main__':
    server = ServerData()
    server.startSend()
