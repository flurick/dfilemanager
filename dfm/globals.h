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


#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QDir>

namespace DFM
{

enum IOTask { CopyTask = 0, MoveTask = 1, RemoveTask = 2 };

struct IOJobData
{
    QString inPaths, outPath; //used for interprocess communication
    QStringList inList;
    IOTask ioTask;
};

static int defaultInteger = 0;
static QDir::Filters allEntries = QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden|QDir::System;

namespace FS
{
enum Roles
{
    FileIconRole = Qt::DecorationRole,
    FileNameRole = Qt::DisplayRole,
    FilePathRole = Qt::UserRole + 1,
    FilePermissions = Qt::UserRole + 2,
    Category = Qt::UserRole + 3,
    MimeType = Qt::UserRole + 4,
    FileType = Qt::UserRole + 5,
    Url = Qt::UserRole + 6
};
}

}

#endif // GLOBALS_H