/*
 * Unplayer
 * Copyright (C) 2015 Alexey Rochev <equeim@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "queue.h"
#include "queue.moc"

#include "utils.h"

namespace Unplayer
{

QueueTrack::QueueTrack(const QVariantMap &trackMap)
    : title(trackMap.value("title").toString()),
      url(trackMap.value("url").toString()),
      duration(trackMap.value("duration").toLongLong()),
      artist(trackMap.value("artist").toString()),
      album(trackMap.value("album").toString()),
      mediaArt(Utils::mediaArt(trackMap.value("rawArtist").toString(), trackMap.value("rawAlbum").toString(), url))
{

}

Queue::Queue(QObject *parent)
    : QObject(parent),
      m_currentIndex(-1),
      m_shuffle(false),
      m_repeat(false)
{

}

Queue::~Queue()
{
    qDeleteAll(m_tracks);
}

const QList<QueueTrack*>& Queue::tracks() const
{
    return m_tracks;
}

int Queue::currentIndex() const
{
    return m_currentIndex;
}

void Queue::setCurrentIndex(int index)
{
    m_currentIndex = index;
    emit currentIndexChanged();
}

QString Queue::currentTitle() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_tracks.length())
        return m_tracks.at(m_currentIndex)->title;
    return QString();
}

QString Queue::currentUrl() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_tracks.length())
        return m_tracks.at(m_currentIndex)->url;
    return QString();
}

int Queue::currentDuration() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_tracks.length())
        return m_tracks.at(m_currentIndex)->duration;
    return 0;
}

QString Queue::currentArtist() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_tracks.length())
        return m_tracks.at(m_currentIndex)->artist;
    return QString();
}

QString Queue::currentAlbum() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_tracks.length())
        return m_tracks.at(m_currentIndex)->album;
    return QString();
}

QString Queue::currentMediaArt() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_tracks.length())
        return m_tracks.at(m_currentIndex)->mediaArt;
    return QString();
}

bool Queue::isShuffle() const
{
    return m_shuffle;
}

void Queue::setShuffle(bool shuffle)
{
    m_shuffle = shuffle;
    emit shuffleChanged();

    if (!shuffle)
        resetNotPlayedTracks();
}

bool Queue::isRepeat() const
{
    return m_repeat;
}

void Queue::setRepeat(bool repeat)
{
    m_repeat = repeat;
    emit repeatChanged();

    if (m_shuffle && !repeat)
        resetNotPlayedTracks();
}

void Queue::add(const QVariantList &trackList)
{
    if (!trackList.isEmpty()) {
        QVariantList::const_iterator iterator = trackList.cbegin();
        while (iterator != trackList.cend()) {
            QueueTrack *track = new QueueTrack((*iterator).toMap());
            m_tracks.append(track);
            m_notPlayedTracks.append(track);
            iterator++;
        }
    }
}

void Queue::remove(int index)
{
    QueueTrack *track = m_tracks.takeAt(index);
    m_notPlayedTracks.removeOne(track);
    delete track;

    emit trackRemoved(index);

    if (index < m_currentIndex) {
        setCurrentIndex(m_currentIndex - 1);
    } else if (index == m_currentIndex) {
        if (m_tracks.isEmpty())
            setCurrentIndex(-1);
        else
            emit currentIndexChanged();
        emit currentTrackChanged();
    }
}

void Queue::clear()
{
    qDeleteAll(m_tracks);
    m_tracks.clear();
    m_notPlayedTracks.clear();
}

bool Queue::hasUrl(const QString &url)
{
    QList<QueueTrack*>::const_iterator iterator = m_tracks.cbegin();
    while (iterator != m_tracks.cend()) {
        if ((*iterator)->url == url)
            return true;
        iterator++;
    }

    return false;
}

void Queue::next()
{
    if (m_shuffle) {
        if (m_notPlayedTracks.length() == 1)
            resetNotPlayedTracks();
        m_notPlayedTracks.removeOne(m_tracks.at(m_currentIndex));
        setCurrentIndex(m_tracks.indexOf(m_notPlayedTracks.at(qrand() % m_notPlayedTracks.length())));
    } else {
        if (m_currentIndex == m_tracks.length() - 1)
            setCurrentIndex(0);
        else
            setCurrentIndex(m_currentIndex + 1);
    }

    emit currentTrackChanged();
}

bool Queue::nextOnEos()
{
    if (m_shuffle) {
        QueueTrack *track = m_tracks.at(m_currentIndex);
        m_notPlayedTracks.removeOne(track);
        if (m_notPlayedTracks.length() == 0) {
            if (m_repeat) {
                resetNotPlayedTracks();
                m_notPlayedTracks.removeOne(track);
            } else {
                return false;
            }
        }
        setCurrentIndex(m_tracks.indexOf(m_notPlayedTracks.at(qrand() % m_notPlayedTracks.length())));
    } else {
        if (m_currentIndex == m_tracks.length() - 1) {
            if (m_repeat)
                setCurrentIndex(0);
            else
                return false;
        } else {
            setCurrentIndex(m_currentIndex + 1);
        }
    }

    emit currentTrackChanged();

    return true;
}

void Queue::previous()
{
    if (m_shuffle)
        return;

    if (m_currentIndex == 0)
        setCurrentIndex(m_tracks.length() - 1);
    else
        setCurrentIndex(m_currentIndex - 1);

    emit currentTrackChanged();
}

void Queue::resetNotPlayedTracks()
{
    m_notPlayedTracks = m_tracks;
}

}
