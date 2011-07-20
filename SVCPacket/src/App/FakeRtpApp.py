import SockClientWrap
from packetization import packet
from packetization import unpacket
import threading
import time
class FakeRtpApp:
	def __init__(self,file_name):
		self.client_socket = SockClientWrap.SockClientWrap()
		self.file_name = file_name
	def producer_rtp(self):
		print 'producer start'
		while True:
			one_packet = self.pack.get_one_packet()
			if one_packet == None:
				break
			self.unpack.put_one_rtp(one_packet)
		self.unpack.end_rtp()
		print 'producer end'

	def consumer_rtp(self):
		time.sleep(1)
		print 'consumer start'
		nal_num = 0
		while True:
			nal = self.unpack.get_one_nal()
			if nal == None:
				self.client_socket.close()
				break
			self.client_socket.send_one_packet(nal)
			nal_num += 1
			if len(nal)<=0:
				continue
		print "consumer end"
	def work(self):
		connect_ok = False
		while not connect_ok:
			print 'try connect server!'
			connect_ok = self.client_socket.connect()
		self.pack = packet.packet(self.file_name,400,177)
		self.unpack = unpacket.unpacket()
		self.pack_thread = threading.Thread(target=self.producer_rtp)
		self.unpack_thread = threading.Thread(target=self.consumer_rtp)
		self.pack_thread.start()
		self.unpack_thread.start()
		self.pack_thread.join()
		self.unpack_thread.join()