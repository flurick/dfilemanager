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

#ifndef COLUMNSWIDGET_H
#define COLUMNSWIDGET_H

#include <QScrollArea>
#include <QHBoxLayout>

#include "filesystemmodel.h"
#include "columnsview.h"
#include "viewcontainer.h"

namespace DFM
{
class ViewContainer;
class ColumnsView;
class ColumnsWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit ColumnsWidget(QWidget *parent = 0);
    ~ColumnsWidget(){}
    void setModel(FS::Model *model);
    inline void setSelectionModel(QItemSelectionModel *model) { m_slctModel = model; }
    QModelIndex currentIndex();
    ColumnsView *currentView();
    inline ViewContainer *container() { return m_container; }
    void clearFrom(const QModelIndexList &list = QModelIndexList());
    void scrollTo(const QModelIndex &index);
    
public slots:
    void edit(const QModelIndex &index);
    void setRootIndex(const QModelIndex &index);
    void reconnectViews();
    void showCurrent();

protected:
    void connectView(ColumnsView *view);
    void showEvent(QShowEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    FS::Model *m_model;
    QItemSelectionModel *m_slctModel;
    QFrame *m_viewport;
    QHBoxLayout *m_viewLay;
    QMap<QPersistentModelIndex, ColumnsView *> m_map;
    static QMap<QUrl, int> m_widthMap;
    ViewContainer *m_container;
    QPersistentModelIndex m_rootIndex;
    QList<QUrl> m_rootList;
    bool m_isResizingColumns;
    friend class ColumnsView;
    friend class ResizeCorner;
};

}

#endif // COLUMNSWIDGET_H
