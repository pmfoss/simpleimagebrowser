/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#include "ibimageinfowidget.hpp"

/* used to convert the enumeration QPixelFormat::ColorModel to string */
static const char *strPixelFormat[] = {"RGB", "BGR", "Color Palette", "Grayscale", "CMYK", "HSL", "HSV", "YUV", "none"};

/* Constructs the widget */
IBImageInfoWidget::IBImageInfoWidget(QWidget *parent)
   : QWidget(parent)
{
}

/* reimpl. Draw the image and its information. */
void IBImageInfoWidget::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);
   QImage himage;
   QRect imgrect(20, 20, this->width() - 40, this->height() - 200);
   QPoint txtpos(20, imgrect.y() + imgrect.height() + 20);
   QFont paintfont;
   QFileInfo finfo;

   QWidget::paintEvent(event);

   painter.setPen(Qt::NoPen);
   painter.fillRect(0, 0, this->width(), this->height(), this->parentWidget()->palette().base()); 

   if(!this->imgData.isNull())
   {
      painter.setRenderHint(QPainter::Antialiasing);
      painter.save();

      if(imgrect.width() < this->imgData.width() || imgrect.height() < this->imgData.height())
      {
         himage = this->imgData.scaled(imgrect.size(), Qt::KeepAspectRatio);
      }
      else
      {
         himage = this->imgData.copy();
      }

      painter.setBrush(Qt::NoBrush);
      painter.drawImage(QRect(imgrect.x() + (imgrect.width() - himage.width()) / 2, 
                         imgrect.y() + (imgrect.height() - himage.height()) / 2, 
                         himage.width(), himage.height()), himage);  

      painter.setPen(this->palette().color(QPalette::Text));
      painter.drawRect(imgrect.x() + (imgrect.width() - himage.width()) / 2, 
                         imgrect.y() + (imgrect.height() - himage.height()) / 2, 
                         himage.width(), himage.height());  

      finfo.setFile(this->strPath);
      paintfont = painter.font();
      paintfont.setPixelSize(14);
      painter.setFont(paintfont);

      painter.drawText(txtpos, QString("Filename: %1").arg(finfo.fileName()));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("Location: %1").arg(finfo.absolutePath()));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("File size: %1").arg(this->locale().formattedDataSize(finfo.size())));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("Last modification: %1").arg(finfo.lastModified().toString("yyyy-MM-dd HH:mm:ss")));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("Image size (WxH): %1x%2").arg(this->imgData.width()).arg(this->imgData.height()));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("Color model: %1").arg(strPixelFormat[this->imgData.pixelFormat().colorModel()]));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("Color depth: %1 bit").arg(this->imgData.depth()));
      txtpos += QPoint(0, 17);
      painter.drawText(txtpos, QString("Alpha channel: %1").arg(this->imgData.hasAlphaChannel() ? "yes" : "no"));

      painter.restore();
   }
}

/* Sets the path of an image file and loads it. */ 
void IBImageInfoWidget::setImagePath(const QString &path)
{
   this->strPath = path;
   this->imgData.load(path);
   this->update();
}

/* Returns the path of an image file. */
QString IBImageInfoWidget::getImagePath() const
{
   return this->strPath; 
}

/* Returns the loaded image. */
QImage IBImageInfoWidget::getImage() const
{
   return this->imgData;
}
