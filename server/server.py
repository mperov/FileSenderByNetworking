#!/usr/bin/env python
# -*- coding: utf-8 -*-

import SocketServer
import sys

MEMORY = []
COUNTER = 0x0

class TCPHandler(SocketServer.BaseRequestHandler):
    """
    """
    def __init__(self, *kargs):
        SocketServer.BaseRequestHandler.__init__(self, *kargs)

    def handle(self):
        while True:
            self.data = self.request.recv(256)
            if not self.data:
                break
            # parsing
            if ord(self.data[0]) == 0x5A:
                command = self.getCommand()
                data = self.getData()
                if command == 0:
                    MEMORY.append(data)
                elif command == 255:
                    MEMORY.append(data)
                    print 'I have recieved file!'
                    global COUNTER
                    COUNTER += 1
                    self.mem_dump()
                else:
                    print 'Error!'
            else:
                print hex(ord(self.data[0]))
                print 'Package is not true!'
                break

    def mem_dump(self):
        with open("image" + str(COUNTER) + ".jpg", 'w') as dump_file:
            for data in MEMORY:
                dump_file.write(data)

    def getCommand(self):
        com = bin( ord( self.data[1] ) )
        return int(com, 2) # from binary string to int

    def getData(self):
        return self.data[2:]


if __name__ == "__main__":
    try:
        port = sys.argv[1]
    except:
        print 'Первым аргументом должен быть порт, на котором будет работать сервер!'
        exit(1)
    HOST, PORT = "localhost", int(port)
    server = SocketServer.TCPServer((HOST, PORT), TCPHandler)
    server.serve_forever()
