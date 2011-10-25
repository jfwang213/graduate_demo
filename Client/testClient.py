#!/usr/bin/env python
from ClientControl import ClientControl

def main():
    client = ClientControl(2)
    client.start()
    while True:
        content = raw_input("command:")
        if content == 'send':
           client.sendFreqReq()
        elif content == 'end':
            break
    client.stop()


if __name__ == '__main__':
    main()
