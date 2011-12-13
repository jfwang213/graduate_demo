#!/bin/sh
./benchmark_ofdm_tx.py -f 2.45G -T B -v --tx-amplitude 5000 -i 64 --fft-length 128 --occupied-tones 80 --cp-length 32 -M 10 $*
