#!/usr/bin/env python
from gnuradio.eng_option import eng_option
from gnuradio import gr
from optparse import OptionParser
import struct, sys
from transceiver import transceiver


import time

def main():
    global n_rcvd, n_right
        
    n_rcvd = 0
    n_right = 0

    def rx_callback(ok, payload):
        global n_rcvd, n_right
        n_rcvd += 1
        (pktno,) = struct.unpack('!H', payload[0:2])
        if ok:
            n_right += 1
        print "ok: %r \t pktno: %d \t n_rcvd: %d \t n_right: %d" % (ok, pktno, n_rcvd, n_right)

        if 0:
            printlst = list()
            for x in payload[2:]:
                t = hex(ord(x)).replace('0x', '')
                if(len(t) == 1):
                    t = '0' + t
                printlst.append(t)
            printable = ''.join(printlst)

            print printable
            print "\n"

    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinuous")
    transceiver.add_options(parser, expert_grp)

    (options, args) = parser.parse_args()

    tr = transceiver(rx_callback, options)
    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: failed to enable realtime scheduling"

    tr.start()


    #nbytes = int(1e6)
    #n = 0
    #pkt_size = 400
    #pktno = 0
    #while n < nbytes:
    #    tr.send_pkt(struct.pack('!H', pktno) + (pkt_size - 2) * chr(pktno & 0xff))
    #    pktno += 1
    #    n += 1
    #    if pktno % 5 == 0:
    #        print '.',
    #time.sleep(1000)
    #send_pkt('',True)
    tr.wait()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass


