#!/bin/sh
alias arm-none-linux-gnueabi-gcc="color_compile arm-none-linux-gnueabi-gcc"
alias arm-none-linux-gnueabi-g++="color_compile arm-none-linux-gnueabi-++"
alias make="color_compile make"

export QMAKESPEC=/opt/sdk/qt4-arm/mkspecs/qws/linux-arm-gnueabi-g++
QMAKE=/opt/sdk/qt4-arm/bin/qmake

#make distclean
$QMAKE TWSDNetPay_App.pro -r CONFIG+=debug
#$QMAKE TWSDNetPay_App.pro
#make clean
make

scp TWSDNetPay_App root@192.168.101.156:/tmp/
