#ifndef DBCONNECT_H
#define DBCONNECT_H

#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>

inline bool create_connection(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7");
    db.setDatabaseName("practicechat");
    db.setUserName("postgres");
    db.setPassword("jh02a2345");
    if (db.open()){
        return true;
    } else {
        QMessageBox::warning(0, "Ошибка", db.lastError().text());
        return false;
    }
}

#endif // DBCONNECT_H
