/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#include "ibfilecombobox.hpp"

/* class IBFileSystemModel */

/* Constructs a file system model. */
IBFileSystemModel::IBFileSystemModel(QObject *parent)
   : QFileSystemModel(parent)
{
}

/* reimpl. */
int IBFileSystemModel::columnCount(const QModelIndex &parent) const
{
   Q_UNUSED(parent);

   return 1;
}

/* class IBFileComboBox */

/* Constructs a combobox for selecting a file system path. */
IBFileComboBox::IBFileComboBox(QWidget *parent)
   : QComboBox(parent)
{
   this->initComboBox();
}

/* Constructs a combobox for selecting a file system path with a given file system path (path). */
IBFileComboBox::IBFileComboBox(const QString &path, QWidget *parent)
   : QComboBox(parent)
{
   this->initComboBox();
   this->setPath(path);
}

/* Initializes all components of the combobox (TreeView, FileSystemModel). */
void IBFileComboBox::initComboBox()
{
   this->iCurrentHistoryIndex = -1;
   this->setEditable(true);
   this->connect(this, SIGNAL(currentIndexChanged(int)), SLOT(onIndexChanged(int)));

   this->fsModel = new IBFileSystemModel(this);
   this->fsModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Drives);
   this->tvView = new QTreeView(this);
   this->tvView->setHeaderHidden(true);
   this->tvView->viewport()->installEventFilter(this);

   this->setModel(this->fsModel);
   this->setView(this->tvView);
   this->bSkipNextHide = false;
   this->fsModel->setRootPath(QDir::rootPath());
   this->setRootModelIndex(this->fsModel->index(QDir::rootPath()));

   this->slHistory.clear();
   this->goToHomeDirectory();
}

/* Decreases the current history index */
void IBFileComboBox::goHistoryBack()
{
   if(this->iCurrentHistoryIndex > 0)
   {
      this->navigateTo(this->slHistory[--this->iCurrentHistoryIndex], true);
   }
}

/* Increases the current history index */
void IBFileComboBox::goHistoryForward()
{
   if(this->iCurrentHistoryIndex < this->slHistory.size())
   {
      this->navigateTo(this->slHistory[++this->iCurrentHistoryIndex], true);
   }
}

/* Navigates to the home directory */
void IBFileComboBox::goToHomeDirectory()
{
   this->navigateTo(QDir::homePath());
}

/* Sets the path (path) to the combo box and emits the signal pathChanged. If the path is not from the history operations 
   (fromhistory == false) the history is rewound to the current history index. */
void IBFileComboBox::navigateTo(const QString &path, bool fromhistory)
{
   if(this->slHistory.isEmpty() || (!fromhistory && this->slHistory.last() != path))
   {
      if(this->iCurrentHistoryIndex > 0)
      {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
         this->slHistory.remove(this->iCurrentHistoryIndex, this->slHistory.size() - this->iCurrentHistoryIndex);
#else
         while(this->iCurrentHistoryIndex < this->slHistory.size() - 1)
         {
            this->slHistory.removeLast();
         }
#endif /*(QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) */
      }
      this->slHistory.append(path);
      this->iCurrentHistoryIndex = this->slHistory.size() - 1;
   }

   this->setCurrentText(path);
   emit this->pathChanged(path);
}

/* Sets the path (newpath) to be used. */
void IBFileComboBox::setPath(const QString &newpath)
{
   this->navigateTo(newpath, false);
}

/* Returns the currently used path. */
QString IBFileComboBox::getPath() const
{
    return this->currentText();
}

/* Filters the Enter- and Return-KeyPressed event for clearing the objects focus. */
bool IBFileComboBox::event(QEvent *e)
{
   if(e->type() == QEvent::KeyPress)
   {
      QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(e);
      
      if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
      {
         this->clearFocus();
         this->navigateTo(this->currentText(), false);
         return true;
      }
   } 
   return QComboBox::event(e); 
}

/* Filters the MouseButtonPress event on the viewport of the treeview object 
   for preventing premature closing of the popup widget. */
bool IBFileComboBox::eventFilter(QObject* object, QEvent* event)
{
   if(event->type() == QEvent::MouseButtonPress && object == view()->viewport())
   {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent *>(event); 
      QModelIndex index = this->tvView->indexAt(mouseEvent->pos());
      if(!this->tvView->visualRect(index).contains(mouseEvent->pos()))
      {
         this->bSkipNextHide = true;
      }
   }
   
   return false;
}

/* Prepare the treeview object with expanding items to the currently used path
   and popup the treeview. */
void IBFileComboBox::showPopup()
{
   QDir expdir = this->currentText();

   this->tvView->setCurrentIndex(this->fsModel->index(this->currentText()));
   this->tvView->collapseAll();
   
   do
   {   
      this->tvView->setExpanded(this->fsModel->index(expdir.absolutePath()), true);
   }while(expdir.cdUp());

   QComboBox::showPopup();
}

/* Prevent the hiding of the popup widget, when an treeview item is clicked. */
void IBFileComboBox::hidePopup()
{
   if(this->bSkipNextHide)
   {
      this->bSkipNextHide = false;
   }
   else
   {
      QComboBox::hidePopup();
   }
}

/* Sets the selected path to the combobox. */
void IBFileComboBox::onIndexChanged(int index)
{
   Q_UNUSED(index)

   this->navigateTo(this->fsModel->filePath(this->tvView->currentIndex()), false);
}
