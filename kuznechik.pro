TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    gost_3412_2015_calc.c \
    main.c

HEADERS += \
    gost_3412_2015_calc.h \
    gost_3412_2015_const.h \
    test.h
