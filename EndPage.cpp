#include "EndPage.h"
#include "StarWarsCrawl.h"
#include <QVBoxLayout>

EndPage::EndPage(QWidget *parent) {
    auto * layout = new QVBoxLayout(this);
    auto * label = new QLabel;
    label->setText("abacaba");

    layout->addWidget(label);
    auto * swc = new StarWarsCrawl();
    layout->addWidget(swc);
}
