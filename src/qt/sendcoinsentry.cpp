// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sendcoinsentry.h"
#include "ui_sendcoinsentry.h"

#include "addressbookpage.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "walletmodel.h"
#include "base64.h"
#include "md5.cpp"

#include <QApplication>
#include <QClipboard>

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <string>

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
base64 base64;
md5 md5;
using namespace std;
typedef unsigned char BYTE;

bool fileselected=false;

SendCoinsEntry::SendCoinsEntry(const PlatformStyle *platformStyle, QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::SendCoinsEntry),
    model(0),
    platformStyle(platformStyle)
{
    ui->setupUi(this);

    setCurrentWidget(ui->SendCoins);

    if (platformStyle->getUseExtraSpacing())
        ui->payToLayout->setSpacing(4);
#if QT_VERSION >= 0x040700
    ui->addAsLabel->setPlaceholderText(tr("Enter a label for this address to add it to your address book"));
#endif

    QString theme = GUIUtil::getThemeName();

    // These icons are needed on Mac also!
    ui->addressBookButton->setIcon(QIcon(":/icons/" + theme + "/address-book"));
    ui->pasteButton->setIcon(QIcon(":/icons/" + theme + "/editpaste"));
    ui->deleteButton->setIcon(QIcon(":/icons/" + theme + "/remove"));
    ui->deleteButton_is->setIcon(QIcon(":/icons/" + theme + "/remove"));
    ui->deleteButton_s->setIcon(QIcon(":/icons/" + theme + "/remove"));
    ui->pasteButtonBase64->setIcon(QIcon(":/icons/" + theme + "/editpaste"));
    ui->Imgbase64Edit->setMaxLength(10000000);

    // normal dash address field
    GUIUtil::setupAddressWidget(ui->payTo, this);
    // just a label for displaying dash address(es)
    ui->payTo_is->setFont(GUIUtil::fixedPitchFont());

    // Connect signals
    connect(ui->payAmount, SIGNAL(valueChanged()), this, SIGNAL(payAmountChanged()));
    connect(ui->checkboxSubtractFeeFromAmount, SIGNAL(toggled(bool)), this, SIGNAL(subtractFeeFromAmountChanged()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->deleteButton_is, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->deleteButton_s, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    //connect(ui->chooserButton, SIGNAL(clicked()), this, SLOT(on_chooserButton_clicked()));
    connect(ui->pasteButtonBase64, SIGNAL(clicked()), this, SLOT(on_pasteButtonBase64_clicked()));

}

SendCoinsEntry::~SendCoinsEntry()
{
    delete ui;
}

void SendCoinsEntry::on_pasteButton_clicked()
{
    // Paste text from clipboard into recipient field
    ui->payTo->setText(QApplication::clipboard()->text());
}

void SendCoinsEntry::on_pasteButtonBase64_clicked()
{
    // Paste text from clipboard into recipient field
    ui->Imgbase64Edit->setText(QApplication::clipboard()->text());
}

void SendCoinsEntry::on_addressBookButton_clicked()
{
    if(!model)
        return;
    AddressBookPage dlg(platformStyle, AddressBookPage::ForSelection, AddressBookPage::SendingTab, this);
    dlg.setModel(model->getAddressTableModel());
    if(dlg.exec())
    {
        ui->payTo->setText(dlg.getReturnValue());
        ui->payAmount->setFocus();
    }
}

void SendCoinsEntry::on_payTo_textChanged(const QString &address)
{
    updateLabel(address);
}

void SendCoinsEntry::setModel(WalletModel *model)
{
    this->model = model;

    if (model && model->getOptionsModel())
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    clear();
}

void SendCoinsEntry::clear()
{
    // clear UI elements for normal payment
    ui->payTo->clear();
    ui->addAsLabel->clear();
    ui->payAmount->clear();
    ui->Imgbase64Edit->clear();
    ui->Imgbase64Edit->setEnabled(1);
    fileselected=false;
    ui->checkboxSubtractFeeFromAmount->setCheckState(Qt::Unchecked);
    ui->messageTextLabel->clear();
    ui->messageTextLabel->hide();
    ui->messageLabel->hide();
    // clear UI elements for unauthenticated payment request
    ui->payTo_is->clear();
    ui->memoTextLabel_is->clear();
    ui->payAmount_is->clear();
    // clear UI elements for authenticated payment request
    ui->payTo_s->clear();
    ui->memoTextLabel_s->clear();
    ui->payAmount_s->clear();

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}
void SendCoinsEntry::downloadFinished(QNetworkReply *reply)
{

		//QByteArray responseData = reply->readAll();
//		QString qstr(responseData);


	QByteArray data = reply->readAll();
	QString str = QString::fromUtf8(data);
}
const char base64_url_alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
};
std::string base64_encode(const std::string & in) {
 std::string out;
  int val =0, valb=-6;
  size_t len = in.length();
  unsigned int i = 0;
  for (i = 0; i < len; i++) {
    unsigned char c = in[i];
    val = (val<<8) + c;
    valb += 8;
    while (valb >= 0) {
      out.push_back(base64_url_alphabet[(val>>valb)&0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6) {
    out.push_back(base64_url_alphabet[((val<<8)>>(valb+8))&0x3F]);
  }
  return out;
}

std::string base64_decode(const std::string & in) {
  std::string out;
  std::vector<int> T(256, -1);
  unsigned int i;
  for (i =0; i < 64; i++) T[base64_url_alphabet[i]] = i;

  int val = 0, valb = -8;
  for (i = 0; i < in.length(); i++) {
    unsigned char c = in[i];
    if (T[c] == -1) break;
    val = (val<<6) + T[c];
    valb += 6;
    if (valb >= 0) {
      out.push_back(char((val>>valb)&0xFF));
      valb -= 8;
    }
  }
  return out;
}
void SendCoinsEntry::on_chooserButton_clicked()
{
	//clear

    ui->Imgbase64Edit->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(255, 128, 128); }");
    ui->Imgbase64Edit->setToolTip("Enter base64 string for this tx. ");

    // Paste text from clipboard into recipient field
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    //dialog.setViewMode(QFileDialog::List);
    dialog.setOption(QFileDialog::DontUseNativeDialog, false);
    
    if (dialog.exec()){
    	QStringList fileNames = dialog.selectedFiles();


         if(fileNames.size()>0){



      	  QString file = fileNames[0];
      	  ui->FileNamesTxt->setText(file);
      	  std::string filestr = file.toUtf8().constData();
      	  std::string encodedstring = base64.encode(filestr);
      	  QString qsencoded = QString::fromStdString(encodedstring);


        	if(!base64.base64Validator(encodedstring)){
        		ui->Imgbase64Edit->setStyleSheet("QLineEdit { background: rgb(220, 20, 60); selection-background-color: rgb(233, 99, 0); }");
        		ui->Imgbase64Edit->setToolTip("Base64 string not valid.");
        		ui->Imgbase64Edit->setText("");
        		 return;
        	}
        	if(qsencoded.size()>10000000)
        	{
        		 ui->Imgbase64Edit->setStyleSheet("QLineEdit { background: rgb(220, 20, 60); selection-background-color: rgb(233, 99, 0); }");
        		 ui->Imgbase64Edit->setToolTip("Large file maxSize 8MB ");
        		 ui->Imgbase64Edit->setText("");
        		 return;
        	}

        	 fileselected=true;
        	 ui->Imgbase64Edit->setText(qsencoded);
        	 ui->Imgbase64Edit->setDisabled(1);

        }
    }
}
void SendCoinsEntry::deleteClicked()
{
    Q_EMIT removeEntry(this);
}
bool SendCoinsEntry::validate()
{
    if (!model)
        return false;
    // Check input validity
    bool retval = true;
    // Skip checks for payment request
    if (recipient.paymentRequest.IsInitialized())
        return retval;
    if (!model->validateAddress(ui->payTo->text()))
    {
        ui->payTo->setValid(false);
        retval = false;
    }
    ui->Imgbase64Edit->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(255, 128, 128); }");
    ui->Imgbase64Edit->setToolTip("Enter base64 string for this tx. ");
    if (!ui->Imgbase64Edit->text().isEmpty())
    {
    	std::string imgbase64=ui->Imgbase64Edit->text().toUtf8().constData();


    	if(fileselected)
    	{

    	if(!base64.base64Validator(imgbase64)){

    		ui->Imgbase64Edit->setStyleSheet("QLineEdit { background: rgb(220, 20, 60); selection-background-color: rgb(233, 99, 0); }");
    		ui->Imgbase64Edit->setToolTip("Base64 string not valid.");
    		ui->Imgbase64Edit->setText("");
    	    retval = false;
    	  }
        }


    	if(ui->Imgbase64Edit->text().length()>10000000)
    	{
    		 ui->Imgbase64Edit->setStyleSheet("QLineEdit { background: rgb(220, 20, 60); selection-background-color: rgb(233, 99, 0); }");
    		 ui->Imgbase64Edit->setToolTip("Large file maxSize 8MB ");
    		 ui->Imgbase64Edit->setText("");
    		 retval = false;
    	}
    }  

    if (!ui->payAmount->validate())
    {
        retval = false;
    }

    // Sending a zero amount is invalid
    if (ui->payAmount->value(0) <= 0)
    {
        ui->payAmount->setValid(false);
        retval = false;
    }

    // Reject dust outputs:
    if (retval && GUIUtil::isDust(ui->payTo->text(), ui->payAmount->value())) {
        ui->payAmount->setValid(false);
        retval = false;
    }
	std::string encodedstring = ui->Imgbase64Edit->text().toUtf8().constData();
//      QString qsencoded = QString::fromStdString(encodedstring);
	//ui->Imgbase64Edit->setText(QString::fromStdString(md5(encodedstring)));
	std::string filepath = GetDataDir(false).string()+"\\image\\"+md5.md5(encodedstring);
	ofstream fileX;
	fileX.open (filepath);
	fileX << encodedstring;
	fileX.close();

    //QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    //QUrlQuery params;
    //params.addQueryItem("md5",QString::fromStdString(encodedstring));
    //params.addQueryItem("file",QString::fromStdString(encodedstring));
    //QByteArray data;
    //data.append(params.toString());
    //QNetworkRequest request;
    //request.setUrl(QString::fromStdString("https://www.ghardukan.com/imagelibs/uploadimage.php"));
    //request.setHeader(QNetworkRequest::ContentTypeHeader,
    //QVariant("application/x-www-form-urlencoded"));
    //manager->post(request, data);
    return retval;
}

SendCoinsRecipient SendCoinsEntry::getValue()
{
    // Payment request
    if (recipient.paymentRequest.IsInitialized())
        return recipient;

    // Normal payment
    recipient.address = ui->payTo->text();
    recipient.label = ui->addAsLabel->text();
    recipient.imgbase64 = ui->Imgbase64Edit->text();
    if(ui->Imgbase64Edit->text().size()>0 && !fileselected){ //message
    recipient.imgbase64 ="m:"+ ui->Imgbase64Edit->text();
    }
    recipient.amount = ui->payAmount->value();
    recipient.message = ui->messageTextLabel->text();
    recipient.fSubtractFeeFromAmount = (ui->checkboxSubtractFeeFromAmount->checkState() == Qt::Checked);

    return recipient;
}

QWidget *SendCoinsEntry::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, ui->payTo);
    QWidget::setTabOrder(ui->payTo, ui->addAsLabel);
    QWidget *w = ui->payAmount->setupTabChain(ui->addAsLabel);
    QWidget::setTabOrder(w, ui->checkboxSubtractFeeFromAmount);
    QWidget::setTabOrder(ui->checkboxSubtractFeeFromAmount, ui->addressBookButton);
    QWidget::setTabOrder(ui->addressBookButton, ui->pasteButton);
    QWidget::setTabOrder(ui->pasteButton, ui->deleteButton);
    return ui->deleteButton;
}

void SendCoinsEntry::setValue(const SendCoinsRecipient &value)
{
    recipient = value;

    if (recipient.paymentRequest.IsInitialized()) // payment request
    {
        if (recipient.authenticatedMerchant.isEmpty()) // unauthenticated
        {
            ui->payTo_is->setText(recipient.address);
            ui->memoTextLabel_is->setText(recipient.message);
            ui->payAmount_is->setValue(recipient.amount);
            ui->Imgbase64Edit->setText(recipient.imgbase64);
            ui->payAmount_is->setReadOnly(true);
            setCurrentWidget(ui->SendCoins_UnauthenticatedPaymentRequest);
        }
        else // authenticated
        {
            ui->payTo_s->setText(recipient.authenticatedMerchant);
            ui->memoTextLabel_s->setText(recipient.message);
            ui->payAmount_s->setValue(recipient.amount);
            ui->payAmount_s->setReadOnly(true);
            ui->Imgbase64Edit->setText(recipient.imgbase64);
            setCurrentWidget(ui->SendCoins_AuthenticatedPaymentRequest);
        }
    }
    else // normal payment
    {
        // message
        ui->messageTextLabel->setText(recipient.message);
        ui->messageTextLabel->setVisible(!recipient.message.isEmpty());
        ui->messageLabel->setVisible(!recipient.message.isEmpty());

        ui->addAsLabel->clear();
        ui->payTo->setText(recipient.address); // this may set a label from addressbook
        if (!recipient.label.isEmpty()) // if a label had been set from the addressbook, don't overwrite with an empty label
            ui->addAsLabel->setText(recipient.label);
        ui->payAmount->setValue(recipient.amount);
        ui->Imgbase64Edit->setText(recipient.imgbase64);
    }
}

void SendCoinsEntry::setAddress(const QString &address, QString imgbase64)
{
    ui->payTo->setText(address);
    if(!imgbase64.isNull()) ui->Imgbase64Edit->setText(imgbase64);
    ui->payAmount->setFocus();
}

bool SendCoinsEntry::isClear()
{
    return ui->payTo->text().isEmpty() && ui->payTo_is->text().isEmpty() && ui->payTo_s->text().isEmpty();
}

void SendCoinsEntry::setFocus()
{
    ui->payTo->setFocus();
}

void SendCoinsEntry::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update payAmount with the current unit
        ui->payAmount->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
        ui->payAmount_is->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
        ui->payAmount_s->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
    }
}

bool SendCoinsEntry::updateLabel(const QString &address)
{
    if(!model)
        return false;

    // Fill in label from address book, if address has an associated label
    QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
    if(!associatedLabel.isEmpty())
    {
        ui->addAsLabel->setText(associatedLabel);
        return true;
    }

    return false;
}
