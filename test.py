import tcpserver
from ctypes import *

server = tcpserver.CSockServer(6050)
buf = "0"*1024

while True:
    ret,fd = server.readdata(buf, 1024)
    if ret <= 0:
        continue;

    ret = buf[:ret]
    print ret,fd

