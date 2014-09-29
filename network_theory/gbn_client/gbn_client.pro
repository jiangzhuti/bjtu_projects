TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    gobackn_protocol.h \

SOURCES += \
    gobackn_protocol.cpp \

HEADERS += \
    shell_client.h

SOURCES += \
    shell_client.cpp

HEADERS += \
    udp_connection/udp_connection.h \
    udp_connection/common.h \
    udp_connection/client_udpconnection.h

SOURCES += \
    udp_connection/udp_connection.cpp \
    udp_connection/client_udpconnection.cpp

