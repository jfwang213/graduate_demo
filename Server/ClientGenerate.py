from socket import *
from SocketUtils import recvFixedLen
import time
import threading
import heapq
import thread
import random
import struct
import sys, traceback
host = 'localhost'
port = 12341
addr = (host,port)
REQUEST=1
ALLOCATE=2
RELEASE=3
END=255
class Client(object):
    def __init__(self):
        self.tcpSocket = socket(AF_INET,SOCK_STREAM)
        self.tcpSocket.connect(addr)
        self.releaseQueue = []
        self.queueLock = thread.allocate_lock()
        self.end = False
        time.clock()
        self.averageServiceTime = 95
        self.serviceTimeLamb = 1.0/self.averageServiceTime
        self.inputTimeLamb = 3.6
        random.seed()
        self.oneBw = 1300
        self.logFile = file('clientLog.txt','w')
    def work(self):
        #start request
        self.requestThread = threading.Thread(target=self.dealWithRequest,args=())
        self.requestThread.start()
        #start release
        self.releaseThread = threading.Thread(target=self.dealWithRelease,args=())
        self.releaseThread.start()
        try:
            i = 0
            while not self.end and i < 10000:
                time.sleep(1)
                i += 1
        except:
            traceback.print_exc(file=sys.stdout) 
            pass
        print 'before thread join'
        self.requestThread.join()
        print 'request thread end'
        self.releaseThread.join()
        print 'release thread end'
        print 'thread end'
        self.EndRequest()
        self.tcpSocket.close()
        self.logFile.close()
        pass
    def stop(self):
        self.end = True
    def dealWithRelease(self):
        while not self.end:
            self.queueLock.acquire()
            nowTime = time.clock()
            tempList = []
            while len(self.releaseQueue) > 0 and nowTime > self.releaseQueue[0][0]:
                tempList.append(self.releaseQueue[0][1])
                heapq.heappop(self.releaseQueue)
            self.queueLock.release()
            for bw in tempList:
                self.releaseOnce(bw)
            time.sleep(0.01)
        pass
    def releaseOnce(self,bw):
        data = struct.pack('!bf',RELEASE,bw)
        self.tcpSocket.send(data)

    def SetParam(self, inputrate, averageService):
        print "set client param inputrate:%f serviceTime:%f" % (inputrate, averageService)
        self.inputTimeLamb  = inputrate
        self.serviceTimeLamb = 1.0/averageService
        
    def requestOnce(self, serviceTime):
        data = struct.pack('!bff',REQUEST,self.oneBw, serviceTime)
        self.tcpSocket.send(data)
        data = recvFixedLen(self.tcpSocket,5)
        allocateBw = struct.unpack('!bf',data)[1]
        return allocateBw

    def EndRequest(self):
        data = struct.pack("!B", END)
        self.tcpSocket.send(data)
        print "stop request"
        
    def dealWithRequest(self):
        allSleepTime = 0
        allNum = 0
        while not self.end:
            nowTime = time.clock()
            serviceTime = random.expovariate(self.serviceTimeLamb)
            allocateBw = self.requestOnce(serviceTime)
            self.queueLock.acquire()
            self.logFile.write(str(nowTime)+' '+str(serviceTime)+' '+str(allocateBw) + ' ' + str(len(self.releaseQueue) + 1) + '\n')
            heapq.heappush(self.releaseQueue,(nowTime+serviceTime,allocateBw))
            self.queueLock.release()
            sleepTime = random.expovariate(self.inputTimeLamb)
            allSleepTime += sleepTime
            allNum += 1
            while time.clock()<(nowTime + sleepTime) and not self.end:
                time.sleep(sleepTime-(time.clock()-nowTime))
        pass

if __name__ == '__main__':
    cli = Client()
    print 'client start to work'
    cli.work()
