/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#ifndef H_IBFILECOMBOBOX
#define H_IBFILECOMBOBOX

#include <QComboBox>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QTreeView>
#include <QWidget>

/* class IBFileSystemModel */

class IBFileSystemModel : public QFileSystemModel
{
   public:
      IBFileSystemModel(QObject *parent = nullptr);
      int columnCount(const QModelIndex &parent = QModelIndex()) const override;
};

/* class IBFileComboBox */

class IBFileComboBox : public QComboBox
{
   Q_OBJECT

   public:
      IBFileComboBox(QWidget *parent = nullptr);
      IBFileComboBox(const QString &syspath, QWidget *parent = nullptr);
     
      void setPath(const QString &newpath);
      QString getPath() const;
     
      void hidePopup();
      void showPopup();
      bool eventFilter(QObject* object, QEvent* event); 
      bool event(QEvent *e); 

   signals:
      void pathChanged(const QString &path);

   protected slots:
      void onIndexChanged(int index);

   private:
      void initComboBox();

      /* Treeview for the popup widget */
      QTreeView *tvView;
      /* file system model for the Treeview */
      IBFileSystemModel *fsModel;
      /* prevention of premature closing of the popup widget */
      bool bSkipNextHide;
};

#endif /*H_IBFILECOMBOBOX*/
