# Simple Image Browser

[Screenshot](./screenshot.png "Screenshot")

## Description

This project demonstrates a way to customize the class `QListView` of the Qt framework. It does not claim to be completed or 
to provide and support a program for productive use. The usable versions are Qt5 (tested 5.15) and Qt6 (tested 6.4).

The main goals are:

* customize items,
* showing or hiding of section labels and
* sorting of these parts separately from each other.

The following diagram shows the developed classes starting with the abbreviation `IB` (**I**mage **B**rowser) and its relations to realize the goals. All classes with a starting `Q` are part of Qt framework.

[classes](./classes.svg)

## Goals

This section descripts the realization of the named goals. All mentioned components are joined in class `IBImageListWidget`, which is implemented in the files `ibimagelistwidget.hpp` and `ibimagelistwidget.cpp`.

### Customize items

The customization of `QListView` items is very simple. Its generalizing class `QAbstractItemView` provides the possiblity to use an object of the class `QAbstractItemDelegate` which provides the method `paint`. The specializion to this class with the overriding of this method makes it possible to customize the items. The delegate class has another advantage. It supplies the size of an item for the `QListView` class and it is possible to display items with different sizes. The implementation of this part is located in the files `ibitemdelegate.hpp` and `ibitemdelegate.cpp`.

### Section labels

The goal for showing or hiding of section labels is closely related to the `QListView` data/items. For the realizion a so-called model (`QAbstractItemModel`) is necessary which is used by a `QAbstractItemView` to display items. It manages and prepares the displaying data. The classes of `QAbstractItemView` are only responsible for the rendering of the items. A detailed overview of that can be found in the [Qt Documentation](https://doc.qt.io/qt-6/model-view-programming.html). A `QListView` instance can only handle 1-dimensional models or one specific column of a multidimensional model.

In case of the project, it is advantageous to organize the data in a 2-dimensional structure consisting of section items (`IBImageListSectionItem`) and images items (`IBImageListImageItem`) that stored in a main list (`IBImageListSectionList`). This simply helps to achieve the stated goals. The image items contains all image data (path, size, thumbnail, last modifed date and so on) and are stored in a list of the section items. The section items, in turn,  are only consists of an ID and the list of image items. The main list finally stores and manages the section items with its image items and transforms the 2-dimensional data in 1-dimensional data. The tasks of the project model (`IBImageListModel`) are to populate the structure, to invoke its managment and to interface the data to the `QListView` instances.

If no section labels are to be displayed, the structure contains only one section item with an empty ID and all image items.

The item property of having different sizes, described in section `Customize items`, allows displaying section and image items. Section items are span the entired width of the view and must resize when the view is resized. The image items have a static size. 

### Sorting

Sorting the different parts is very simple with the described structure. The class `IBImageListSectionItem` sorts the image items and the class `IBImageListSectionList` sorts the sections.

### Others

In Addition, the model class initialize a thread instance of class (`IBThumbnailLoader`), that loads the thumbnails of the image items. This approach allows a nonblocking use of the resulting application.  

The implementation of this part is located in the files `ibimagelistmodel.hpp` and `ibimagelistmodel.hpp`.

## Compiling

```
git clone https://github.com/pmfoss/simpleimagebrowser
cd simpleimagebrowser 
qmake
make
./simpleimagebrowser
```

## License

BSD-3-License
