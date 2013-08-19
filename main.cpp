/**************************************************************************
*   Copyright (C) 2013 by Robert Metsaranta                               *
*   therealestrob@gmail.com                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include <QSharedMemory>
#include "application.h"
#include "mainwindow.h"
#include "config.h"

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(resources);

    QSharedMemory mem("dfmkeys");
    bool isRunning;
    if ( mem.attach() )
        isRunning = true;
    else
    {
        isRunning = false;
        if ( !mem.create(1) )
            qDebug() << "mem creation failed";
    }

    if ( isRunning )
    {
        qDebug() << "dfm is already running... exiting";
        return 0;
    }

#if QT_VERSION < 0x050000
    QApplication::setGraphicsSystem("raster");
#endif
    QApplication app(argc, argv);

    DFM::Configuration::readConfig();

    if ( !DFM::Configuration::config.styleSheet.isEmpty() )
    {
        QFile file(DFM::Configuration::config.styleSheet);
        file.open(QFile::ReadOnly);
        app.setStyleSheet(file.readAll());
        file.close();
    }

    DFM::MainWindow *mainWin = new DFM::MainWindow(app.arguments());
    mainWin->show();
    return app.exec();
}

