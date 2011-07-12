from gnuradio import gr, blks2
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import struct, sys

# from current dir
from receive_path import receive_path
import fusb_options

class transceiver_top_block(gr.top_block):
    def __init__(self, callback, options):
        gr.top_block.__init__(self)

        #init receive
        self._rx_freq = options.rx_freq
        self._rx_gain = options.rx_gain
        self._rx_subdev_spec = options.rx_subdev_spec
        self._decim = options.decim

        #init transmit

        #init both
        self._fusb_block_size = options.fusb_block_size
        self._fusb_nblocks = options.fusb_nblocks

        #check receive

        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit
        #check transmit

        #set up usrp source
        self._setup_usrp_source()




    def _setup_usrp_source(self):
        self.u = usrp.source_c (fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        adc_rate = self.u.adc_rate()

        self.u.set_decim_rate(self._decim)

        # determine the daughterboard subdevice we're using
        if self._rx_subdev_spec is None:
            self._rx_subdev_spec = usrp.pick_rx_subdevice(self.u)
        self.subdev = usrp.selected_subdev(self.u, self._rx_subdev_spec)

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, self._rx_subdev_spec))
    
    def set_freq(self, target_freq):
        r = self.u.tune(0, self.subdev, target_freq)
        if r:
            return True

        return False


