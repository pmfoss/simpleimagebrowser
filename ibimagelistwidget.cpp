/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#include "ibimagelistwidget.hpp"

/* Constructs the image list view with its item delegate and list model. */
IBImageListWidget::IBImageListWidget(QWidget *parent)
   : QListView(parent)
{
   QSize thumbsize(232,130);
   QString path = QDir::currentPath();

   this->setResizeMode(QListView::Adjust);
   this->setViewMode(QListView::IconMode);

   this->idDelegate = new IBItemDelegate(this);
   this->setItemDelegate(this->idDelegate);

   this->ifmImageModel = new IBImageListModel(path, thumbsize);
   this->ifmImageModel->setSectionType(IBImageListModel::NoSection);
   this->connect(this->ifmImageModel, SIGNAL(itemChanged(const QModelIndex &)), SLOT(update(const QModelIndex &)));

   this->setModel(ifmImageModel);
}

/* Changes the visible size of the item delegate for the section items. */ 
void IBImageListWidget::resizeEvent(QResizeEvent *event)
{
   this->idDelegate->resizeSectionSize(event->size());

   QListView::resizeEvent(event);
}

/* Invoke the refreshing of the list model. */
void IBImageListWidget::refresh()
{
   this->ifmImageModel->refresh();
}

/* Sets a path (path) for the list model. */
void IBImageListWidget::setImagePath(const QString &path)
{
   this->ifmImageModel->setImagePath(path);
}

/* Returns the image path of the list model. */
QString IBImageListWidget::getImagePath() const
{
   return this->ifmImageModel->getImagePath();
}

/* Sets a section type (type) for the list model and reselect the currently selected item. */ 
void IBImageListWidget::setSectionType(IBImageListModel::IBListSectionType type)
{
   QModelIndex selidx = this->selectionModel()->currentIndex();
   
   selidx = this->ifmImageModel->setSectionType(type, selidx);
   this->setCurrentIndex(selidx); 
}

/* Returns the section type of the list model. */
IBImageListModel::IBListSectionType IBImageListWidget::getSectionType()
{
   return this->ifmImageModel->getSectionType();
}

/* Sets a order of image sorting for the list model and reselect the currently selected item. */
void IBImageListWidget::setImageSortOrder(Qt::SortOrder order)
{
   QModelIndex selidx = this->selectionModel()->currentIndex();

   selidx = this->ifmImageModel->setImageSortOrder(order, selidx);
   this->setCurrentIndex(selidx); 
}

/* Returns the order of image sorting. */
Qt::SortOrder IBImageListWidget::getImageSortOrder() const
{
   return this->ifmImageModel->getImageSortOrder();
}

/* Sets a order of section sorting for the list model and reselect the currently selected item. */
void IBImageListWidget::setSectionSortOrder(Qt::SortOrder order)
{
   QModelIndex selidx = this->selectionModel()->currentIndex();

   selidx = this->ifmImageModel->setSectionSortOrder(order, selidx);
   this->setCurrentIndex(selidx); 
}

/* Returns the order of section sorting. */
Qt::SortOrder IBImageListWidget::getSectionSortOrder() const
{
   return this->ifmImageModel->getSectionSortOrder();
}

/* Sets a field of image sorting for the list model and reselect the currently selected item. */
void IBImageListWidget::setImageSortField(IBImageListModel::IBImageSortField field)
{
   QModelIndex selidx = this->selectionModel()->currentIndex();

   selidx = this->ifmImageModel->setImageSortField(field, selidx);
   this->setCurrentIndex(selidx); 
}

/* Returns the field of image sorting. */
IBImageListModel::IBImageSortField IBImageListWidget::getImageSortField() const
{
   return this->ifmImageModel->getImageSortField();
}

/* Emits the signal selectionChanged with the first selected item. */
void IBImageListWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
   QListView::selectionChanged(selected, deselected);
   emit this->selectionChanged(selected.indexes().value(0));
}
