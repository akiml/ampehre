/*
 * main.cpp
 *
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 * encoding: UTF-8
 * tab size: 4
 *
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation
 */


#include "gui/mainwindow.h"
#include <QApplication>
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc == 2){
        std::string s = argv[1];
        if(s != "-d" && s != "--debug")
        {
            std::cout<<"-d (--debug)\t\tdebug information"<< std::endl;
            return 1;
        }
    }else if (argc == 1){
        std::cout.setstate(std::ios_base::failbit);
    }
    else
        return 1;



    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
