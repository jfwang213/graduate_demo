import read_video
import struct
import sys
import time
import threading
sys.path.append('../../src/')
from utils import log
from packetization import packet
from packetization import unpacket
from packetization.nal_utils import get_nal_type
file_name = "F:\\code\\rtp4svc\\trunk\\bin\\test_4layer.264"
import pdb
log.log_start()
produce_packet = packet.packet(file_name,400,177,0)
consume_packet = unpacket.unpacket(1)
def cal_header_len(header):
	base_len = 12
	fb = header[0:1]
	fb = struct.unpack('!B',fb)[0]
	fb = fb&15
	return base_len + fb *4
	
def producer_rtp():
	print 'producer start'
	while True:
		log.log_str("one packet",0)
		one_packet = produce_packet.get_one_packet()
		if one_packet == None:
			break
		consume_packet.put_one_rtp(one_packet)
		header_len = cal_header_len(one_packet)
		one_payload = one_packet[header_len:]
		payload_fb = struct.unpack('!B',one_payload[0:1])[0]

	consume_packet.end_rtp()
	log.log_str('nal num '+str(produce_packet.nal_num),0)
	print 'producer end'

def consumer_rtp():
	time.sleep(1)
	print 'consumer start'
	nal_num = 0
	while True:
		log.log_str("***************************************************",1)
		nal = consume_packet.get_one_nal()
		if nal == None:
			break
		nal_num += 1
		if len(nal)<=0:
			continue
		log.log_str("get one nal nal_type:"+str(get_nal_type(nal)),1)
	log.log_str('nal num '+str(nal_num),1)
	print "consumer end"

producer = threading.Thread(target=producer_rtp)
consumer = threading.Thread(target=consumer_rtp)
producer.start()
consumer.start()

producer.join()
consumer.join()
	
produce_packet.end()
	
log.log_end()

