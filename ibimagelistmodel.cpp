/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#include "ibimagelistmodel.hpp"

/* class IBImageListModel */

/* Constructs the Image List Model with the given parent. */
IBImageListModel::IBImageListModel(QObject * parent)
   : QAbstractListModel(parent), szThumbnailSize(QSize(0,0)), stSectionType(NoSection),
     soSectionSortOrder(Qt::AscendingOrder), isfImageSortField(SortByName), soImageSortOrder(Qt::AscendingOrder)
{
   this->lstItems = new IBImageListSectionList();
   this->initThumbnailLoader();
   this->initImageDir();
}

/* Constructs the Image List Model with the given image path (imagepath) and parent. */
IBImageListModel::IBImageListModel(QString& imagepath, QObject * parent)
   : QAbstractListModel(parent), szThumbnailSize(QSize(0,0)), stSectionType(NoSection),
     soSectionSortOrder(Qt::AscendingOrder), isfImageSortField(SortByName), soImageSortOrder(Qt::AscendingOrder)
{
   this->lstItems = new IBImageListSectionList();
   this->initThumbnailLoader();
   this->initImageDir(imagepath);
}

/* Constructs the Image List Model with the given image path (imagepath), size of thumbnails (thumbsize) and parent. */
IBImageListModel::IBImageListModel(QString& imagepath, QSize& thumbsize, QObject * parent)
   : QAbstractListModel(parent), szThumbnailSize(thumbsize), stSectionType(NoSection),
     soSectionSortOrder(Qt::AscendingOrder), isfImageSortField(SortByName), soImageSortOrder(Qt::AscendingOrder)
{
   this->lstItems = new IBImageListSectionList();
   this->initThumbnailLoader();
   this->initImageDir(imagepath);
}
   
/* Constructs the Image List Model with the given size of thumbnails (thumbsize) and parent. */
IBImageListModel::IBImageListModel(QSize& thumbsize, QObject * parent)
   : QAbstractListModel(parent), szThumbnailSize(thumbsize), stSectionType(NoSection),
     soSectionSortOrder(Qt::AscendingOrder), isfImageSortField(SortByName), soImageSortOrder(Qt::AscendingOrder)
{
   this->lstItems = new IBImageListSectionList();
   this->initThumbnailLoader();
   this->initImageDir();
}

/* Destructs the model. The model destroys all its items. */
IBImageListModel::~IBImageListModel()
{
   if(this->thdThumbLoader->isRunning())
   {
      this->thdThumbLoader->terminate();
      this->thdThumbLoader->wait();
   }
   delete this->thdThumbLoader;
}

/* reimpl. */
int IBImageListModel::rowCount(const QModelIndex& parent) const
{
   Q_UNUSED(parent)

   return lstItems->totalSize();
}

/* reimpl. */
int IBImageListModel::columnCount(const QModelIndex& parent) const
{
   Q_UNUSED(parent)

   return 1;
}

/* reimpl. */
QVariant IBImageListModel::data(const QModelIndex &index, int role) const
{
   IBImageListAbstractItem *item;   

   if(!index.isValid())
   {
      return QVariant();
   }
   
   item = this->lstItems->getItemByLinearIndex(index.row());

   if(item)
   {
      if(role == IBImageListModel::ItemName)
      {
         return item->getName();
      }
      else if(role == IBImageListModel::ItemIsSection)
      {
         return item->getType() == IBImageListAbstractItem::Section;
      }
      else
      {
         if(item->getType() == IBImageListAbstractItem::Image)
         {
            switch(role)
            {
               case IBImageListModel::ItemFileName:
                  return dynamic_cast<IBImageListImageItem *>(item)->getFileName(); 

               case IBImageListModel::ItemFilePath:
                  return dynamic_cast<IBImageListImageItem *>(item)->getFilePath();

               case IBImageListModel::ItemFileType:
                  return dynamic_cast<IBImageListImageItem *>(item)->getFileType();

               case IBImageListModel::ItemFileLastModified:
                  return dynamic_cast<IBImageListImageItem *>(item)->getLastModified();

               case IBImageListModel::ItemImageSize:
                  return dynamic_cast<IBImageListImageItem *>(item)->getImageSize();

               case IBImageListModel::ItemImageLoaded:
                  return dynamic_cast<IBImageListImageItem *>(item)->isImageLoaded();

               case IBImageListModel::ItemThumbnail:
                  return dynamic_cast<IBImageListImageItem *>(item)->getThumbnail();
            }
         }
      } 
   }

   return QVariant();
}
/* Loads the file data, invokes the generation of the model structure and starts the loading of thumbnails. */
void IBImageListModel::loadImageData()
{   
   QFileInfoList fileinfos;
   QList<QFileInfo>::iterator it;

  
   if(this->thdThumbLoader->isRunning())
   {
      this->thdThumbLoader->terminate();
      this->thdThumbLoader->wait();
   }

   this->lstFileData.clear();

   fileinfos = this->dirImages.entryInfoList();

   for(it = fileinfos.begin(); it != fileinfos.end(); ++it)
   {
      lstFileData.append(new IBImageListImageItem(*it));
   }

   this->buildItemsList();
   this->thdThumbLoader->start();
}

/* Invokes the repreparing of the model data */
void IBImageListModel::refresh()
{
   this->dirImages.refresh();
   this->loadImageData(); 
}

/* Sets the path to images (imagepath) and invokes the preparing of the model data */
void IBImageListModel::setImagePath(const QString& imagepath)
{ 
   if(imagepath == dirImages.path())
   {
      return;
   }
   this->dirImages.setPath(imagepath);
   this->loadImageData();
}

/* Returns the current image path. */
QString IBImageListModel::getImagePath() const
{
   return this->dirImages.path();
}

/* Sets the size of thumbnails (size) */
void IBImageListModel::setThumbnailSize(QSize& size)
{
   this->szThumbnailSize = size;
   
   if(this->thdThumbLoader)
   {
      this->thdThumbLoader->setThumbnailSize(size);
   }
}

/* Returns the size of thumbnails */
QSize IBImageListModel::getThumbnailSize() const
{
   return this->szThumbnailSize;
}

/* Sets the type (type) of section heads and invokes the restructure of the model. If the index of the 
   currently selected item (selected) is given, the new index of this item is returned. 
   see IBImageListModel::IBListSectionType */
QModelIndex IBImageListModel::setSectionType(const IBListSectionType type, const QModelIndex &selected)
{
   IBImageListAbstractItem *selitem;   

   if(this->stSectionType != type)
   {
      selitem = this->getRawItem(selected);
      this->stSectionType = type;
      this->buildItemsList();
      return this->getRawItemIndex(selitem);
   }

   return selected;
}

/* Returns the type of section heads. see IBImageListModel::IBListSectionType */
IBImageListModel::IBListSectionType IBImageListModel::getSectionType() const
{
   return this->stSectionType;
}

/* Sets the order (order) of the section sorting and invokes the sorting of the model. If the index of the 
   currently selected item (selected) is given, the new index of this item is returned. 
   see Qt::SortOrder */
QModelIndex IBImageListModel::setSectionSortOrder(Qt::SortOrder order, const QModelIndex &selected)
{
   IBImageListAbstractItem *selitem;   

   if(this->soSectionSortOrder != order)
   {
      selitem = this->getRawItem(selected);
      this->soSectionSortOrder = order;
      this->lstItems->sortSections(order);
      emit this->layoutChanged();
      return this->getRawItemIndex(selitem);
   }

   return selected;
}

/* Returns the order of the section sorting. see Qt::SortOrder */
Qt::SortOrder IBImageListModel::getSectionSortOrder() const
{
   return this->soSectionSortOrder; 
}

/* Sets the order (order) of the image sorting and invokes the sorting of the model. If the index of the 
   currently selected item (selected) is given, the new index of this item is returned. 
   see Qt::SortOrder */
QModelIndex IBImageListModel::setImageSortOrder(Qt::SortOrder order, const QModelIndex &selected)
{
   IBImageListAbstractItem *selitem;   

   if(this->soImageSortOrder != order)
   {
      selitem = this->getRawItem(selected);
      this->soImageSortOrder = order;
      this->lstItems->sortImageItems(this->isfImageSortField, order);
      emit this->layoutChanged();
      return this->getRawItemIndex(selitem);
   }

   return selected;
}

/* Returns the order of the image sorting. see Qt::SortOrder */
Qt::SortOrder IBImageListModel::getImageSortOrder() const
{
   return this->soImageSortOrder; 
}

/* Sets the field (field) of the image sorting and invokes the sorting of the model. If the index of the 
   currently selected item (selected) is given, the new index of this item is returned. 
   see IBImageListModel::IBImageSortField */
QModelIndex IBImageListModel::setImageSortField(IBImageSortField field, const QModelIndex &selected)
{
   IBImageListAbstractItem *selitem;   

   if(this->isfImageSortField != field)
   {
      selitem = this->getRawItem(selected);
      this->isfImageSortField = field;
      this->lstItems->sortImageItems(field, this->soImageSortOrder);
      emit this->layoutChanged();
      return this->getRawItemIndex(selitem);
   }

   return selected;
}

/* Returns the image sorting field. see IBImageListModel::IBImageSortField */
IBImageListModel::IBImageSortField IBImageListModel::getImageSortField() const
{
   return this->isfImageSortField;
} 

/* Resets and (re-)structures the data of the model. */
void IBImageListModel::buildItemsList()
{
   QList<IBImageListImageItem *>::iterator it;
   QString hstr = QStringLiteral("");
   QDate hdate;
  
   this->beginResetModel();
   this->lstItems->clear();

   for(it = this->lstFileData.begin(); it != this->lstFileData.end(); ++it)
   {
      switch(this->stSectionType)
      {
         case IBImageListModel::DateSection:
            hdate = (*it)->getLastModified().date();
            this->lstItems->addImageItem(hdate, (*it));
            break;   

         case IBImageListModel::AlphabeticSection:
            if((*it)->getName()[0].isDigit())
            {
               hstr = QStringLiteral("#");
            }
            else
            {
               hstr = (*it)->getName().left(1).toUpper();
            }
            this->lstItems->addImageItem(hstr, (*it));
            break;
            
         case IBImageListModel::FileTypeSection:
            hstr = (*it)->getFileType().toUpper();
            this->lstItems->addImageItem(hstr, (*it));
            break;

         case IBImageListModel::NoSection:
         default:
            this->lstItems->addImageItem(hstr, (*it));
      }

   }

   this->lstItems->sortSections(this->soSectionSortOrder);
   this->lstItems->sortImageItems(this->isfImageSortField, this->soImageSortOrder);
   
   this->endResetModel();
}

/* Converts the integer index (index) of an item into the corresponding model index and emits the signal itemChanged. */
void IBImageListModel::onImageLoaded(int index)
{
   int lidx = this->lstItems->getLinearIndexOfItem(lstFileData[index]);
   emit this->itemChanged(this->index(lidx, 0));
}

/* Returns the item object of the given model index (index). If the index does not exist, nullptr is returned. */
IBImageListAbstractItem *IBImageListModel::getRawItem(const QModelIndex &index)
{
   if(!index.isValid())
   {
      return nullptr;
   }
   
   return this->lstItems->getItemByLinearIndex(index.row());
}  

/* Returns the corresponding model index of an item index (item). If the item is not indexed, an invalid model index 
   is returned. */
QModelIndex IBImageListModel::getRawItemIndex(IBImageListAbstractItem *item)
{
   int lidx = this->lstItems->getLinearIndexOfItem(item);
   return this->index(lidx, 0);
}

/* Initializes the structure for the fetching the image data. */
void IBImageListModel::initImageDir()
{
   QString curpath = QDir::currentPath();
   this->initImageDir(curpath);
}

/* Initializes the structure for the fetching the image data. */
void IBImageListModel::initImageDir(const QString& imagepath)
{
   QStringList namefilters;

   namefilters << "*.bmp" << "*.jpeg" << "*.jpg" << "*.png" << "*.ppm" << "*.xbm" << "*.xpm";
   this->dirImages.setNameFilters(namefilters);
   this->dirImages.setPath(QDir::currentPath());
   this->setImagePath(imagepath);
}

/* Initializes the thread for loading the image thumbnails. */
void IBImageListModel::initThumbnailLoader()
{
   this->thdThumbLoader = new IBThumbnailLoader(this);
   this->thdThumbLoader->setImageList(&this->lstFileData);
   this->thdThumbLoader->setThumbnailSize(this->szThumbnailSize);
   this->connect(this->thdThumbLoader, SIGNAL(imageLoaded(int)), SLOT(onImageLoaded(int)));
}

/* Class IBImageListAbstractItem */

/* Constructs an abstract object of an item for the image list model with the given type of item (itemtype).
   see IBImageListAbstractItem::ItemType */
IBImageListAbstractItem::IBImageListAbstractItem(IBImageListAbstractItem::ItemType itemtype)
   : itType(itemtype)
{
}

/* Returns the type of an image list model item. see IBImageListAbstractItem::ItemType */
IBImageListAbstractItem::ItemType IBImageListAbstractItem::getType() const
{
   return this->itType;
}

/* Returns the type (itemtype) of an image list model item. see IBImageListAbstractItem::ItemType */
void IBImageListAbstractItem::setType(IBImageListAbstractItem::ItemType itemtype)
{
   this->itType = itemtype; 
} 

/* class IBImageListImageItem */

/* Constructs an empty image data item for the image list model. */
IBImageListImageItem::IBImageListImageItem()
   : IBImageListAbstractItem(IBImageListAbstractItem::Image)
{
}

/* Constructs an image data item for the image list model with given file information (info). */
IBImageListImageItem::IBImageListImageItem(QFileInfo &info)
   : IBImageListAbstractItem(IBImageListAbstractItem::Image)
{
   this->load(info);
}

/* Loads the data of an image data item with the given file information (info). */
void IBImageListImageItem::load(QFileInfo &info)
{
   this->strFileName = info.fileName();
   this->strFileType = info.suffix();
   this->strFilePath = info.canonicalFilePath();
   this->dtLastModified = info.lastModified();
   this->bImageLoaded = false;
}

/* Loads the image data of an item and scales it to the given size (thumbsize). It generates the thumbnail.*/
void IBImageListImageItem::loadImage(QSize &thumbsize)
{
   QPixmap pixtmp;

   pixtmp.load(this->strFilePath);
   this->szImageSize = pixtmp.size();

   if(pixtmp.width() <= thumbsize.width() && pixtmp.height() <= thumbsize.height())
   {
      this->pxThumbnail = pixtmp;
   }
   else
   {
      this->pxThumbnail = pixtmp.scaled(thumbsize, Qt::KeepAspectRatio);
   }

   this->bImageLoaded = true;
}

/* Returns the name of item. It is the name of the corresponding file without extension. */
QString IBImageListImageItem::getName() const
{
   return QString(this->strFileName).remove(QRegularExpression(QString(".%1$").arg(this->strFileType)));
}

/* Returns the corresponding filename of the item. */
QString IBImageListImageItem::getFileName() const
{
   return this->strFileName;
}

/* Returns the file type of the corresponding file. It is the extension of the file. */
QString IBImageListImageItem::getFileType() const
{
   return this->strFileType;
}

/* Returns the path of the corresponding file. */
QString IBImageListImageItem::getFilePath() const
{
   return this->strFilePath;
}

/* Returns the original size of the image. */
QSize IBImageListImageItem::getImageSize() const
{
   return this->szImageSize;
}

/* Returns the thumbnail of the image. */
QVariant IBImageListImageItem::getThumbnail() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
   return QVariant(QMetaType(QMetaType::QPixmap), static_cast<const void *>(&this->pxThumbnail));
#else
   return QVariant(QMetaType::QPixmap, static_cast<const void *>(&this->pxThumbnail));
#endif /*(QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))*/
}

/* Returns the last modification date of the corresponding file. */
QDateTime IBImageListImageItem::getLastModified() const
{
   return this->dtLastModified;
}

/* Returns true if the image is loaded successfully. Otherwise false. */
bool IBImageListImageItem::isImageLoaded() const
{
   return this->bImageLoaded;
}

/* class IBImageListSectionItem */

/* Constructs an section object of the image list model. */ 
IBImageListSectionItem::IBImageListSectionItem()
   : QList<IBImageListImageItem *>(), IBImageListAbstractItem(IBImageListAbstractItem::Section)
{
}

/* Constructs an section object of the image list model with the name (itemid) as the type of QVariant. */ 
IBImageListSectionItem::IBImageListSectionItem(QVariant &itemid)
   : QList<IBImageListImageItem *>(), IBImageListAbstractItem(IBImageListAbstractItem::Section)
{
   this->setItemID(itemid);
}

/* Constructs an section object of the image list model with the name (itemid) as the type of QString. */
IBImageListSectionItem::IBImageListSectionItem(QString &itemid)
   : QList<IBImageListImageItem *>(), IBImageListAbstractItem(IBImageListAbstractItem::Section)
{
   this->setItemID(itemid);
}

/* Constructs an section object of the image list model with the name (itemid) as the type of QDate. */
IBImageListSectionItem::IBImageListSectionItem(QDate &itemid)
   : QList<IBImageListImageItem *>(), IBImageListAbstractItem(IBImageListAbstractItem::Section)
{
   this->setItemID(itemid);
}

/* Returns the name of the section item as a QString value. If no name is set, an empty value is returned. */
QString IBImageListSectionItem::getName() const
{
   if(this->varId.isValid())
   {
      return this->varId.toString();
   }
   else
   {
      return QString();
   }
}

/* Sets the name (itemid) of the section item as the type of QVariant. */
void IBImageListSectionItem::setItemID(QVariant &itemid)
{
   this->varId = itemid;
}

/* Sets the name (itemid) of the section item as the type of QString. */
void IBImageListSectionItem::setItemID(QString &itemid)
{
   this->varId = QVariant(itemid);
}

/* Sets the name (itemid) of the section item as the type of QDate. */
void IBImageListSectionItem::setItemID(QDate &itemid)
{
   this->varId = QVariant(itemid);
}

/* Returns the name of the section item as a QVariant value. */
QVariant IBImageListSectionItem::getItemID() const
{
   return this->varId;
}

/* Sorts the images items of the item according to given the field (field) and the order (order). */ 
void IBImageListSectionItem::sortItems(IBImageListModel::IBImageSortField field, Qt::SortOrder order)
{
   std::sort(this->begin(), this->end(), [field, order](IBImageListImageItem *itemA, IBImageListImageItem *itemB)
     {
        switch(field)
        {
           case IBImageListModel::SortByName:
              if(order ==  Qt::AscendingOrder)
              {
                 return QString::compare(itemA->getName(), itemB->getName(), Qt::CaseInsensitive) < 0;
              }
              else
              {
                 return QString::compare(itemA->getName(), itemB->getName(), Qt::CaseInsensitive) > 0;
              }

           case IBImageListModel::SortByDate:
              if(order ==  Qt::AscendingOrder)
              {
                 return itemA->getLastModified().date() < itemB->getLastModified().date();
              }
              else
              {
                 return itemA->getLastModified().date() > itemB->getLastModified().date();
              }


           case IBImageListModel::SortByFileType:
              if(order ==  Qt::AscendingOrder)
              {
                 return QString::compare(itemA->getFileType(), itemB->getFileType(), Qt::CaseInsensitive) > 0;
              }
              else
              {
                 return QString::compare(itemA->getFileType(), itemB->getFileType(), Qt::CaseInsensitive) > 0;
              }
        }
        return false;
     });
}

/* Returns true if the name of the item is less than the given item (item). Otherwise false is returned. 
   If the name is of type QString, then the name of item is lexically less than the name of the given item. 
   If the name is of type QDate, then the name of item is older than the name of the given item. */
bool IBImageListSectionItem::operator< (const IBImageListSectionItem &item) noexcept(false)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
   if(this->varId.typeId() != item.varId.typeId())
   {
     return false;
   }

   if(this->varId.typeId() == QMetaType::QDate)
   {
      return this->varId.toDate() < item.varId.toDate();
   }
   else
   {
      return this->varId.toString() < item.varId.toString();
   }
#else
   if(this->varId.type() != item.varId.type())
   {
     return false;
   }

   if(this->varId.type() == QVariant::Date)
   {
      return this->varId.toDate() < item.varId.toDate();
   }
   else
   {
      return this->varId.toString() < item.varId.toString();
   }
#endif /*QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)*/
}

/* Returns true if the name of the item is greater than the given item (item). Otherwise false is returned. 
   If the name is of type QString, then the name of item is lexically greater than the name of the given item. 
   If the name is of type QDate, then the name of item is newer than the name of the given item. */
bool IBImageListSectionItem::operator> (const IBImageListSectionItem &item) noexcept(false)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
   if(this->varId.typeId() != item.varId.typeId())
   {
     return false;
   }

   if(this->varId.typeId() == QMetaType::QDate)
   {
      return this->varId.toDate() > item.varId.toDate();
   }
   else
   {
      return this->varId.toString() > item.varId.toString();
   }
#else
   if(this->varId.type() != item.varId.type())
   {
     return false;
   }

   if(this->varId.type() == QVariant::Date)
   {
      return this->varId.toDate() > item.varId.toDate();
   }
   else
   {
      return this->varId.toString() > item.varId.toString();
   }
#endif /*QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)*/
}

/* class IBImageListSectionList */

/* Constructs a list object for all model data. */

IBImageListSectionList::IBImageListSectionList()
  : QList<IBImageListSectionItem *>()
{
}

/* Adds a given item (item) to a given section item (section). If the section item does not exist, it will be created. */
void IBImageListSectionList::addImageItem(QVariant &section, IBImageListImageItem *item)
{
   QList<IBImageListSectionItem *>::iterator it;
   IBImageListSectionItem *newsec;

   bool found = false;

   for(it = this->begin(); it != this->end() && !found; ++it)
   {
      if((*it) == section)
      {
        (*it)->append(item);
        found = true;
      }
   }

   if(!found)
   {
      newsec = new IBImageListSectionItem(section);
      newsec->append(item);
      this->append(newsec);
   }  
}

/* Adds a given item (item) to a given section item (section). If the section item does not exist, it will be created. */
void IBImageListSectionList::addImageItem(QString &section, IBImageListImageItem *item)
{
   QVariant sec = section;
   this->addImageItem(sec, item);
}

/* Adds a given item (item) to a given section item (section). If the section item does not exist, it will be created. */
void IBImageListSectionList::addImageItem(QDate &section, IBImageListImageItem *item)
{
   QVariant sec = section;
   this->addImageItem(sec, item);
}

/* Return the item of a given index (index). The 2D data list are handled like a 1D list. */ 
IBImageListAbstractItem *IBImageListSectionList::getItemByLinearIndex(int index) const
{
   QList<IBImageListSectionItem *>::const_iterator it;
   IBImageListAbstractItem *retitem = nullptr;
   int revidx;

   if(this->isEmpty() && index < 0)
   {
      return nullptr;
   }

   if(this->first() == this->last() && this->first()->getName().isEmpty())
   {
      if(index < this->first()->size())
      {
         retitem = this->first()->at(index);
      }
   }
   else
   {
      revidx = index;
      for(it = this->begin(); it != this->end() && !retitem; ++it)
      {
         if(revidx <= (*it)->size())
         {
            if(revidx == 0)
            {
               retitem = (*it); 
            }
            else
            {
               retitem = (*it)->at(revidx - 1);
            }
         }

         revidx -= (*it)->size() + 1;
      }
   }

   return retitem;
}

/* Return the index of a given item (item). The 2D data list are handled like a 1D list. */ 
int IBImageListSectionList::getLinearIndexOfItem(IBImageListAbstractItem *item) const
{
   QList<IBImageListSectionItem *>::const_iterator sit;
   IBImageListSectionItem::const_iterator iit;
   int itmidx = 0;
   
   if(!item)
   {
      return -1;
   }
 
   if(this->first() == this->last() && this->first()->getName().isEmpty())
   {
      for(iit = this->first()->begin(); iit != this->first()->end(); ++iit)
      {
         if((*iit) == item)
         {
            return itmidx;
         }
         itmidx++;
       }
   }
   else
   {
      for(sit = this->begin(); sit != this->end(); ++sit)
      {
         if((*sit) == item)
         {
            return itmidx; 
         }
         itmidx++;

         for(iit = (*sit)->begin(); iit != (*sit)->end(); ++iit)
         {
            if((*iit) == item)
            {
               return itmidx;
            }
            itmidx++;
         }
      }
   }

   return -1;
}

/* reimpl. */
void IBImageListSectionList::clear()
{
   QList<IBImageListSectionItem *>::iterator it;

   for(it = this->begin(); it != this->end(); ++it)
   {
      (*it)->clear();
   }
   QList<IBImageListSectionItem *>::clear();
}

/* Returns the number of section items and its image items. If only one section item exists and 
   its name is empty, then the number of its image items is returned. */
int IBImageListSectionList::totalSize() const
{
   QList<IBImageListSectionItem *>::const_iterator it;
   int size;
 
   if(this->isEmpty())
   {
      return 0;
   }

   if(this->first() == this->last() && this->first()->getName().isEmpty())
   {
      size = this->first()->size();
   }
   else
   {
      size = this->size();
      for(it = this->begin(); it != this->end(); ++it)
      {
         size += (*it)->size();
      }
   }
   
   return size;
}

/* Invoke the sorting the images items of the section items according to given the field (field) and the order (order). */
void IBImageListSectionList::sortImageItems(IBImageListModel::IBImageSortField field, Qt::SortOrder order)
{
   QList<IBImageListSectionItem *>::iterator it;

   for(it = this->begin(); it != this->end(); ++it)
   {
      (*it)->sortItems(field, order);
   } 
}

/* Sorts the section item according to given the order (order). */
void IBImageListSectionList::sortSections(Qt::SortOrder order)
{
   std::sort(this->begin(), this->end(), [order](IBImageListSectionItem *itemA, IBImageListSectionItem *itemB)
                                             { 
                                                if(order == Qt::AscendingOrder)
                                                {
                                                    return (*itemA) < (*itemB); 
                                                }
                                                return  (*itemA) > (*itemB);});
}

/* class IBThumbnailLoader */

/* Constructs the thread for loading the image thumbnails */
IBThumbnailLoader::IBThumbnailLoader(QObject *parent)
   : QThread(parent), lstFileData(nullptr), szThumbnailSize(QSize(0,0))
{
}

/* Constructs the thread for loading the image thumbnails with size of the thumbnails (thumbsize) and
   the image list (data). */
IBThumbnailLoader::IBThumbnailLoader(QSize &thumbsize, QList<IBImageListImageItem *> *data, QObject *parent)
   : QThread(parent), lstFileData(data), szThumbnailSize(thumbsize)
{
}

/* Loads the images and invokes the creation of the thumbnail. It is finished, the signal imageLoaded is emitted. */
void IBThumbnailLoader::run()
{
   QList<IBImageListImageItem *>::iterator it;
   QPixmap pixtmp;

   if(!this->lstFileData)
   {
      return;
   }
  
   for(it = this->lstFileData->begin(); it != this->lstFileData->end(); ++it)
   {
      if((*it)->getType() == IBImageListAbstractItem::Image)
      {
         dynamic_cast<IBImageListImageItem *>(*it)->loadImage(this->szThumbnailSize);
         emit imageLoaded(it - this->lstFileData->begin());
      }
   }
}

/* Sets the size (size) of the thumbnails. */
void IBThumbnailLoader::setThumbnailSize(QSize &size)
{
   this->szThumbnailSize = size;
}

/* Returns the size of the thumbnails. */
QSize IBThumbnailLoader::getThumbnailSize() const
{
   return this->szThumbnailSize; 
}

/* Sets the image list (data) to be handled. */
void IBThumbnailLoader::setImageList(QList<IBImageListImageItem *> *data)
{
   this->lstFileData = data;
}

/* Returns the handled image list. */
QList<IBImageListImageItem *> *IBThumbnailLoader::getImageList() const
{
   return this->lstFileData;
}
