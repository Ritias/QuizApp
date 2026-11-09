#pragma once
#include <QToolButton>
#include <QLabel>
#include <QString>
#include <QWidget>

class Answer : public QWidget
{
    Q_OBJECT

public:
    explicit Answer(QWidget* parent = nullptr,
        int num = 0,
        const QString& front = QString(),
        const QString& back = QString()
        );
    void setNewAnswer(const QString& front, const QString& back);
    void setCorrect(bool isCorrect);
    void fitLabelText();

private:
    QToolButton *answerBtn;
    QLabel *btnLabel;
    QLabel *questionNumber;
    QString frontText;
    QString flipText;
    bool correct;
    bool answered = false;

    void flip();

    signals:
    void giveAns(bool correctly);
};
