#!/usr/bin/env python
from ServerControl import ServerControl

def main():
    server = ServerControl()
    server.Start()

    try:
        while True:
            command = raw_input("command:")
            if command == "end":
                server.Stop()
                break
            elif command == "help":
                print "end: stop the server"
                print "help: show this message"
                pass
    except KeyboardInterrupt:
        server.Stop()


if __name__ == "__main__":
    main()
