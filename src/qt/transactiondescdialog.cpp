// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "guiutil.h"
#include "transactiondescdialog.h"
#include "ui_transactiondescdialog.h"

#include "transactiontablemodel.h"
#include "base64.h"
#include <QModelIndex>
#include <QSettings>
#include <QString>
#include <QByteArray>
#include <QImage>
#include <QSize>
#include <QBuffer>
#include <QImageReader>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <string>
#include <vector>




TransactionDescDialog::TransactionDescDialog(const QModelIndex &idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    ui->graphicsView->setVisible(false);
    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    QString desc = idx.data(TransactionTableModel::LongDescriptionRole).toString();
    ui->detailText->setHtml(desc);

    /* Start ImageView */
    QString encodeqstring = idx.data(TransactionTableModel::Imgbase64Role).toString();
     std::string  encodestr = encodeqstring.toUtf8().constData();
   
    
    if(encodestr.size()>0)
         	{
			base64 base64trdialog;
             std::vector<unsigned char> bytesarray = base64trdialog.decode(encodestr);

             if(bytesarray.size()>0)
                     	{
                  QByteArray *base64decodearray;
     		  base64decodearray = new QByteArray(reinterpret_cast<const char*>(bytesarray.data()), bytesarray.size());


     		  QBuffer buffer(base64decodearray);
     		  QImageReader reader(&buffer);
     		  QImage image = reader.read();
                     QPixmap imagePixmap =  QPixmap::fromImage(image);
                     QPixmap newPixmap = imagePixmap.scaled(QSize(400,250),  Qt::KeepAspectRatio);

     		  QGraphicsScene* scene = new QGraphicsScene(QRect(0, 0, 400, 250));

     		   scene->addPixmap(newPixmap);


     		   ui->graphicsView->setScene( scene );
                      ui->graphicsView->setVisible(true);
                      ui->graphicsView->setGeometry(QRect(0, 0, 400, 250));
     		   ui->graphicsView->show();
     		 

                     	}
     	}

}

TransactionDescDialog::~TransactionDescDialog()
{
    delete ui;
}
