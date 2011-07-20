from socket import *
import sys
import struct
class SockClientWrap:
	def __init__(self,server_port=12345,server_addr='127.0.0.1'):
		self.client_socket = socket(AF_INET,SOCK_STREAM)
		self.server_port = server_port
		self.server_addr = server_addr
		#self.client_socket.settimeout(1)
	def connect(self):
		try:
			self.client_socket.connect((self.server_addr,self.server_port))
		except:
			print "Unexpected error:", sys.exc_info()[0]
			return False
		return True
	def send_one_packet(self,content):
		length = len(content)
		payload = struct.pack("!I",length)
		payload += content
		self.client_socket.send(payload)
		
	def close(self):
		self.client_socket.close()