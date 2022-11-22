/* Copyright (C) 2022 Martin Pietsch <@pmfoss>
   SPDX-License-Identifier: BSD-3-Clause */

#include <QApplication>
#include "ibmainwindow.hpp"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  IBMainWindow mainwin;

  mainwin.showMaximized();

  return app.exec();
}
