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


#include "thumbsloader.h"
#include <QImage>
#include "columnsview.h"
#include "mainwindow.h"
#include "operations.h"
#include "config.h"
#include "viewcontainer.h"
#include "application.h"
#include <QBitmap>
#include <QMutexLocker>

using namespace DFM;

ThumbsLoader *ThumbsLoader::m_instance = 0;

ThumbsLoader::ThumbsLoader(QObject *parent) :
    Thread(parent),
    m_extent(256)
{
    connect(APP,SIGNAL(aboutToQuit()),this,SLOT(discontinue()));
    start();
}

ThumbsLoader
*ThumbsLoader::instance()
{
    if (!m_instance)
        m_instance = new ThumbsLoader(APP);
    return m_instance;
}

void
ThumbsLoader::fileRenamed(const QString &path, const QString &oldName, const QString &newName)
{
    const QString &file = QDir(path).absoluteFilePath(oldName);
    _removeThumb(file);
}

void
ThumbsLoader::_removeThumb(const QString &file)
{
    QMutexLocker locker(&m_thumbsMutex);
    if (m_thumbs.contains(file))
    {
        m_thumbs.remove(file);
        m_dateCheck.remove(file);
    }
}

bool
ThumbsLoader::_hasIcon(const QString &dir) const
{
    QMutexLocker locker(&m_thumbsMutex);
    return m_icons.contains(dir);
}

QString
ThumbsLoader::_icon(const QString &dir) const
{
    QMutexLocker locker(&m_thumbsMutex);
    if (m_icons.contains(dir))
        return m_icons.value(dir);
    return QString();
}

void
ThumbsLoader::_removeIcon(const QString &dir)
{
    QMutexLocker locker(&m_thumbsMutex);
    if (m_icons.contains(dir))
        m_icons.remove(dir);
}

bool
ThumbsLoader::_hasThumb(const QString &file) const
{
    QMutexLocker locker(&m_thumbsMutex);
    return m_thumbs.contains(file) && m_dateCheck.value(file) == QFileInfo(file).lastModified().toString();
}

void
ThumbsLoader::clearQ()
{
    QMutexLocker locker(&m_queueMutex);
    m_queue.clear();
}

void
ThumbsLoader::_queueFile(const QString &file)
{
    if (_hasThumb(file))
        return;
    if (_hasIcon(file))
        return;
    m_queueMutex.lock();
    const bool inQueue = m_queue.contains(file);
    m_queueMutex.unlock();
    if (inQueue)
    {
        m_queueMutex.lock();
        int i = m_queue.indexOf(file);
        m_queue.move(i, qMax(0, i-10));
        m_queueMutex.unlock();
        return;
    }
    m_queueMutex.lock();
    m_queue << file;
    m_queueMutex.unlock();
    if (isPaused())
        setPause(false);
}

QImage
ThumbsLoader::_thumb(const QString &file) const
{
    QMutexLocker locker(&m_thumbsMutex);
    if (m_thumbs.contains(file))
        return m_thumbs.value(file);
    return QImage();
}

void
ThumbsLoader::genThumb(const QString &path)
{
    const QFileInfo fi(path);
    if (!fi.isReadable())
    {
        _removeThumb(path);
        return;
    }
#if defined(Q_OS_UNIX)
    if (fi.isDir())
    {
        const QString &dirFile(fi.absoluteFilePath());
        const QString &settingsFile(QDir(dirFile).absoluteFilePath(".directory"));
        const QSettings settings(settingsFile, QSettings::IniFormat);
        const QString &iconName = settings.value("Desktop Entry/Icon").toString();
        if (!iconName.isEmpty())
            if (!hasIcon(dirFile))
            {
                m_thumbsMutex.lock();
                m_icons.insert(dirFile, iconName);
                m_thumbsMutex.unlock();
                emit thumbFor(path, iconName);
            }
        return;
    }
#endif
    QImage image;
    if (!APP->activeThumbIfaces().isEmpty())
        for (int i = 0; i<APP->activeThumbIfaces().count(); ++i)
            if (APP->activeThumbIfaces().at(i)->thumb(path, m_extent, image))
            {
                m_thumbsMutex.lock();
                m_dateCheck.insert(path, fi.lastModified().toString());
                m_thumbs.insert(path, image);
                m_thumbsMutex.unlock();
                emit thumbFor(path, QString());
                return;
            }
}

void
ThumbsLoader::run()
{
    foreach (ThumbInterface *ti, APP->thumbIfaces())
        ti->init();

    while (!m_quit)
    {
        while (!m_queue.isEmpty())
        {
            m_queueMutex.lock();
            const QString &file = m_queue.takeFirst();
            m_queueMutex.unlock();
            genThumb(file);
        }
        setPause(!m_quit);
        pause();
    }
}

///////////////////////////////////////////////////////////////////////

static QHash<QString, QImage> s_themeIcons[2];

ImagesThread::ImagesThread(QObject *parent)
    : Thread(parent)
{}

void
ImagesThread::removeData(const QString &file)
{
    QMutexLocker locker(&m_thumbsMutex);
    for (int i = 0; i < 2; ++i)
        m_images[i].remove(file);
    m_imgQueue.remove(file);
}

void
ImagesThread::genImagesFor(const QString &file)
{
    m_queueMutex.lock();
    const QImage &source = m_imgQueue.take(file);
    m_queueMutex.unlock();
    if (!source.isNull())
    {
        m_images[0].insert(file, Ops::flowImg(source));
        m_images[1].insert(file, Ops::reflection(source));
        emit imagesReady(file);
    }
}

void
ImagesThread::genNameIconsFor(const QString &name)
{
    m_queueMutex.lock();
    const QImage &source = m_nameQueue.take(name);
    m_queueMutex.unlock();
    if (!source.isNull())
    {
        s_themeIcons[0].insert(name, Ops::flowImg(source));
        s_themeIcons[1].insert(name, Ops::reflection(source));
    }
}

void ImagesThread::run()
{
    while (!m_quit)
    {
        while (!m_nameQueue.isEmpty())
        {
            m_queueMutex.lock();
            const QString &key = m_nameQueue.keys().first();
            m_queueMutex.unlock();
            genNameIconsFor(key);
        }
        while (!m_imgQueue.isEmpty())
        {
            m_queueMutex.lock();
            const QString &key = m_nameQueue.keys().first();
            m_queueMutex.unlock();
            genImagesFor(key);
        }
        setPause(!m_quit);
        pause();
    }
}

QImage
ImagesThread::flowData(const QString &file, const bool refl)
{
    QMutexLocker locker(&m_thumbsMutex);
    if (m_images[refl].contains(file))
        return m_images[refl].value(file);
    return QImage();
}

QImage
ImagesThread::flowNameData(const QString &name, const bool refl)
{
    QMutexLocker locker(&m_thumbsMutex);
    if (s_themeIcons[refl].contains(name))
        return s_themeIcons[refl].value(name);
    return QImage();
}

bool
ImagesThread::hasData(const QString &file)
{
    QMutexLocker locker(&m_thumbsMutex);
    return m_images[0].contains(file);
}

bool
ImagesThread::hasNameData(const QString &name)
{
    QMutexLocker locker(&m_thumbsMutex);
    return s_themeIcons[0].contains(name);
}

void
ImagesThread::queueName(const QIcon &icon)
{
    if (hasNameData(icon.name()))
        return;
    m_queueMutex.lock();
    const bool inQueue = m_nameQueue.contains(icon.name());
    m_queueMutex.unlock();
    if (inQueue)
        return;

    const QImage &source = icon.pixmap(SIZE).toImage();
    m_queueMutex.lock();
    m_nameQueue.insert(icon.name(), source);
    m_queueMutex.unlock();
    if (isPaused())
        setPause(false);
    if (!isRunning())
        start();
}

void
ImagesThread::queueFile(const QString &file, const QImage &source, const bool force)
{
    if (hasData(file) && !force)
        return;
    m_queueMutex.lock();
    const bool inQueue = m_imgQueue.contains(file);
    m_queueMutex.unlock();
    if (inQueue && !force)
        return;

    m_queueMutex.lock();
    m_imgQueue.insert(file, source);
    m_queueMutex.unlock();

    if (isPaused())
        setPause(false);
    if (!isRunning())
        start();
}
