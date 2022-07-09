/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *lineEdit;
    QSpinBox *sender_port;
    QPushButton *pushButton;
    QSpinBox *receiver_port;
    QTextEdit *textEdit;
    QPushButton *sendingButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(470, 332);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 441, 257));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        lineEdit_2 = new QLineEdit(layoutWidget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        gridLayout->addWidget(lineEdit_2, 1, 0, 1, 1);

        lineEdit_3 = new QLineEdit(layoutWidget);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

        gridLayout->addWidget(lineEdit_3, 4, 1, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));

        gridLayout->addLayout(verticalLayout_3, 0, 5, 4, 1);

        lineEdit = new QLineEdit(layoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout->addWidget(lineEdit, 0, 0, 1, 1);

        sender_port = new QSpinBox(layoutWidget);
        sender_port->setObjectName(QString::fromUtf8("sender_port"));
        sender_port->setMaximum(999999);

        gridLayout->addWidget(sender_port, 0, 1, 1, 1);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 0, 4, 1, 1);

        receiver_port = new QSpinBox(layoutWidget);
        receiver_port->setObjectName(QString::fromUtf8("receiver_port"));
        receiver_port->setMaximum(9999999);

        gridLayout->addWidget(receiver_port, 1, 1, 1, 1);

        textEdit = new QTextEdit(layoutWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        gridLayout->addWidget(textEdit, 2, 1, 1, 1);

        sendingButton = new QPushButton(layoutWidget);
        sendingButton->setObjectName(QString::fromUtf8("sendingButton"));

        gridLayout->addWidget(sendingButton, 4, 4, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 470, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        lineEdit_2->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\273\321\203\321\207\320\260\321\202\320\265\320\273\321\214", nullptr));
        lineEdit->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\320\265\320\273\321\214", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\320\265\320\264\320\270\320\275\320\270\321\202\321\214", nullptr));
        sendingButton->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
