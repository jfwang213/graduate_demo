from socket import *
from SocketUtils import recvFixedLen
import time
import threading
import heapq
import thread
import random
import struct
host = 'localhost'
port = 12341
addr = (host,port)
REQUEST=1
ALLOCATE=2
RELEASE=3
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
            while not self.end and i < 10000 
                time.sleep(1)
                i += 1
        except:
            pass
        self.end = True
        self.requestThread.join()
        self.releaseThread.join()
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
                print 'release one'
            self.queueLock.release()
            for bw in tempList:
                self.releaseOnce(bw)
            time.sleep(0.01)
        pass
    def releaseOnce(self,bw):
        data = struct.pack('!bf',RELEASE,bw)
        self.tcpSocket.send(data)

    def SetParam(self, inputrate, averageService):
        self.inputTimeLamb  = inputrate
        self.serviceTimeLamb = averageService
        
    def requestOnce(self):
        print 'send one request'
        data = struct.pack('!bf',REQUEST,self.oneBw)
        self.tcpSocket.send(data)
        data = recvFixedLen(self.tcpSocket,5)
        allocateBw = struct.unpack('!bf',data)[1]
        print 'get spectrum '+str(allocateBw)
        return allocateBw
        
    def dealWithRequest(self):
        while not self.end:
            allocateBw = self.requestOnce()
            nowTime = time.clock()
            serviceTime = random.expovariate(self.serviceTimeLamb)
            print 'service Time '+str(serviceTime)
            self.queueLock.acquire()
            self.logFile.write(str(nowTime)+' '+str(serviceTime)+' '+str(allocateBw) + ' ' + str(len(self.releaseQueue) + 1) + '\n')
            heapq.heappush(self.releaseQueue,(nowTime+serviceTime,allocateBw))
            self.queueLock.release()
            sleepTime = random.expovariate(self.inputTimeLamb)
            print 'interarrival time' + str(sleepTime)
            time.sleep(sleepTime)
        pass

if __name__ == '__main__':
    cli = Client()
    print 'client start to work'
    cli.work()
