# Create your views here.
from django.shortcuts import render_to_response
from django.http import HttpResponse
from socket import *
import struct

"""
the Command between server and server web ui
"""
GetFreeDataChannel = 1
GetAllActiveClient = 2
GetOneActiveClient = 3
GetEstParam = 4
SetClientParam = 5

GiveAllActiveClient = 101
GiveOneActiveClient = 102
GiveFreeDataChannel = 103
GiveEstParam = 104

End = 254
Error = 255
def RecvFixLen(sock, length):
    res = ''
    while length > 0:
        content = sock.recv(length)
        length -= len(content)
        res += content
    return res

def EndChat(socket):
    content = struct.pack("!B", End)
    socket.send(content)

def GetAllActiveClientInfo(socket):
    content = struct.pack("!B", GetAllActiveClient)
    socket.send(content)
    command = RecvFixLen(socket, 1)
    command = struct.unpack("!B", command)[0]
    if command != GiveAllActiveClient:
        return None
    else:
        number = RecvFixLen(socket, 4)
        number = struct.unpack("!I", number)[0]
        content = RecvFixLen(socket, 5*number)
        res = []
        for i in range(number):
            (mac, reqID) = struct.pack("!BI", content[i*5:i*5+5])
            res.append({'mac':mac, 'reqID':reqID})
        return res

def GetFreeDataChannelNumber(socket):
    content = struct.pack("!B", GetFreeDataChannel)
    socket.send(content)
    command = RecvFixLen(socket, 1)
    command = struct.unpack("!B", command)[0]
    if command != GiveFreeDataChannel:
        return None
    else:
        number = RecvFixLen(socket, 1)
        number = struct.unpack("!B", number)[0]
        return number
def GetEstInfomation(socket):
    content = struct.pack("!B", GetEstParam)
    socket.send(content)
    command = RecvFixLen(socket, 9)
    (command, averSerTime, inputrate) = struct.unpack("!Bff", command)
    if command != GiveEstParam:
        return None
    else:
        return (averSerTime, inputrate)

def info(request):
    sock = socket(AF_INET, SOCK_STREAM)
    serverOn = True
    try:
        sock.connect(("127.0.0.1", 12347))
    except:
        serverOn = False
    if serverOn:
        freeDataChannelNumber = GetFreeDataChannelNumber(sock)
        clients = GetAllActiveClientInfo(sock)
        (averSerTime, inputrate) = self.GetEstInformation(sock)
        EndChat(sock)
        sock.close()
        return render_to_response('server/info.html', {'freeDataChannelNumber': freeDataChannelNumber, 'clients': clients, 'averSerTime': averSerTime, 'inputrate', inputrate})
    else:
        return HttpResponse("The server is Down!")

