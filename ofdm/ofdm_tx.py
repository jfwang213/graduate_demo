#!/usr/bin/env python


from gnuradio import blks2, gr
from gnuradio.eng_option import eng_option
import fusb_options
from pick_bitrate import pick_tx_bitrate
from optparse import OptionParser
from transmit_path import transmit_path

from benchmark_ofdm_tx import my_top_block
import struct

class ofdm_tx(object):
    def __init__(self, midFreq, fft_len, occupied_len, cp_len, inter):
        if midFreq == None or fft_len == None or \
            occupied_len == None or cp_len == None:
            print 'param is not ok!'
            return
        args = ['--tx-amplitude', '5000', '-i', str(inter), '-T', 'B']        
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
        transmit_path.add_options(parser, expert_grp)
        blks2.ofdm_mod.add_options(parser, expert_grp)
        blks2.ofdm_demod.add_options(parser, expert_grp)
        fusb_options.add_options(expert_grp)

        (options, args) = parser.parse_args (args)
        
        self.tb = my_top_block(options)

        r = gr.enable_realtime_scheduling()
        if r != gr.RT_OK:
            print 'Warning: failed to enable realtime scheduling'

        self.tb.start()
        print 'tx init ok!'

    def __del__(self):
        print "del ofdm_tx"
        del self.tb

    def send_pkt(self, payload='', eof=False):
        self.tb.txpath.send_pkt(payload, eof)

    def wait(self):
        self.tb.wait()

    def stop(self):
        self.tb.stop()

if __name__ == '__main__':
    tx = ofdm_tx('2.4G', 128, 80, 32)
    size = 400
    for i in range(100):
        payload = struct.pack('!H', i+1)
        payload += struct.pack('!B', ((i+1) % 254) + 1) * (size - 2)
        tx.send_pkt(payload)
        print '.',
    print ''
    tx.send_pkt(eof = True)
    tx.wait()
