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
