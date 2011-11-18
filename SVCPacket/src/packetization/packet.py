import read_video
import struct
import pdb
from Utils.Log import Log
class packet:
    def __init__(self, file_name, mtu_size, ssrc, dqID):
        read_video.file_name = file_name
        read_video.init_file(read_video.file_name)
        self.mtu_size = mtu_size
        self.seq_num = 1
        self.ssrc = ssrc
        self.timestamp = 0
        self.nals = []
        self.left_nal = ""
        self.first_byte = 0
        self.nal_num = 0
        self.accessUnitID = 0
        self.dqID = dqID
        self.packetLog = Log("packetLog.txt")
        self.packetNumber = 1
        
    def end(self):
        read_video.close_file()

    def extractNalsBydqID(self, nals):
        res = []
        for nal in nals:
            self.packetLog.LogStr('get one nal with dqID %d' % nal.nal_dqID)
            if nal.nal_dqID <= self.dqID:
                res.append(nal)
        return res

    def get_one_packet(self):

        self.packetLog.LogStr('************* packet id %d **********************' % self.packetNumber)
        self.packetNumber += 1
        consume_nal_num = 0
        header = self.build_rtp_header()
        left_space = self.mtu_size - len(header)
        #deal with fu_a
        if len(self.left_nal) != 0:
            (consume_size,payload) = self.build_fu_a_payload(self.left_nal, left_space, 0)
            self.left_nal = self.left_nal[consume_size:]
            return header + payload
        #deal with new nal
        while len(self.nals) == 0:
            self.packetLog.LogStr('read one access unit access unit id ' +
                str(self.accessUnitID))
            self.accessUnitID += 1
            au = read_video.read_one_access_unit()
            if au == None:
                return None
            self.packetLog.LogStr('access unit id ' + str(self.accessUnitID) + ' nals number: ' + str(len(au.nals)))
            self.nals += self.extractNalsBydqID(au.nals)
            self.nal_num = len(self.nals)

        curr_nal = self.nals[0]
        if len(curr_nal.video_byte) > left_space:
            self.packetLog.LogStr('one large nal: nal size '+str(len(curr_nal.video_byte)))
            self.left_nal = curr_nal.video_byte
            self.first_byte = struct.unpack('!B',self.left_nal[0:1])[0]
            (consume_size,payload) = self.build_fu_a_payload(self.left_nal,left_space,1)
            self.left_nal = self.left_nal[consume_size:]
            consume_nal_num = 1
        else:
            num = self.cal_stap_a_nal_num(self.nals,left_space)
            if num <= 1:
                payload = self.build_nor_payload(curr_nal)
                consume_nal_num = 1
            else:
                consume_nal_num = num
                payload = self.build_stap_a_payload(self.nals[0:num])
        self.nals = self.nals[consume_nal_num:]
        return header + payload
        
    def cal_stap_a_nal_num(self,au_left_nals,left_space):
        num = 0
        left_space -= 1
        next_id = 0
        while left_space >= 0 and next_id < len(au_left_nals):
            left_space -= len(au_left_nals[next_id].video_byte) + 2
            next_id += 1
            num += 1
        if left_space <0:
            num -= 1
        return num

    def build_stap_a_payload(self,nals):
        fb = 0
        for i in range(len(nals)):
            fb |= struct.unpack('!B', nals[i].video_byte[0:1])[0]
        fb &= 255-31
        fb |= 24
        payload = ""
        payload += struct.pack("!B",fb)
        for i in range(len(nals)):
            self.packetLog.LogStr('stap_a one nal size:'+str(len(nals[i].video_byte)))
            payload += struct.pack("!H",len(nals[i].video_byte))
            payload += nals[i].video_byte
        return payload

    def build_fu_a_payload(self,left_nal,left_space,start):
        self.packetLog.LogStr('fu_a start '+str(start))
        
        left_space -= 2
        payload = ''
        fb = self.first_byte
        fb  = fb & (255-31)
        fb = fb | 28
        sb = start<<7
        if len(left_nal) <= left_space:
            left_space = len(left_nal)
            sb |= 1<<6
        sb = sb | (self.first_byte&31)
        self.packetLog.LogStr('fu_header '+str(sb))
        payload = struct.pack('!BB',fb,sb)
        payload += left_nal[0:left_space]
        self.packetLog.LogStr('fu_a consume size '+str(left_space))
        return (left_space,payload)
        
    def build_nor_payload(self,nal):
        self.packetLog.LogStr('nor consume size ' + str(len(nal.video_byte)))
        return nal.video_byte

    def build_rtp_header(self):
        fb = 0
        v = 2
        p = 0
        x = 0
        cc = 0
        fb += cc+(x<<4)+(p<<5)+(v<<6)
        sb = 0
        M = 0
        pt = 0
        sb += pt + (M<<7)
        
        header = ""
        self.packetLog.LogStr('seq num '+str(self.seq_num))
        header += struct.pack('!BBHII',fb,sb,self.seq_num,self.timestamp,self.ssrc)
        self.seq_num += 1
        return header
