// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "guiutil.h"
#include "util.h"
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


    		 std::string delimiter = ":img:";

    		 std::string mimg = encodestr.substr(5);// remove mimg:



    		  std::size_t pos = mimg.find(delimiter);      // position of ":img:" in str


              if(pos>0){ // check exists :img:
    		 std::string message =  mimg.substr (0, pos);

    		 //LogPrintf("message: %s", message);

    		 desc = desc + "<br><b><b>"+message.c_str();
    		 ui->detailText->setHtml(desc);

    		 std::string imgbase64 =  mimg.substr (pos+5);


    		 encodestr=imgbase64;
              }   // end check exists :img:

        	}

    	if(ismessagetpye2 == "m:" || ismessagetpye5 == "from:")
        	 { //message

    		if(ismessagetpye2 == "m:"){
    		     encodestr = encodestr.substr(2);// remove m:
    		}

        		 ui->DownloadButton->setVisible(false);
        		 desc = desc + "<br><b><b>"+encodestr.c_str();
        		 ui->detailText->setHtml(desc);
        	}
    	else{

    	ui->DownloadButton->setVisible(true);

    	std::string typebase64 = encodestr.substr(0, 1);
             if(typebase64=="J" /*pdf*/ || typebase64=="V" /*txt*/){
            	 delctype = 2;
             }
             else if(typebase64=="R" /*gif*/)
             {
            	 delctype = 1;
             }
             else if(typebase64=="A" /*mp4*/ || typebase64=="U" /*avi*/)
             {
                	 delctype = 3;
             }
             else if(typebase64=="S" /*mp3*/)
             {
                	 delctype = 4;
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

     		 QGraphicsProxyWidget *proxy = scene->addWidget(gif_anim);
                  scene->addWidget(gif_anim);
     		              ui->graphicsView->setScene( scene );
     		             ui->graphicsView->show();

     		     movie->start();
     		       }
     		  /* Start Video */
     		  else if (delctype==3)
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
     		  /* Start mp3 audio */
     		  else if (delctype==4)
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

