FORMS += \
    ../src/qt/forms/aboutdialog.ui \
    ../src/qt/forms/addressbookpage.ui \
    ../src/qt/forms/askpassphrasedialog.ui \
    ../src/qt/forms/coincontroldialog.ui \
    ../src/qt/forms/darksendconfig.ui \
    ../src/qt/forms/debugwindow.ui \
    ../src/qt/forms/editaddressdialog.ui \
    ../src/qt/forms/encryptdecryptdialog.ui \
    ../src/qt/forms/helpmessagedialog.ui \
    ../src/qt/forms/intro.ui \
    ../src/qt/forms/masternodelist.ui \
    ../src/qt/forms/openuridialog.ui \
    ../src/qt/forms/optionsdialog.ui \
    ../src/qt/forms/overviewpage.ui \
    ../src/qt/forms/receivecoinsdialog.ui \
    ../src/qt/forms/receiverequestdialog.ui \
    ../src/qt/forms/sendcoinsdialog.ui \
    ../src/qt/forms/sendcoinsentry.ui \
    ../src/qt/forms/signverifymessagedialog.ui \
    ../src/qt/forms/transactiondescdialog.ui \
    ../src/qt/forms/webwindow.ui

RESOURCES += \
    ../src/qt/imagecoin.qrc


HEADERS += \
    ../src/qt/encryptdecryptdialog.h \
    ../src/qt/webwindow.h \
    ../src/qt/addressbookpage.h \
   ../src/amount.h


SOURCES += \
 ../src/qt/encryptdecryptdialog.cpp \
  ../src/qt/webwindow.cpp \
 ../src/qt/addressbookpage.cpp \
 ../src/amount.cpp

# Input
DEPENDPATH += \
../src/qt \
../src/

INCLUDEPATH += \
  $$PWD ../src \
  E:/boost/boost_1_61_0/ \
  E:/protobuf/

QT += widgets
QT += multimedia
QT += multimediawidgets
QT += webkit
QT += webkitwidgets
QT += webenginewidgets

win32:QMAKE_LFLAGS += -shared

LIBS += "-LE:/boost/boost_1_61_0/stage/lib/"

INCLUDEPATH += E:\Qt\5.9.3\
