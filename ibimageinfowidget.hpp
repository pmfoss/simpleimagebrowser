/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#ifndef IBIMAGEINFOWIDGET_H
#define IBIMAGEINFOWIDGET_H

#include <QDateTime>
#include <QFileInfo>
#include <QImage>
#include <QLocale>
#include <QPainter>
#include <QPaintEvent>
#include <QPixelFormat>
#include <QString>
#include <QWidget>

class IBImageInfoWidget : public QWidget
{
   public:
      IBImageInfoWidget(QWidget *parent = nullptr);
      void setImagePath(const QString &path);
      QString getImagePath() const;
      QImage getImage() const;

   protected:
      void paintEvent(QPaintEvent *event) override;

   private:
      /* contains the path to the displayed image */
      QString strPath;
      /* contains the loaded image */
      QImage imgData;
};

#endif /*IBIMAGEINFOWIDGET_H*/
