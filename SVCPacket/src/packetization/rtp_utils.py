import struct
def get_rtp_header_len(rtp):
	base_len = 12
	fb = rtp[0:1]
	fb = struct.unpack('!B',fb)[0]
	fb = fb&15
	return base_len + fb *4
	pass
def get_seq_num(rtp_header):
	num = struct.unpack('!H',rtp_header[2:4])[0]
	return num
def get_fu_a_header(rtp_payload):
	return struct.unpack('!B',rtp_payload[1:2])[0]
def check_rtp_fu_as(rtps):
	if len(rtps)<=1:
		return False
	#check start
	first_header_len = get_rtp_header_len(rtps[0])
	first_payload = rtps[0][first_header_len:]
	fu_header = struct.unpack('!B',first_payload[1:2])[0]
	if fu_header&(1<<7) == 0:
		print 'start error'
		return False
	#check end
	last_header_len = get_rtp_header_len(rtps[-1])
	last_payload = rtps[-1][last_header_len:]
	fu_header = struct.unpack('!B',last_payload[1:2])[0]
	if fu_header&(1<<6) == 0:
		print 'end error'
		return False
	#check sequenceNum
	first_seq = get_seq_num(rtps[0])
	max_num = 1<<16
	for i in range(len(rtps),1):
		next_seq = get_seq_num(rtps[i])
		if next_seq != ((first_seq+i)%max_num):
			print 'seq num error'
			return False
	return True
	pass