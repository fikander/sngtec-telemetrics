#!/usr/bin/python

# Skrypt do testowania; udaje on sensor, komunikujacy sie po TCP
# wysyla wartosc 42 (czyli '*') w ramce jako wartosc typu Value po czym czeka 5s
# i tak w petli 10 razy

def HexToByte( hexStr ):
    """
    Convert a string hex byte values into a byte string. The Hex Byte values may
    or may not be space separated.
    """
    bytes = []

    hexStr = ''.join( hexStr.split(" ") )

    for i in range(0, len(hexStr), 2):
        bytes.append( chr( int (hexStr[i:i+2], 16 ) ) )

    return ''.join( bytes )

import socket
import time

Host, Port = "127.0.0.1", 8800

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((Host, Port))
s.listen(1)
conn, addr = s.accept()
print "Polaczono z: ", addr

data = "FF FF 01 02 03 02 04 06 06 2A 00 00 00 FE"
print "Wysylam do telemetrona: "
print data

for i in xrange(10):
    n = conn.send(HexToByte(data))
    print "Sent (bytes): ", n
    time.sleep(5)

print "Koniec"
conn.close()

