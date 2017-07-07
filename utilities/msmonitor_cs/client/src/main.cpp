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
    int port = -1;
    if(argc > 1)
    {
        std::string s = argv[1];
        if(s != "-d" && s != "-p")
        {
            std::cout<<"-d \t\tdebug information"<< std::endl;
            std::cout<<"-p [port]\tspecify server port"<< std::endl;
            return 1;
        }
        else if( s == "-d")
        {
            if(argc > 3)
            {
                s = argv[2];
                if(s == "-p")
                {
                    s = argv[3];
                    port = atoi(s.c_str());
                }
            }
        }
        else if( s == "-p")
        {
            if(argc > 3)
            {
                s = argv[2];
                port = atoi(s.c_str());

                s = argv[3];
                if( s != "-d")
                {
                   std::cout.setstate(std::ios_base::failbit);
                }
            }
            else if(argc == 3)
            {
                s = argv[2];
                port = atoi(s.c_str());
                std::cout.setstate(std::ios_base::failbit);
            }
        }
    }else if (argc == 1)
    {
        std::cout.setstate(std::ios_base::failbit);
    }
    else
        return 1;



    QApplication a(argc, argv);
    MainWindow w(port);
    w.show();

    return a.exec();
}
