#pragma once

#include <QWidget>
#include <QLabel>
#include <QVector>
#include "results.h"

class QPushButton;
class QResizeEvent;

class EndPage : public QWidget {
    Q_OBJECT
public:
    explicit EndPage(QWidget *parent = nullptr);
    void setResults(const QVector<Result> &results);

signals:
    void goToStart();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void showResults();
    QVector<Result> quizResults;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QPushButton *resultsButton;
    QPushButton *menuButton;
};
