#include <QVBoxLayout>
#include <QLabel>

#include "exceptionhandler.h"

ExceptionHandler::ExceptionHandler(QWidget *parent)
        : QDialog(parent)
{
   // setStyleSheet("background: rgba(51,51,51,95%); border-style: solid; border-width: 1px; border-radius: 2px; border-color:red;");
    setStyleSheet("background-color: none; background-image: url(:/files/dialogbg.png);");
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedWidth(550);
    setFixedHeight(200);
    QLabel * label = new QLabel ("An Exception!",0);
    label->setStyleSheet("*{ color:red; font-size 20px;}");

    QVBoxLayout * layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(label);
    setModal(true);
}
