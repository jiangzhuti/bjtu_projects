TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    gobackn_protocol.h \

SOURCES += \
    gobackn_protocol.cpp

HEADERS += \
    udp_connection/udp_connection.h \
    udp_connection/common.h \

SOURCES += \
    udp_connection/udp_connection.cpp \

HEADERS += \
    shell_server.h

SOURCES += \
    shell_server.cpp

HEADERS += \
    udp_connection/server_udpconnection.h

SOURCES += \
    udp_connection/server_udpconnection.cpp
