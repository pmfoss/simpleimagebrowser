/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#ifndef H_IBIMAGELISTWIDGET
#define H_IBIMAGELISTWIDGET

#include <QItemSelection>
#include <QListView>
#include <QRect>
#include <QRegion>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWidget>

#include "ibimagelistmodel.hpp"
#include "ibitemdelegate.hpp"

class IBImageListWidget : public QListView
{
   Q_OBJECT 

   public:
      IBImageListWidget(QWidget *parent = nullptr);
      void setSectionType(IBImageListModel::IBListSectionType type);
      IBImageListModel::IBListSectionType getSectionType();

      void setSectionSortOrder(Qt::SortOrder order);
      Qt::SortOrder getSectionSortOrder() const;

      void setImageSortOrder(Qt::SortOrder order);
      Qt::SortOrder getImageSortOrder() const;
      void setImageSortField(IBImageListModel::IBImageSortField field);
      IBImageListModel::IBImageSortField getImageSortField() const;

      QString getImagePath() const;

   signals:
      void selectionChanged(const QModelIndex &index);

   public slots:
      void setImagePath(const QString &path);
      void refresh();

   protected:
      void resizeEvent(QResizeEvent *event) override;
      void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

   private:
      /* contains the ItemDelegate object of the view */ 
      IBItemDelegate *idDelegate;
      /* contains the List Model of the view */
      IBImageListModel *ifmImageModel;
};

#endif /*H_IBIMAGELISTWIDGET*/
