#!/usr/bin/env python

from Constants import Constants

from gnuradio import gr, modulation_utils
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import usrp_receive_path, usrp_transmit_path

from transceiver import my_top_block
import struct

class Client(object):
    def __init__(self):
        self.macAddr = 1
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
        self.reqId = 0

        print "client init ok!"
    def callback(self, ok, payload):
        (pktno,) = struct.unpack('!H', payload[0:2])
        self.n_rcvd += 1
        if ok:
            self.n_right += 1
            self.dealWithCommand(payload[2:])
        print "ok = %5s  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
            ok, pktno, self.n_rcvd, self.n_right)

    def dealWithCommand(self, commandContent):
        srcMac = struct.unpack('!B', commandContent[0:1])
        dstMac = struct.unpack('!B', commandContent[1:2])
        if dstMac != self.macAddr:
            print 'not my package'
            return
        commandType = struct.unpack('!B', commandContent[2:3])
        if commandType == Constants.FreqAssign:
            width = struct.unpack('!I', commandContent[3:7])
            print 'get freqAssign width:', width

    def sendReqPackage(self, width):
        payload = ''
        payload += struct.pack('!BBB', self.macAddr, 0, Constants.FreqReq)
        payload += struct.pack('!II', width, self.reqId)
        self.send_pkt(payload)
        self.reqId += 1

    def send_pkt(self, payload='', eof=False):
        self.pkt_no += 1
        payload = struct.pack('!H', self.pkt_no) + payload
        self.tb.txpath.send_pkt(payload, eof)

    def wait(self):
        self.tb.wait()


if __name__ == '__main__':
    client = Client()
    content = raw_input("input command!\n")
    while content != 'E':
        if content[0:1] == 'R':
            try:
                width = int(content[2:])
            except ValueError as e:
                width = None
            if width != None:
                client.sendReqPackage(width)
            else:
                print 'invalid number'
        content = raw_input()
    server.send_pkt('', True)
    print 'send end!'
    server.wait()

