#!/usr/bin/env python
from ServerControl import ServerControl


def main():
    server = ServerControl()
    server.Start()

    while True:
        command = raw_input("command:")
        if command == "end":
            server.Stop()
            break


if __name__ == "__main__":
    main()
