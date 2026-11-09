#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>

class EndPage : public QWidget {
    Q_OBJECT
public:
    explicit EndPage(QWidget *parent = nullptr);
private:
    QLabel *label;
};