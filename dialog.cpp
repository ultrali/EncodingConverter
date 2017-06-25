#include "dialog.h"
#include "ui_dialog.h"
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
	setWindowTitle(tr("Set File Filter"));
    connect(ui->btn_ok, &QPushButton::clicked, this, &Dialog::onClickedButtonOk);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &Dialog::onClickedButtonCancel);
}
Dialog::~Dialog()
{
    delete ui;
}
void Dialog::onClickedButtonOk()
{
    ui->lineEdit->text();
	this->close();
    emit sigfilter(ui->lineEdit->text());
}
void Dialog::onClickedButtonCancel()
{
	this->close();
}