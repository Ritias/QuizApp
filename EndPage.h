#pragma once

#include <QWidget>
#include <QLabel>

class QPushButton;
class QResizeEvent;

class EndPage : public QWidget {
    Q_OBJECT
public:
    explicit EndPage(QWidget *parent = nullptr);

signals:
    void goToStart();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QPushButton *menuButton;
};
