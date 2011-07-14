from gnuradio import gr, blks2
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import struct, sys

# from current dir
from receive_path import receive_path
from transmit_path import transmit_path

import fusb_options

class transceiver(gr.top_block):
    def __init__(self, callback, options):
        gr.top_block.__init__(self)

        #init receive
        self._rx_freq = options.rx_freq
        self._rx_gain = options.rx_gain
        self._rx_subdev_spec = options.rx_subdev_spec
        self._decim = options.decim

        #init transmit
        self._tx_freq = options.tx_freq
        self._tx_subdev_spec = options.tx_subdev_spec
        self._interp = options.interp
        
        #init both
        self._fusb_block_size = options.fusb_block_size
        self._fusb_nblocks = options.fusb_nblocks

        #check receive
        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        #check transmit
        if self._tx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        #set up usrp source
        self._setup_usrp_source()
        self._setup_usrp_sink()

        # Set the USRP for maximum transmit gain
        # (Note that on the RFX cards this is a nop.)
        self.tx_subdev.set_gain((self.tx_subdev.gain_range()[0] + \
            self.tx_subdev.gain_range()[1])/2)

        self.rx_path = receive_path(callback, options)
        self.connect(self.rx_u, self.rx_path)

        self.tx_path = transmit_path(options)
        self.connect(self.tx_path, self.tx_u)

    def _setup_usrp_sink(self):
        """
        Creates a USRP sink, determines the settings for best bitrate,
        and attaches to the transmitter's subdevice.
        """
        self.tx_u = usrp.sink_c(fusb_block_size=self._fusb_block_size,
                             fusb_nblocks=self._fusb_nblocks)

        self.tx_u.set_interp_rate(self._interp)

        # determine the daughterboard subdevice we're using
        if self._tx_subdev_spec is None:
            self._tx_subdev_spec = usrp.pick_tx_subdevice(self.tx_u)
        self.tx_u.set_mux(usrp.determine_tx_mux_value(self.tx_u, self._tx_subdev_spec))
        self.tx_subdev = usrp.selected_subdev(self.tx_u, self._tx_subdev_spec)

        # Set center frequency of USRP
        ok = self.tx_u.tune(0, self.tx_subdev, self._tx_freq)
        if not ok:
            print "Failed to set Tx frequency to %s" % (eng_notation.num_to_str(self._tx_freq),)
            raise ValueError


        # enable Auto Transmit/Receive switching
        self.tx_subdev.set_auto_tr(True)



    def _setup_usrp_source(self):
        self.rx_u = usrp.source_c (fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        adc_rate = self.rx_u.adc_rate()

        self.rx_u.set_decim_rate(self._decim)

        # determine the daughterboard subdevice we're using
        if self._rx_subdev_spec is None:
            self._rx_subdev_spec = usrp.pick_rx_subdevice(self.rx_u)
        self.rx_subdev = usrp.selected_subdev(self.rx_u, self._rx_subdev_spec)

        self.rx_u.set_mux(usrp.determine_rx_mux_value(self.rx_u, self._rx_subdev_spec))
        ok = self.rx_u.tune(0, self.rx_subdev, self._rx_freq)
        if not ok:
            print "Failed to set Rx frequency to %s" % (eng_notation.num_to_str(self._rx_freq),)
            raise ValueError

        self.rx_subdev.set_auto_tr(True)
        

    def add_options(normal, expert):
        """
        Adds usrp-specific options to the Options Parser
        """
        add_freq_option(normal)
        normal.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                          help="select USRP Tx side A or B")
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("", "--tx-freq", type="eng_float", default=None,
                          help="set transmit frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-i", "--interp", type="intx", default=256,
                          help="set fpga interpolation rate to INTERP [default=%default]")
       
        normal.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B")
        normal.add_option("", "--rx-gain", type="eng_float", default=None, metavar="GAIN",
                          help="set receiver gain in dB [default=midpoint].  See also --show-rx-gain-range")
        normal.add_option("", "--show-rx-gain-range", action="store_true", default=False, 
                          help="print min and max Rx gain available on selected daughterboard")
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("", "--rx-freq", type="eng_float", default=None,
                          help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-d", "--decim", type="intx", default=128,
                          help="set fpga decimation rate to DECIM [default=%default]")
        expert.add_option("", "--snr", type="eng_float", default=30,
                          help="set the SNR of the channel in dB [default=%default]")

        #other necessary options
        transmit_path.add_options(normal, expert)
        receive_path.add_options(normal, expert)
        blks2.ofdm_mod.add_options(normal, expert)
        blks2.ofdm_demod.add_options(normal, expert)
        fusb_options.add_options(expert)

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)

def add_freq_option(parser):
    """
    Hackery that has the -f / --freq option set both tx_freq and rx_freq
    """
    def freq_callback(option, opt_str, value, parser):
        parser.values.rx_freq = value
        parser.values.tx_freq = value

    if not parser.has_option('--freq'):
        parser.add_option('-f', '--freq', type="eng_float",
                          action="callback", callback=freq_callback,
                          help="set Tx and/or Rx frequency to FREQ [default=%default]",
                          metavar="FREQ")

