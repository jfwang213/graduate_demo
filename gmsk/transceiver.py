from gnuradio import gr, modulation_utils
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import struct, sys

# from current dir
import usrp_receive_path
import usrp_transmit_path

class transceiver(gr.top_block):
    def __init__(self, callback):
        gr.top_block.__init__(self)
        
        parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
        expert_grp = parser.add_option_group("Expert")
        parser.add_option("","--discontinuous", action="store_true", default=False, help="enable discontinuous")
        transceiver.add_options(parser, expert_grp)
        args = ['-f', '2.4G', '-u', '1', '-i', '64', '-d', '32']
        (options, args) = parser.parse_args(args)

        #init receive
        self._rx_freq = options.rx_freq
        self._rx_gain = options.rx_gain
        self._rx_subdev_spec = options.rx_subdev_spec
        self._decim = options.decim

        #init transmit
        self._tx_freq = options.tx_freq
        self._tx_subdev_spec = options.tx_subdev_spec
        self._interp = options.interp
        
        #check receive
        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        #check transmit
        if self._tx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        demods = modulation_utils.type_1_demods()
        self.rx_path = usrp_receive_path.usrp_receive_path(demods[options.modulation], callback, options)
        self.connect(self.rx_path)

        mods = modulation_utils.type_1_mods()
        self.tx_path = usrp_transmit_path.usrp_transmit_path(mods[options.modulation], options)
        self.connect(self.tx_path)
        r = gr.enable_realtime_scheduling()
        if r != gr.RT_OK:
            print "Warning: failed to enable realtime scheduling"


    def send_pkt(self, payload='', eof=False):
        return self.tx_path.send_pkt(payload, eof)

    def add_options(normal, expert):
        """
        Adds usrp-specific options to the Options Parser
        """
        add_freq_option(normal)
        mods = modulation_utils.type_1_mods()
        demods = modulation_utils.type_1_demods()

        normal.add_option("-m", "--modulation", type="choice", choices=mods.keys(),
                      default='gmsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(mods.keys()),))

        #other necessary options
        usrp_transmit_path.add_options(normal, expert)
        usrp_receive_path.add_options(normal, expert)
        for mod in mods.values():
            mod.add_options(expert)
        for demod in demods.values():
            demod.add_options(expert)

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

