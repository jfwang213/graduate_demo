import struct
"""
the Command between server and server web ui
"""
GetFreeDataChannel = 1
GetAllActiveClient = 2
GetOneActiveClient = 3
GetEstParam = 4
SetClientParamCommand = 5
GetServerInfoCom = 6
GetClientGenerateInfoCom = 7

GiveAllActiveClient = 101
GiveOneActiveClient = 102
GiveFreeDataChannel = 103
GiveEstParam = 104
GiveServerInfoCom = 105
GiveClientGenerateInfoCom = 106

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
def GetEstInformation(socket):
    content = struct.pack("!B", GetEstParam)
    socket.send(content)
    command = RecvFixLen(socket, 9)
    (command, averSerTime, inputrate) = struct.unpack("!Bff", command)
    return (averSerTime, inputrate)
    
def GetClientGenerateInfo(socket):
    content = struct.pack("!B", GetClientGenerateInfoCom)
    socket.send(content)
    content = RecvFixLen(socket, 9)
    (command, averSerTime, inputrate) = struct.unpack("!Bff", content)
    return (averSerTime, inputrate)
def GetServerInfo(socket):
    content = struct.pack("!B", GetServerInfoCom)
    socket.send(content)
    command = RecvFixLen(socket, 25)
    (command, maxFreq, leftFreq, nowSatis, activeNum, servedNum, inputtype) = struct.unpack("!BfffIII", command)
    return (maxFreq, leftFreq, nowSatis, activeNum, servedNum, inputtype)

def SetClientParam(socket, inputtype):
    content = struct.pack("!BB", SetClientParamCommand, inputtype)
    socket.send(content)
