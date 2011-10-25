import struct,threading
from SVCPacket.src.utils import log
from rtp_utils import get_rtp_header_len,check_rtp_fu_as,get_seq_num,get_fu_a_header
from nal_utils import get_nal_type
class unpacket:
    def __init__(self,log_id=-1):
        self.nals = []
        self.next_rtp = None
        self.rtp_con = threading.Condition()
        self.rtp_buffer = []
        self.rtp_end = False
        self.log_id = log_id

    def get_one_rtp(self):
        self.rtp_con.acquire()
        while len(self.rtp_buffer) == 0:
            if self.rtp_end:
                self.rtp_con.release()
                return None
            #wait will release the contition and acquire it after time expire
            self.rtp_con.wait(1) 
        one_rtp = self.rtp_buffer[0]
        self.rtp_buffer = self.rtp_buffer[1:]
        self.rtp_con.release()
        return one_rtp

    def put_one_rtp(self,rtp):
        self.rtp_con.acquire()
        self.rtp_buffer.append(rtp)
        self.rtp_con.notify()
        self.rtp_con.release()
        
    def end_rtp(self):
        self.rtp_end = True
        
    def get_rtp_type(self,rtp_payload):
        fb = struct.unpack('!B',rtp_payload[0:1])[0]
        rtp_type = fb&31
        return rtp_type
    def extract_nal_from_nor_rtp(self,rtp_payload):
        return rtp_payload
    
    def extract_nal_from_fu_a_rtp(self,rtp_payloads,rtps):
        nal = ''
        if not check_rtp_fu_as(rtps):
            log.log_str('fu check error',self.log_id)
            return nal
        log.log_str('fu check succeed', self.log_id)
        for i in range(len(rtp_payloads)):
            nal += rtp_payloads[i][2:]
        return nal
    
    def extract_nal_from_stap_a_rtp(self,rtp_payload):
        rtp_payload = rtp_payload[1:]
        nals = []
        while len(rtp_payload)>0:
            now_nal_size = struct.unpack('!H',rtp_payload[0:2])[0]
            nals.append(rtp_payload[2:now_nal_size+2])
            rtp_payload = rtp_payload[now_nal_size+2:]
        log.log_str('extract nal from stap, nal number: ' + str(len(nals)),
            self.log_id)
        return nals
    
    def get_one_nal(self):
        #pdb.set_trace()
        if len(self.nals) >0:
            res = self.nals[0]
            self.nals = self.nals[1:]
            return res
        else:
            if self.next_rtp == None:
                now_rtp = self.get_one_rtp()
                if now_rtp == None:
                    return None
            else:
                now_rtp = self.next_rtp
                self.next_rtp = None
            header_len = get_rtp_header_len(now_rtp)
            now_payload = now_rtp[header_len:]
            rtp_type = self.get_rtp_type(now_payload)
            if rtp_type <=23:
                log.log_str("receive one nor rtp",self.log_id)
                res = self.extract_nal_from_nor_rtp(now_payload)
                return res
            elif rtp_type == 24:
                log.log_str('receive one stap_a rtp',self.log_id)
                self.nals = self.extract_nal_from_stap_a_rtp(now_payload)
                res = self.nals[0]
                self.nals = self.nals[1:]
                return res
            elif rtp_type == 28:
                #pdb.set_trace()
                log.log_str('receive one fu_a rtp',self.log_id)
                rtp_payloads = []
                rtps = []
                now_seq_num = get_seq_num(now_rtp)
                log.log_str('fu_a start seq num '+str(now_seq_num),self.log_id)
                fu_a_end = False
                while now_rtp != None and (not fu_a_end):
                    header_len = get_rtp_header_len(now_rtp)
                    now_payload = now_rtp[header_len:]
                    rtp_type = self.get_rtp_type(now_payload)
                    if rtp_type != 28:
                        self.next_rtp = now_rtp
                        fu_a_end = True
                    else:
                        fu_header = get_fu_a_header(now_payload)
                        if len(rtps)>0:
                            if fu_header &(1<<7)>0:
                                self.next_rtp = now_rtp
                                fu_a_end = True
                            else:
                                rtps.append(now_rtp)
                                rtp_payloads.append(now_payload)
                                if fu_header & (1<<6) >0:
                                    fu_a_end = True
                                else:
                                    now_rtp = self.get_one_rtp()
                        else:
                            rtps.append(now_rtp)
                            rtp_payloads.append(now_payload)
                            now_rtp = self.get_one_rtp()
                now_seq_num = get_seq_num(rtps[-1])
                log.log_str('fu_a end seq num '+str(now_seq_num),self.log_id)
                res = self.extract_nal_from_fu_a_rtp(rtp_payloads,rtps)
                return res
            else:
                print "error rtp_type"
                pass
