#pragma once

#include <QWidget>
#include <QString>

class StarWarsCrawl;
class QPushButton;

class CrawlPage : public QWidget
{
    Q_OBJECT

public:
    explicit CrawlPage(const QString &crawlHtml,
                       const QString &buttonText,
                       QWidget *parent = nullptr);

signals:
    void proceed();

private:
    StarWarsCrawl *crawl_;
    QPushButton *button_;
};
