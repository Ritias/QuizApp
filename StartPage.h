#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QList>

class QResizeEvent;

class StartPage : public QWidget {
    Q_OBJECT
public:
    explicit StartPage(const QList<int> &availableQuizzes, QWidget *parent = nullptr);
    void markQuizCompleted(int quizNumber);

signals:
    void goToQuiz(int, const QString &file);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *eyebrow;
    QLabel *label;
    QLabel *subtitle;
    QList<QPushButton*> quizButtons;
};
