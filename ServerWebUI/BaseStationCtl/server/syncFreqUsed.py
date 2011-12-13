from utils import *
from socket import *
import struct
import time
def syncUsedFreq():
    while True:
        try:
            sock = socket(AF_INET, SOCK_STREAM)
            sock.connect(("127.0.0.1", 12347))
        except:
            print "connect error"
            time.sleep(1)
            continue
        break
    resultFile = file("freqResult.txt", 'w')
    try:
        while True:
            (maxFreq, leftFreq, nowSatis, activeNum, servedNum,a) = GetServerInfo(sock)
            resultFile.write(str((maxFreq-leftFreq)/maxFreq) + '\n')
            resultFile.flush()
            time.sleep(1)
    except:
        pass
    resultFile.close()
    EndChat(sock)
    sock.close()

if __name__ == '__main__':
    syncUsedFreq()
