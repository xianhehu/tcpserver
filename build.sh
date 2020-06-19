g++ --shared -fPIC source/tcpserver.cpp -o libtcpserver_c.so
g++ --shared -fPIC tcpserver_wrap.cxx -L. -ltcpserver_c -lpython2.7 -I/usr/include/python2.7/ -o _tcpserver.so
