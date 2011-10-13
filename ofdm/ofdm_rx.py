#!/usr/bin/env python


from gnuradio import blks2, gr
from gnuradio.eng_option import eng_option
import fusb_options
from pick_bitrate import pick_rx_bitrate
from optparse import OptionParser
from receive_path import receive_path

from benchmark_ofdm_rx import my_top_block
import struct

class ofdm_rx(object):
    def __init__(self, midFreq, fft_len, occupied_len, cp_len, dinter, callback = None):
        if midFreq == None or fft_len == None or \
            occupied_len == None or cp_len == None:
            print 'param is not ok!'
            return

        if callback == None:
            callback = self.rx_callback
        args = ['-d', str(dinter), '-R', 'B']        
        args.append('-f')
        args.append(str(midFreq))
        args.append('--fft-length')
        args.append(str(fft_len))
        args.append('--occupied-tones')
        args.append(str(occupied_len))
        args.append('--cp-length')
        args.append(str(cp_len))
        
        parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
        expert_grp = parser.add_option_group("Expert")

        my_top_block.add_options(parser, expert_grp)
        receive_path.add_options(parser, expert_grp)
        blks2.ofdm_mod.add_options(parser, expert_grp)
        blks2.ofdm_demod.add_options(parser, expert_grp)
        fusb_options.add_options(expert_grp)

        (options, args) = parser.parse_args (args)
        
        self.tb = my_top_block(callback, options)

        r = gr.enable_realtime_scheduling()
        if r != gr.RT_OK:
            print 'Warning: failed to enable realtime scheduling'

        self.n_rcvd = 0
        self.n_right = 0
        
        print 'rx init ok!'
    
    def rx_callback(self, ok, payload):
        self.n_rcvd += 1
        (pktno,) = struct.unpack('!H', payload[0:2])
        if ok:
            self.n_right += 1
        print "ok: %r \t pktno: %d \t n_rcvd: %d \t n_right: %d" % (ok, pktno, self.n_rcvd, self.n_right)
    
    def wait(self):
        self.tb.wait()
    def start(self):
        self.tb.start()
if __name__ == '__main__':
    tx = ofdm_rx('2.4G', 128, 80, 32)
    tx.start()
    tx.wait()
