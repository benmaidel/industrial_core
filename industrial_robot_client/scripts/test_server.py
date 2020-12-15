#!/usr/bin/env python

import socket
import sys
import time

import construct as c2

SM_Bool = c2.Int32sl
SM_Float = c2.Float32l
SM_Integer = c2.Int32sl

Header = c2.Struct(
    'msg_type' / SM_Integer ,
    'comm_type' / SM_Integer,
    'reply_type' / SM_Integer,
)

# TestMsgBody = c2.Struct(
#     'test' / SM_Float,
# )

# GenericBody = c2.Struct(
#     # use optional here, as body may be zero-length
#     c2.GreedyRange('data' / c2.Int8ub)
# )

# GenericMessage = c2.Struct(
#     'Header'/ Header,
#     'body' / c2.Renamed(GenericBody),
#     c2.Terminated
# )

# RobotStatus = c2.Struct(
#     'Header'/ Header,
#     'body' / c2.Renamed(TestMsgBody),
#     c2.Terminated,
# )

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
server_address = ('localhost', 11002)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()

    try:
        print >>sys.stderr, 'connection from', client_address

        value = 0.0
        # Receive the data in small chunks and retransmit it
        while True:
            
            #i = input("Continue:")


            SimpleMessage = c2.Struct(
                'Header' / c2.Struct(
                    'msg_type' / c2.Int32sl,
                    'comm_type' / c2.Int32sl,
                    'reply_type' / c2.Int32sl,
                ),
                'body' / c2.Struct(
                    'test' / c2.Float32l,
                ),
                c2.Terminated
            )
            msg = dict(
            Header=dict(msg_type=999, comm_type=2, reply_type=0),
            body=dict(test=value))
            data = SimpleMessage.build(msg)
            #print(feedback_data)
            data_len = c2.Int32sl.build(len(data))
            print('sending: {}'.format(value))
            #self.transport.write(data_len + data)
            connection.send(data_len + data)

            data = connection.recv(4)
            data_len = int(data[0].encode('hex'), 32)
            print >>sys.stderr, 'received {}'.format(data_len)
            data = connection.recv(data_len)
            print >>sys.stderr, 'received {}'.format(data)

            header = SimpleMessage.parse(data)
            print("header: {}".format(header))

            value = value + 0.01
            time.sleep(1)
    finally:
        # Clean up the connection
        connection.close()