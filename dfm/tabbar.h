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


#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QToolBar>
#include <QWidget>
#include <QMouseEvent>
#include <QDrag>

#include "mainwindow.h"

namespace DFM
{
class MainWindow;

class WindowFrame : public QWidget
{
public:
    inline explicit WindowFrame(QWidget *parent = 0) : QWidget(parent) {setAttribute(Qt::WA_TransparentForMouseEvents);}
protected:
    void paintEvent(QPaintEvent *);

};

class WinButton : public QFrame
{
    Q_OBJECT
public:
    enum Type { Close = 0, Min, Max, Other };
    WinButton(Type t = Other, QWidget *parent = 0);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private slots:
    void toggleMax();

private:
    Type m_type;
    MainWindow *m_mainWin;
    bool m_hasPress;
};

class TabBar;

class FooBar : public QWidget
{
    Q_OBJECT
public:
    enum TabShape { Standard = 0, Chrome };
    explicit FooBar(QWidget *parent = 0);
    void setTabBar(TabBar *tabBar);
    static int headHeight(MainWindow *win);
    static QLinearGradient headGrad(MainWindow *win);
    static QPainterPath tab( const QRect &r, int round = 4, TabShape shape = Standard );

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    QRegion shape();

private slots:
    void correctTabBarHeight();

private:
    TabBar *m_tabBar;
    QToolBar *m_toolBar;
    int m_topMargin;
    bool m_hasPress;
    QPoint m_pressPos;
    MainWindow *m_mainWin;
    WindowFrame *m_frame;
    friend class TabBar;
};

class TabCloser : public WinButton
{
public:
    inline explicit TabCloser(QWidget *parent = 0) : WinButton(WinButton::Other, parent) {}
protected:
    void paintEvent(QPaintEvent *);
};

class TabButton : public WinButton
{
public:
    inline explicit TabButton(QWidget *parent = 0) : WinButton(WinButton::Other, parent) {setFixedSize(28, 18);}
protected:
    void paintEvent(QPaintEvent *e);
};

class DropIndicator : public QWidget
{
public:
    inline explicit DropIndicator(QWidget *parent = 0) : QWidget(parent){}
protected:
    void paintEvent(QPaintEvent *);
};

class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget *parent = 0);
    inline void setAddTabButton(QWidget *addButton);
    
signals:
    void newTabRequest();

protected:
    QSize tabSizeHint(int index) const;
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void tabInserted(int index);
    void tabRemoved(int index);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
#if 0
    bool eventFilter(QObject *, QEvent *);
#endif
    void correctAddButtonPos();
    void drawTab( QPainter *p, int index );

private slots:
    void tabCloseRequest();
    void newWindowTab(int tab);
#if 0
    void drag() { if (mouseGrabber()&&!m_mouseGrabber) {m_mouseGrabber=mouseGrabber();m_mouseGrabber->installEventFilter(this);}}
#endif

private:
    void genNewTabButton();
    friend class FooBar;
    int m_hoveredTab;
    bool m_hasPress, m_dragCancelled;
    QWidget *m_addButton, *m_mouseGrabber;
    DropIndicator *m_dropIndicator;
    QString m_lastDraggedFile;
};

}

#endif // TABBAR_H