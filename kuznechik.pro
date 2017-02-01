TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    gost_3412_2015_calc.c

HEADERS += \
    gost_3412_2015_const.h \
    gost_3412_2015_calc.h \
    test.h
