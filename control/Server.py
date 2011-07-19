#!/usr/bin/env python

from Constants import Constants
from gnuradio import gr, modulation_utils
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import usrp_receive_path, usrp_transmit_path

from transceiver import my_top_block
import struct

class Server:
    def __init__(self):
        self.macAddr = 0
        args = ["-f", "2.4G", "-R", "B", "-T", "B"]

        demods = modulation_utils.type_1_demods()
        mods = modulation_utils.type_1_mods()

        parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
        expert_grp = parser.add_option_group("Expert")
        
        parser.add_option("-m", "--modulation", type="choice", choices=mods.keys(),
                      default='gmsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(mods.keys()),))

        usrp_transmit_path.add_options(parser, expert_grp)
        usrp_receive_path.add_options(parser, expert_grp)

        for mod in mods.values():
            mod.add_options(expert_grp)

        for demod in demods.values():
            demod.add_options(expert_grp)

        (options, args) = parser.parse_args(args)
        
        self.tb = my_top_block(demods[options.modulation], mods[options.modulation], self.callback, options)

        r = gr.enable_realtime_scheduling()
        if r != gr.RT_OK:
            print 'Warning: Failed to enable realtime scheduling.'
        self.tb.start()
        self.n_rcvd = 0
        self.n_right = 0
        self.pkt_no = 0

        print "server init ok!"
    def callback(self, ok, payload):
        (pktno,) = struct.unpack('!H', payload[0:2])
        self.n_rcvd += 1
        if ok:
            self.n_right += 1
            self.dealWithCommand(payload[2:])
        print "ok = %5s  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
            ok, pktno, self.n_rcvd, self.n_right)

    def dealWithCommand(self, commandContent):
        srcMac = struct.unpack('!B', commandContent[0:1])[0]
        dstMac = struct.unpack('!B', commandContent[1:2])[0]
        if dstMac != self.macAddr:
            print 'not my package ', dstMac
            return
        commandType = struct.unpack('!B', commandContent[2:3])[0]
        if commandType == Constants.FreqReq:
            width = struct.unpack('!I', commandContent[3:7])[0]
            print 'get freqReq width:', width
            self.sendAssignPacket(srcMac, width, width/2)

    def sendAssignPacket(self, dstMac, width, midFreq):
        payload = ''
        payload += struct.pack('!BBB', self.macAddr, dstMac, Constants.FreqAssign)
        payload += struct.pack('!II', width, midFreq)
        self.send_pkt(payload)

    def send_pkt(self, payload='', eof=False):
        self.pkt_no += 1
        payload = struct.pack('!H', self.pkt_no) + payload
        self.tb.txpath.send_pkt(payload, eof)

    def wait(self):
        self.tb.wait()


if __name__ == '__main__':
    server = Server()
    content = raw_input("say something!\n")
    while content != 'E':
        server.send_pkt(content)
        print content
        content = raw_input()
    server.send_pkt('', True)
    print 'send end!'
    server.wait()

