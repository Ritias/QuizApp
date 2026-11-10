#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>
#include<QPushButton>

class EndPage : public QWidget {
    Q_OBJECT
public:
    explicit EndPage(QWidget *parent = nullptr);
private:
    QLabel *label;
signals:
    void goToStart();
};