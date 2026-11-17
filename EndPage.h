#pragma once

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QList>

#include "results.h"

class QResizeEvent;
class QPushButton;
class QTableWidget;

class EndPage : public QWidget
{
    Q_OBJECT

public:
    explicit EndPage(QWidget *parent = nullptr);
    void setResults(const QVector<Result> &results, const QList<int> &selectedQuizzes);

signals:
    void exitApplication();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *titleLabel = nullptr;
    QLabel *subtitleLabel = nullptr;
    QTableWidget *resultsTable = nullptr;
    QPushButton *finishButton = nullptr;
};
