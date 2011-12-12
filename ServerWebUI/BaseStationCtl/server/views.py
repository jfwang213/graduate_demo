# Create your views here.
from django.shortcuts import render_to_response
from django.http import HttpResponse
from socket import *
import struct
from utils import *

def GetFreqLine():
    resultFile = file("/root/graduate_demo/ServerWebUI/freqResult.txt")
    numbers = []
    while True:
        line = resultFile.readline()
        if len(line) == 0:
            break
        number = float(line[0:-1]) #remove enter
        numbers.append(number)
    resultFile.close()
    maxNum = 100
    if maxNum > len(numbers):
        startNum = 0
    else:
        startNum = len(numbers)-maxNum

    result = "["
    while startNum < len(numbers):
        #result.append(numbers[startNum])
        result += str(numbers[startNum]) + ','
        startNum += 1
    return result + ']'

def recentFreqUsed(request):
    return HttpResponse(GetFreqLine())

def info(request):
    sock = socket(AF_INET, SOCK_STREAM)
    serverOn = True
    try:
        sock.connect(("127.0.0.1", 12347))
    except:
        serverOn = False
    if serverOn:
        if 'inputtype' in request.GET:
            inputtype = request.GET['inputtype']
            if inputtype == '1':
                SetClientParam(sock, 1)
            elif inputtype == '2':
                SetClientParam(sock, 2)
            elif inputtype == '3':
                SetClientParam(sock, 3)
        freeDataChannelNumber = GetFreeDataChannelNumber(sock)
        clients = GetAllActiveClientInfo(sock)
        (averSerTime, inputrate) = GetEstInformation(sock)
        (maxFreq, leftFreq, nowSatis, activeNum, servedNum, inputtype) = GetServerInfo(sock)
        freqRatio = (maxFreq-leftFreq)/maxFreq * 100
        (cliAverSerTime, cliInputRate) = GetClientGenerateInfo(sock)
        EndChat(sock)
        sock.close()
        chartStr = GetFreqLine()
        return render_to_response('server/info.html', {'freeDataChannelNumber': freeDataChannelNumber, 'clients': clients, 'averSerTime': averSerTime, 'inputrate':inputrate, 'inputtype':str(inputtype), 'maxFreq':maxFreq, 'leftFreq':leftFreq, 'nowSatis':nowSatis, 'activeNum':activeNum, 'freqRatio':freqRatio, 'servedNum':servedNum, 'cliAverSerTime':cliAverSerTime, 'cliInputRate':cliInputRate, 'chartStr':chartStr})
    else:
        return HttpResponse("The server is Down!")

