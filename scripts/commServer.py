#!/usr/bin/python

# na bazie skryptu Marcina
# Skrypt do testowania; udaje on sensor, komunikujacy sie po TCP
# odbiera ramke od programu i wypisuje ja

def ByteToHex( byteStr ):
    """
    Convert a byte string to it's hex string representation e.g. for output.
    """   
    return ''.join( [ "%02X " % ord( x ) for x in byteStr ] ).strip()

import socket

Host, Port = "127.0.0.1", 8800

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((Host, Port))
s.listen(1)
conn, addr = s.accept()
print "Polaczono z:", addr

data = conn.recv(14)
print "Odebrano od telemetrona: "
print ByteToHex(data)

print "Koniec"
conn.close()

