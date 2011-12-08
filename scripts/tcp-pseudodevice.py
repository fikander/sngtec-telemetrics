#!/usr/bin/python

# Skrypt do testowania; udaje on 'czujnik' komunikujacy sie po TCP
# Laczy sie z programem, po czym 10 krotnie
# - wysyla stringa 'test'
# - oczekuje na komunikat od programu i wypisuje go

import socket

Host, Port = "127.0.0.1", 7171

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((Host, Port))
s.listen(1)
conn, addr = s.accept()
print "Polaczono z:", addr

for i in xrange(10):
  sent = conn.send("test")
  print "Wyslano: ", sent, "bajty"

  data = conn.recv(2)
  print "Odebrano od telemetrona:", data

print "Koniec"
conn.close()

