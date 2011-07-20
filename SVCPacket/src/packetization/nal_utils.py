import struct
def get_nal_type(header):
	fb = struct.unpack('!B',header[0:1])[0]
	return fb&(31)