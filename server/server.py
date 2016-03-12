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
                global MEMORY
                if command == 0:
                    MEMORY.append(data)
                    self.request.sendall('\x67')
                elif command == 255:
                    MEMORY.append(data)
                    print 'I have recieved file!'
                    global COUNTER
                    COUNTER += 1
                    self.mem_dump()
                    self.request.sendall('\x67')
                else:
                    print 'Error!'
            else:
                print hex(ord(self.data[0]))
                print 'Package  is not true!'
                self.request.sendall('\xEE')

    def mem_dump(self):
        global MEMORY
        with open("image" + str(COUNTER) + ".jpg", 'w') as dump_file:
            for data in MEMORY:
                dump_file.write(data)
        MEMORY = []

    def getCommand(self):
        com = bin( ord( self.data[1] ) )
        return int(com, 2) # from binary string to int

    def getData(self):
        return self.data[2:]


if __name__ == "__main__":
    try:
        ip = sys.argv[1]
    except:
        print 'Первым аргументом должен быть IP адрес сервера'
        exit(1)
    try:
        port = sys.argv[2]
    except:
        print 'Вторым аргументом должен быть порт, на котором будет работать сервер!'
        exit(1)
    HOST, PORT = ip, int(port)
    server = SocketServer.TCPServer((HOST, PORT), TCPHandler)
    server.serve_forever()
