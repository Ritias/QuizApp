#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QList>

class QResizeEvent;

class StartPage : public QWidget {
    Q_OBJECT
public:
    explicit StartPage(QWidget *parent = nullptr);

signals:
    void goToQuiz(const QString &file);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *eyebrow;
    QLabel *label;
    QLabel *subtitle;
    QList<QPushButton*> quizButtons;
};
