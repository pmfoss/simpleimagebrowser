/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#include "ibmainwindow.hpp"

/* Constructs the main window and all its components. */
IBMainWindow::IBMainWindow(QWidget *parent, Qt::WindowFlags flags)
   : QMainWindow(parent, flags)
{
   QActionGroup *hactgrp;
   QMenu *hsubmn;
   QAction *hmnact;

   this->setWindowTitle(QStringLiteral("Simple Image Browser"));
   /* toolbar */
   this->tbMain = new QToolBar(this);  
   this->tbMain->setFloatable(false);
   this->tbMain->setMovable(false);
   
   /* toolbar button for navigation */
   this->tbHistoryBack = new QToolButton(this->tbMain);
   this->tbHistoryBack->setIcon(QIcon::fromTheme(QStringLiteral("go-previous")));
   this->tbHistoryBack->setEnabled(false);
   this->tbMain->addWidget(this->tbHistoryBack);

   this->tbHistoryForward = new QToolButton(this->tbMain);
   this->tbHistoryForward->setIcon(QIcon::fromTheme(QStringLiteral("go-next")));
   this->tbHistoryForward->setEnabled(false);
   this->tbMain->addWidget(this->tbHistoryForward);

   this->tbHome = new QToolButton(this->tbMain);
   this->tbHome->setIcon(QIcon::fromTheme(QStringLiteral("go-home")));
   this->tbMain->addWidget(this->tbHome);
   
   this->tbRefresh = new QToolButton(this->tbMain);
   this->tbRefresh->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
   this->tbMain->addWidget(this->tbRefresh);

   /* file system combobox */
   this->cbPath = new IBFileComboBox(this);
   this->cbPath->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
   this->cbPath->setPath(QDir::homePath());
   this->tbMain->addWidget(cbPath);

   this->connect(this->tbHistoryBack, SIGNAL(clicked()), this->cbPath, SLOT(goHistoryBack()));
   this->connect(this->tbHistoryForward, SIGNAL(clicked()), this->cbPath, SLOT(goHistoryForward()));
   this->connect(this->tbHome, SIGNAL(clicked()), this->cbPath, SLOT(goToHomeDirectory()));

   /* menu with button */
   this->tbMenu = new QToolButton(this->tbMain);
   this->tbMenu->setIcon(QIcon::fromTheme(QStringLiteral("application-menu")));
   this->tbMenu->setPopupMode(QToolButton::InstantPopup);

   this->mnMain = new QMenu(this->tbMenu);

   /* section submenu */
   hsubmn = mnMain->addMenu(QStringLiteral("Sections"));
   hactgrp = new QActionGroup(hsubmn);
   hsubmn->addSection(QStringLiteral("Sectioning by"));
 
   this->createNewMenuAction(hsubmn, QStringLiteral("None"), true, true,
                             IBMainWindow::ActionFlag_SectionNone, hactgrp);

   this->createNewMenuAction(hsubmn, QStringLiteral("Alphabetic"), false, true, 
                             IBMainWindow::ActionFlag_SectionAlphabetic, hactgrp);

   this->createNewMenuAction(hsubmn, QStringLiteral("Date"), false, true,
                             IBMainWindow::ActionFlag_SectionDate, hactgrp);

   this->createNewMenuAction(hsubmn, QStringLiteral("Filetype"), false, true,
                             IBMainWindow::ActionFlag_SectionFileType, hactgrp);

   hsubmn->addSection(QStringLiteral("Sorting order"));
   hactgrp = new QActionGroup(hsubmn);

   this->createNewMenuAction(hsubmn, QStringLiteral("Ascending"), true, true,
                             IBMainWindow::ActionFlag_Section | IBMainWindow::ActionFlag_SortAscending,
                             hactgrp);

   this->createNewMenuAction(hsubmn, QStringLiteral("Descending"), false, true, 
                             IBMainWindow::ActionFlag_Section | IBMainWindow::ActionFlag_SortDescending,
                             hactgrp);
    
   /* image submenu */
   hsubmn = mnMain->addMenu(QStringLiteral("Images"));

   hsubmn->addSection(QStringLiteral("Sorting by"));
   hactgrp = new QActionGroup(hsubmn);

   this->createNewMenuAction(hsubmn, QStringLiteral("Name"), true, true,
                             IBMainWindow::ActionFlag_SortImageName, hactgrp);

   this->createNewMenuAction(hsubmn, QStringLiteral("Date"), false, true,
                             IBMainWindow::ActionFlag_SortImageDate, hactgrp);

   
   this->createNewMenuAction(hsubmn, QStringLiteral("Filetype"), false, true,
                             IBMainWindow::ActionFlag_SortImageFileType, hactgrp);

   hsubmn->addSection(QStringLiteral("Sorting order"));
   hactgrp = new QActionGroup(hsubmn);

   this->createNewMenuAction(hsubmn, QStringLiteral("Ascending"), true, true, 
                             IBMainWindow::ActionFlag_Image | IBMainWindow::ActionFlag_SortAscending,
                             hactgrp);

   this->createNewMenuAction(hsubmn, QStringLiteral("Descending"), false, true,
                             IBMainWindow::ActionFlag_Image | IBMainWindow::ActionFlag_SortDescending,
                             hactgrp);

   this->mnMain->addSeparator();

   hmnact = this->mnMain->addAction(QStringLiteral("About"));
   hmnact->setData(IBMainWindow::ActionFlag_About);

   hmnact = this->mnMain->addAction(QStringLiteral("About Qt"));
   hmnact->setData(IBMainWindow::ActionFlag_AboutQt);

   this->mnMain->addSeparator();

   hmnact = this->mnMain->addAction(QStringLiteral("Close"));
   hmnact->setShortcuts(QKeySequence::Close);   
   this->connect(hmnact, &QAction::triggered, this, &IBMainWindow::close);

   this->connect(this->mnMain, SIGNAL(triggered(QAction *)), SLOT(onMenuTriggered(QAction *)));

   this->tbMenu->setMenu(mnMain);
   this->tbMain->addWidget(this->tbMenu);

   this->addToolBar(this->tbMain);

   /* central widget */
   this->ilwView = new IBImageListWidget(this);
   this->ilwView->setImagePath(QDir::homePath());
   this->connect(ilwView, SIGNAL(selectionChanged(const QModelIndex &)), SLOT(onImageWidgetSelectionChanged(const QModelIndex &)));
   this->connect(this->tbRefresh, SIGNAL(clicked()), this->ilwView, SLOT(refresh()));

   this->iiwPreview = new IBImageInfoWidget(this);
   this->iiwPreview->hide();
   this->iiwPreview->setMinimumWidth(100);

   this->swCentralWidget = new QSplitter(Qt::Horizontal, this);
   this->swCentralWidget->addWidget(this->ilwView);
   this->swCentralWidget->addWidget(this->iiwPreview);

   this->setCentralWidget(swCentralWidget);

   this->connect(this->cbPath, SIGNAL(pathChanged(const QString &)), 
                 this, SLOT(onPathChanged(const QString &)));
}

/* Creates a new menu action and adds it to the parent menu (parentmenu) and an action group (actiongroup). */
inline void IBMainWindow::createNewMenuAction(QMenu *parentmenu, const QString &text, bool checked, bool checkable, 
                                              const uint data, QActionGroup *actiongroup)
{
   QAction *action;

   action = parentmenu->addAction(text);
   action->setCheckable(checkable);
   action->setChecked(checked);
   action->setData(data);
    
   if(actiongroup)
   {
      actiongroup->addAction(action);
   }
}



/* Handles a click on a menu entry. */
void IBMainWindow::onMenuTriggered(QAction *action)
{
   uint data;

   if(action)
   {
      data = action->data().toUInt();      
      action->setChecked(true);

      switch(data & IBMainWindow::ActionFlag_ActionMask)
      {
         case IBMainWindow::ActionFlag_SortAscending:
            if((data & ActionFlag_TypeMask) == IBMainWindow::ActionFlag_Image)
            {
               this->ilwView->setImageSortOrder(Qt::AscendingOrder);
            }
            else
            {
               this->ilwView->setSectionSortOrder(Qt::AscendingOrder);
            }
            break;

         case IBMainWindow::ActionFlag_SortDescending:
            if((data & ActionFlag_TypeMask) == IBMainWindow::ActionFlag_Image)
            {
               this->ilwView->setImageSortOrder(Qt::DescendingOrder);
            }
            else
            {   
               this->ilwView->setSectionSortOrder(Qt::DescendingOrder);
            }
            break;

         case IBMainWindow::ActionFlag_SectionNone:
            this->ilwView->setSectionType(IBImageListModel::NoSection);
            break;

         case IBMainWindow::ActionFlag_SectionAlphabetic:
            this->ilwView->setSectionType(IBImageListModel::AlphabeticSection);
            break;

         case IBMainWindow::ActionFlag_SectionDate:
            this->ilwView->setSectionType(IBImageListModel::DateSection);
            break;

         case IBMainWindow::ActionFlag_SectionFileType:
            this->ilwView->setSectionType(IBImageListModel::FileTypeSection);
            break;

         case IBMainWindow::ActionFlag_SortImageName:
            this->ilwView->setImageSortField(IBImageListModel::SortByName);
            break;

         case IBMainWindow::ActionFlag_SortImageDate:
            this->ilwView->setImageSortField(IBImageListModel::SortByDate);
            break;

         case IBMainWindow::ActionFlag_SortImageFileType:
            this->ilwView->setImageSortField(IBImageListModel::SortByFileType);
            break;

         default:
            switch(data & ActionFlag_TypeMask)
            {
               case IBMainWindow::ActionFlag_About:
                  QMessageBox::about(this, QStringLiteral("About"), QString(
                     "<p><h3>About Simple Image Browser</h3></p>"
                     "<p>This project demonstrates a way to customize the QListView class of the Qt Framework with sections and custom items."
                     "   It does not claim to be completed or to provide and support a program for productive use.</p>"
                     "<p>The source code is licensed under the BSD-3-Clause-License and published on "
                     "<a href=\"https://github.com/pmfoss/simpleimagebrowser\">Github</a></p>"
                     "<p>Copyright (C) 2022 Martin Pietsch &lt;@pmfoss&gt;</p>"));
                  break;

               case IBMainWindow::ActionFlag_AboutQt:
                  QMessageBox::aboutQt(this, QStringLiteral("About Qt")); 
                  break;
            }
            break;
      }
   }
}

/* Hides the preview widget, enables/disables history toolbar button
   and sets the new path to the list view widget */
void IBMainWindow::onPathChanged(const QString &newpath)
{
   this->tbHistoryBack->setEnabled(!this->cbPath->isHistoryAtFirstIndex());
   this->tbHistoryForward->setEnabled(!this->cbPath->isHistoryAtLastIndex());
   this->iiwPreview->hide();
   this->ilwView->setImagePath(newpath);
}

/* Shows or hides the image preview widget. */
void IBMainWindow::onImageWidgetSelectionChanged(const QModelIndex &index)
{
   bool issection;
   QString imagepath;

   if(!index.isValid())
   {
      this->iiwPreview->hide();
      return;
   }

   issection = index.model()->data(index, IBImageListModel::ItemIsSection).toBool();
   
   if(!issection)
   {
      this->iiwPreview->show();
      imagepath = index.model()->data(index, IBImageListModel::ItemFilePath).toString(); 
      this->iiwPreview->setImagePath(imagepath);
      this->iiwPreview->show();
   }
   else
   {
      this->iiwPreview->hide();
   }
}
