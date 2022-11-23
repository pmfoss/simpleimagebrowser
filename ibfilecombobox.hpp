/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#ifndef H_IBFILECOMBOBOX
#define H_IBFILECOMBOBOX

#include <QComboBox>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QStringList>
#include <QTreeView>
#include <QWidget>

#include <QDebug>

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

    public:
      /* Returns true, if the this->iCurrentHistoryIndex is at the begin of the history list.
         Otherwise false. */
      inline bool isHistoryAtFirstIndex() const
        {  return this->iCurrentHistoryIndex <= 0; }

      /* Returns true, if the this->iCurrentHistoryIndex is at the end of the history list.
         Otherwise false. */
      inline bool isHistoryAtLastIndex() const
        {  return this->iCurrentHistoryIndex >= (this->slHistory.size() - 1); }
     
      void hidePopup();
      void showPopup();
      bool eventFilter(QObject* object, QEvent* event); 
      bool event(QEvent *e); 

   public slots:
      void goHistoryBack();
      void goHistoryForward();
      void goToHomeDirectory();

   signals:
      void pathChanged(const QString &path);

   protected:
      void navigateTo(const QString &path, bool fromhistory = false);

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
      /* history list */
      QStringList slHistory;
      /* current index of the history list */
      int iCurrentHistoryIndex;
};

#endif /*H_IBFILECOMBOBOX*/
