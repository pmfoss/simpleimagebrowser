/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#ifndef H_IBMAINWINDOW
#define H_IBMAINWINDOW

#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>

#include "ibfilecombobox.hpp"
#include "ibimagelistwidget.hpp"
#include "ibimagelistmodel.hpp"
#include "ibimageinfowidget.hpp"

class IBMainWindow : public QMainWindow
{
  Q_OBJECT

  public:
  
    /* enumerations with ids for menu actions */
    enum ActionFlags
    {
       ActionFlag_SortAscending = 0x01,
       ActionFlag_SortDescending = 0x02,
       ActionFlag_SectionNone = 0x03,
       ActionFlag_SectionAlphabetic = 0x04,
       ActionFlag_SectionDate = 0x05,
       ActionFlag_SectionFileType = 0x06,
       ActionFlag_SortImageName = 0x07,
       ActionFlag_SortImageDate = 0x08,
       ActionFlag_SortImageFileType = 0x09,
       ActionFlag_ActionMask = 0x0F,
       ActionFlag_Section = 0x10,
       ActionFlag_Image = 0x20,
       ActionFlag_About = 0x30,
       ActionFlag_AboutQt = 0x40,
       ActionFlag_TypeMask = 0xF0
    };
    Q_ENUM(ActionFlags)

    IBMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    
  private slots:
    void onMenuTriggered(QAction *action);
    void onPathChanged(const QString &newpath);
    void onImageWidgetSelectionChanged(const QModelIndex &index);

  private:
    inline void createNewMenuAction(QMenu *parentmenu, const QString &text, bool checked = false, bool checkable = false, 
                                    const uint data = 0, QActionGroup *actiongroup = nullptr);
    
    /* main menu assigned to the tool button tbMenu */
    QMenu *mnMain;
    /* main toolbar */
    QToolBar *tbMain;
    /* menu toolbar button*/
    QToolButton *tbMenu;
    /* path label */
    QLabel *lblPath;
    /* central widget */
    QSplitter *swCentralWidget;
    /* combobox to selecting working directory */
    IBFileComboBox *cbPath;
    /* list view */
    IBImageListWidget *ilwView;
    /* preview widget */
    IBImageInfoWidget *iiwPreview;
};

#endif
