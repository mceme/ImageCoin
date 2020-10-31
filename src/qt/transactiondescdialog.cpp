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
#include <QIODevice>
#include <QImageReader>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QMovie>
#include <QGraphicsProxyWidget>
#include <QFileDialog>
#include <string>
#include <vector>
#include <stdio.h>


 std::string  encodestr;

TransactionDescDialog::TransactionDescDialog(const QModelIndex &idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    ui->graphicsView->setVisible(false);
    ui->DownloadButton->setVisible(false);
    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    QString desc = idx.data(TransactionTableModel::LongDescriptionRole).toString();
    ui->detailText->setHtml(desc);

    /* Start ImageView */
    QString encodeqstring = idx.data(TransactionTableModel::Imgbase64Role).toString();
     encodestr = encodeqstring.toUtf8().constData();

   std::string extension = "png";
    float delctype = 0;



    if(encodestr.size()>2)
         	{

    	std::string ismessagetpye2 = encodestr.substr(0, 2);
    	std::string ismessagetpye5 = encodestr.substr(0, 5);

    	 if (ismessagetpye5 == "mimg:") //message and img
        	{




    		 std::string delimiter = "mimg:";
    		 std::string delimiter2 = "[img]:";

    		 std::string mimg = encodestr.substr(5, encodestr.length());// remove mimg:

    		 std::vector<std::string> arrmimg;

    		 arrmimg = this->split(mimg,delimiter2);

    		 std::string message = arrmimg[0].c_str();


    		 desc = desc + "<br><b><b>"+message.c_str();
    		 ui->detailText->setHtml(desc);

    		 std::string imgbase64 = arrmimg[1].c_str();

    		 encodestr=imgbase64;
        	}

    	if(ismessagetpye2 == "m:" || ismessagetpye5 == "from:")
        	 { //message

        		 ui->DownloadButton->setVisible(false);
        		 desc = desc + "<br><b><b>"+encodestr.c_str();
        		 ui->detailText->setHtml(desc);
        	}
    	else{

    	ui->DownloadButton->setVisible(true);

    	std::string typebase64 = encodestr.substr(0, 1);
             if(typebase64=="J" /*pdf*/ || typebase64=="V"){
            	 delctype = 2;
             }
             else if(typebase64=="A" /*mp4*/ || typebase64=="R" /*gif*/|| typebase64=="U" /*avi*/ || typebase64=="S" /*mp3*/)
             {
            	 delctype = 1;
             }

	 base64 base64trdialog;

             std::vector<unsigned char> bytesarray = base64trdialog.decode(encodestr);

             if(bytesarray.size()>0)
                     	{

              QByteArray *base64decodearray;
     		  base64decodearray = new QByteArray(reinterpret_cast<const char*>(bytesarray.data()), bytesarray.size());


     		  QBuffer buffer(base64decodearray);
     		 QGraphicsScene* scene = new QGraphicsScene(QRect(0, 0, 400, 250));
     		   /* Start ImageView */
     		  if(delctype==0){

     		  QImageReader reader(&buffer);
     		  QImage image = reader.read();
              QPixmap imagePixmap =  QPixmap::fromImage(image);
              QPixmap newPixmap = imagePixmap.scaled(QSize(400,250),  Qt::KeepAspectRatio);

     		  scene->addPixmap(newPixmap);

     		  ui->graphicsView->setScene( scene );
              ui->graphicsView->setVisible(true);
              ui->graphicsView->setGeometry(QRect(0, 0, 400, 250));
     		  ui->graphicsView->show();
     		 
     		             }
     		   /* Start MovieView */
     		  else if(delctype==1){
		ui->graphicsView->setVisible(true);
     		              ui->graphicsView->setGeometry(QRect(0, 0, 400, 250));


     		 QLabel *gif_anim = new QLabel();
 		 gif_anim->setGeometry(0, 0, 400, 250);
     		  QBuffer buffermov(base64decodearray);
                buffermov.open(QIODevice::ReadOnly);
		buffermov.seek(0);
                QMovie *movie = new QMovie(&buffermov);
                movie->setScaledSize(QSize(400, 250));
     		 gif_anim->setMovie(movie);
     		 movie->start();
     		 QGraphicsProxyWidget *proxy = scene->addWidget(gif_anim);
                  scene->addWidget(gif_anim);
     		              ui->graphicsView->setScene( scene );
     		             ui->graphicsView->show();
     		       }
    		   /* Start Document */
     		  else if (delctype==2)
    		  {
    			  encodestr = "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";
    		      std::vector<unsigned char> bytesarraydoc = base64trdialog.decode(encodestr);

    			  QByteArray *base64decodearraydoc;
    			  base64decodearraydoc = new QByteArray(reinterpret_cast<const char*>(bytesarraydoc.data()), bytesarraydoc.size());
                  QBuffer bufferdoc(base64decodearraydoc);
		  QImageReader reader(&bufferdoc);
    		  QImage image = reader.read();
             QPixmap imagePixmap =  QPixmap::fromImage(image);
             QPixmap newPixmap = imagePixmap.scaled(QSize(400,250),  Qt::KeepAspectRatio);
    		  scene->addPixmap(newPixmap);
    		  ui->graphicsView->setScene( scene );
                  ui->graphicsView->setVisible(true);
                  ui->graphicsView->setGeometry(QRect(0, 0, 400, 250));
    		  ui->graphicsView->show();

    		   }
            }
     	}  //end image


      }
}

void TransactionDescDialog::on_DownloadButton_clicked()
   {


	     std::string delctype = "Files (*.png *.jpeg *.jpg *.gif *.tiff *.bmp)";

	     std::string savefile = "c:/image.png";



	    std::string typebase64 = encodestr.substr(0, 1);



         if(typebase64=="J" /*pdf*/ ){
        	 delctype = "Files (*.pdf)";
        	  savefile = "c:/doc.pdf";
         }
         else if(typebase64=="V" /*txt*/)
                 {
              delctype = "Files (*.txt)";
              savefile = "c:/text.txt";
                 }
         else if(typebase64=="A" /*mp4*/)
         {
        	 delctype = "Files (*.mp4)";
        	 savefile = "c:/movie.mp4";
         }
         else if( typebase64=="R")  /*gif*/
         {
        	 delctype = "Files (*.gif)";
        	 savefile = "c:/image.gif";
         }
         else if( typebase64=="U")  /*avi*/
         {
           	 delctype = "Files (*.avi)";
           	 savefile = "c:/movie.avi";
         }
         else if( typebase64=="S")  /*mp3*/
         {
           	 delctype = "Files (*.mp3)";
           	 savefile = "c:/music.mp3";
         }

            base64 base64trdialog;

             std::vector<unsigned char> bytesarray = base64trdialog.decode(encodestr);

             if(bytesarray.size()>0)
                     	{

	   QByteArray *base64decodearray;
     		  base64decodearray = new QByteArray(reinterpret_cast<const char*>(bytesarray.data()), bytesarray.size());



	QString fileNamesave = QFileDialog::getSaveFileName(this, tr("Save File"),
			                  savefile.c_str(),
	                            tr(delctype.c_str()));

        QByteArray base64decodefilearray;

        base64decodefilearray.setRawData(base64decodearray->data(), base64decodearray->size());
	// ... fill the array with data ...

	QFile file(fileNamesave.toUtf8().constData());
	file.open(QIODevice::WriteOnly);


	file.write(base64decodefilearray);
	file.close();
 		}
   }

TransactionDescDialog::~TransactionDescDialog()
{
    delete ui;
}

std::vector<std::string> TransactionDescDialog::split(std::string str,std::string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

