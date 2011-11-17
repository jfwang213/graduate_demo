def RecvFixLen(sock, length):
    res = ''
    while length > 0:
        content = sock.recv(length);
        length -= len(content)
        res += content
    return res
