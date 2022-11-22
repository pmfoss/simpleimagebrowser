/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#ifndef H_IBIMAGELISTMODEL
#define H_IBIMAGELISTMODEL

#include <QAbstractListModel>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QPixmap>
#include <QRegularExpression>
#include <QSize>
#include <QThread>
#include <QVariant>

/* forward definitions of class */

class IBThumbnailLoader;
class IBImageListAbstractItem;
class IBImageListImageItem;
class IBImageListSectionList;

/* class IBImageListModel */

class IBImageListModel : public QAbstractListModel
{
   Q_OBJECT

   public:
      /*type of data which model needs. see Qt::ItemDataRole*/
      enum Roles
      {
         ItemName = Qt::DisplayRole,
         ItemFileName = Qt::UserRole + 1,
         ItemFilePath = Qt::UserRole + 2,
         ItemFileType = Qt::UserRole + 3,
         ItemFileLastModified = Qt::UserRole + 4,
         ItemImageSize = Qt::UserRole + 5,
         ItemImageLoaded = Qt::UserRole + 6,
         ItemThumbnail = Qt::UserRole + 7,
         ItemIsSection = Qt::UserRole + 8
      };
      Q_ENUM(Roles)

      /*enumeration of fields for image sorting*/
      enum IBImageSortField
      {
         SortByName,
         SortByDate,
         SortByFileType
      };
      Q_ENUM(IBImageSortField)

      /*enumeration of the type of section heads*/
      enum IBListSectionType
      {
         NoSection,
         AlphabeticSection,
         DateSection,
         FileTypeSection
      };
      Q_ENUM(IBListSectionType)
      IBImageListModel(QObject * parent = 0);
      IBImageListModel(QString& path, QObject * parent = 0);
      IBImageListModel(QSize& thumbsize, QObject * parent = 0);
      IBImageListModel(QString& path, QSize& thumbsize, QObject * parent = 0);
      ~IBImageListModel();

      int rowCount(const QModelIndex& parent = QModelIndex()) const;
      int columnCount(const QModelIndex& parent = QModelIndex()) const;
      QVariant data(const QModelIndex &index, int role) const;

      void setImagePath(const QString& imagepath);
      QString getImagePath() const;

      void setThumbnailSize(QSize& size);
      QSize getThumbnailSize() const;

      QModelIndex setSectionType(const IBImageListModel::IBListSectionType type, const QModelIndex &selected = QModelIndex());
      IBImageListModel::IBListSectionType getSectionType() const;

      QModelIndex setSectionSortOrder(const Qt::SortOrder order, const QModelIndex &selected = QModelIndex());
      Qt::SortOrder getSectionSortOrder() const;
      QModelIndex setImageSortOrder(const Qt::SortOrder order, const QModelIndex &selected = QModelIndex());
      Qt::SortOrder getImageSortOrder() const;
      QModelIndex setImageSortField(IBImageListModel::IBImageSortField field, const QModelIndex &selected = QModelIndex());
      IBImageListModel::IBImageSortField getImageSortField() const;

   signals:
      void itemChanged(const QModelIndex &index);

   protected:
      void buildItemsList();

   protected slots:
      void onImageLoaded(int index);

   private:
      Q_DISABLE_COPY(IBImageListModel)

      /* list with image data, only for internal usage */
      QList<IBImageListImageItem *> lstFileData;
      /* list contains the data for the external usage */
      IBImageListSectionList *lstItems;
      /* handles the specified image directory */
      QDir dirImages;
      /* contains the size of the thumbnails */
      QSize szThumbnailSize;
      /* loads the thumbnails */
      IBThumbnailLoader *thdThumbLoader;
      /* specifies the type of sections */
      IBImageListModel::IBListSectionType stSectionType;
      /* specifies the order of the section sorting */
      Qt::SortOrder soSectionSortOrder;
      /* specifies the field of image sorting */
      IBImageListModel::IBImageSortField isfImageSortField;
      /* specifies the order of image sorting */
      Qt::SortOrder soImageSortOrder;

      void initImageDir();
      void initThumbnailLoader();
      void initImageDir(const QString& imagepath);
      QModelIndex getRawItemIndex(IBImageListAbstractItem *item);
      IBImageListAbstractItem *getRawItem(const QModelIndex &index);
};

/* class IBImageListAbstractItem */

class IBImageListAbstractItem
{
   public:
      /*enumeration of item type*/
      enum ItemType
      {
         None,
         Image,
         Section
      };

      IBImageListAbstractItem(IBImageListAbstractItem::ItemType itemtype = IBImageListAbstractItem::None);

      IBImageListAbstractItem::ItemType getType() const;
      void setType(IBImageListAbstractItem::ItemType type);

      virtual QString getName() const = 0;

   private:
      /* contains the type of list item */
      IBImageListAbstractItem::ItemType itType;
};

/* class IBImageListImageItem */

class IBImageListImageItem : public IBImageListAbstractItem
{
   friend class IBThumbnailLoader;

   public:
      IBImageListImageItem();
      IBImageListImageItem(QFileInfo &info);
   
      void load(QFileInfo &info);

      QString getName() const;
      QString getFileName() const;
      QString getFileType() const;
      QString getFilePath() const;
      QVariant getThumbnail() const;
      QDateTime getLastModified() const;
      QSize getImageSize() const;
      bool isImageLoaded() const;

   protected:
      void loadImage(QSize &thumbsize);

   private:
      /* is true if thumbnail is loaded successfully */
      bool bImageLoaded;
      /* contains the name of the corresponding file */
      QString strFileName;
      /* contains the extension of the corresponding file */
      QString strFileType;
      /* contains the path of the corresponding file */
      QString strFilePath;
      /* contains the thumbnail */
      QPixmap pxThumbnail;
      /* contains the original size of the image */
      QSize szImageSize;
      /* contains the timestamp of the last modification of the corresponding file */
      QDateTime dtLastModified;
};

/* class IBImageListSectionItem */

class IBImageListSectionItem : public QList<IBImageListImageItem *>, public IBImageListAbstractItem
{
   public:
      IBImageListSectionItem();
      IBImageListSectionItem(QVariant &itemid);
      IBImageListSectionItem(QString &itemid);
      IBImageListSectionItem(QDate &itemid);

      QString getName() const override;
      
      void setItemID(QVariant &itemid);
      void setItemID(QString &itemid);
      void setItemID(QDate &itemid);
      QVariant getItemID() const;

      void sortItems(IBImageListModel::IBImageSortField field = IBImageListModel::SortByName,  
                     Qt::SortOrder order = Qt::AscendingOrder);

      /*operator <*/
      bool operator< (const IBImageListSectionItem &item) noexcept(false);
      friend inline bool operator<(const IBImageListSectionItem *item, const QDate &date) noexcept(false)
             { return item->varId.toDate() < date; }
      friend inline bool operator<(const IBImageListSectionItem *item, const QString &str) noexcept(false)
             { return item->varId.toString() < str; };

      /*operator >*/
      bool operator> (const IBImageListSectionItem &item) noexcept(false);
      friend bool operator> (const IBImageListSectionItem *item, const QVariant &data) noexcept(false);
      friend inline bool operator>(const IBImageListSectionItem *item, const QDate &date) noexcept(false)
             { return item->varId.toDate() > date; }
      friend inline bool operator>(const IBImageListSectionItem *item, const QString &str) noexcept(false)
             { return item->varId.toString() > str; };

      /*operator ==*/
      inline bool operator==(const IBImageListSectionItem *item) noexcept(false)
             { return this->varId == item->varId; };
      friend inline bool operator==(const IBImageListSectionItem *item, const QVariant &data) noexcept(false)
             { return item->varId == data; };
      friend inline bool operator==(const IBImageListSectionItem *item, const QDate &date) noexcept(false)
             { return item->varId.toDate() == date; }
      friend inline bool operator==(const IBImageListSectionItem *item, const QString &str) noexcept(false)
             { return item->varId.toString() == str; };

      /*operator !=*/
      inline bool operator!=(const IBImageListSectionItem *item) noexcept(false)
             { return this->varId != item->varId; };
      friend inline bool operator!=(const IBImageListSectionItem *item, const QVariant &data) noexcept(false)
             { return item->varId != data; };
      friend inline bool operator!=(const IBImageListSectionItem *item, const QDate &date) noexcept(false)
             { return item->varId.toDate() != date; }
      friend inline bool operator!=(const IBImageListSectionItem *item, const QString &str) noexcept(false)
             { return item->varId.toString() != str; };

   private:
      /* contains the name of the section items */
      QVariant varId;
};

/* class IBImageListSectionList */

class IBImageListSectionList : QList<IBImageListSectionItem *>
{
   public:
      IBImageListSectionList();

      void addImageItem(QVariant &section, IBImageListImageItem *item);
      void addImageItem(QString &section, IBImageListImageItem *item);
      void addImageItem(QDate &section, IBImageListImageItem *item);
      IBImageListAbstractItem *getItemByLinearIndex(int index) const;
      int getLinearIndexOfItem(IBImageListAbstractItem *item) const;
      void clear();
      int totalSize() const;
      void sortImageItems(IBImageListModel::IBImageSortField field = IBImageListModel::SortByName,  
                          Qt::SortOrder order = Qt::AscendingOrder);
      void sortSections(Qt::SortOrder order = Qt::AscendingOrder);
};

/* class IBThumbnailLoader */

class IBThumbnailLoader : public QThread
{
   Q_OBJECT

   public:
     IBThumbnailLoader(QObject *parent = nullptr);
     IBThumbnailLoader(QSize &thumbsize, QList<IBImageListImageItem *> *data = nullptr, QObject *parent = nullptr); 

     void run() override;

     void setThumbnailSize(QSize &size);
     QSize getThumbnailSize() const;

     void setImageList(QList<IBImageListImageItem *> *data);
     QList<IBImageListImageItem *> *getImageList() const;

   signals:
      void imageLoaded(int index);

   private:
     /* represents a pointer to file data list to be handled */
     QList<IBImageListImageItem *> *lstFileData;
     /* size of the thumbnails */
     QSize szThumbnailSize;
};


#endif /*H_IBIMAGEMODEL*/
