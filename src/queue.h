/*
 * Unplayer
 * Copyright (C) 2015-2017 Alexey Rochev <equeim@gmail.com>
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

#ifndef UNPLAYER_QUEUE_H
#define UNPLAYER_QUEUE_H

#include <memory>

#include <QObject>
#include <QQuickImageProvider>
#include <QStringList>
#include <QUrl>

namespace unplayer
{
    struct QueueTrack
    {
        explicit QueueTrack(const QString& filePath,
                            const QString& title,
                            int duration,
                            const QString& artist,
                            const QString& album,
                            const QString& mediaArtFilePath,
                            const QByteArray& mediaArtData);

        QString filePath;
        QString title;
        int duration;
        QString artist;
        QString album;

        QString mediaArtFilePath;
        QPixmap mediaArtPixmap;
    };

    class Queue : public QObject
    {
        Q_OBJECT

        Q_ENUMS(RepeatMode)

        Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

        Q_PROPERTY(QString currentFilePath READ currentFilePath NOTIFY currentTrackChanged)
        Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY currentTrackChanged)
        Q_PROPERTY(QString currentArtist READ currentArtist NOTIFY currentTrackChanged)
        Q_PROPERTY(QString currentAlbum READ currentAlbum NOTIFY currentTrackChanged)
        Q_PROPERTY(QUrl currentMediaArt READ currentMediaArt NOTIFY mediaArtChanged)

        Q_PROPERTY(bool shuffle READ isShuffle WRITE setShuffle NOTIFY shuffleChanged)
        Q_PROPERTY(RepeatMode repeatMode READ repeatMode NOTIFY repeatModeChanged)

        Q_PROPERTY(bool addingTracks READ isAddingTracks NOTIFY addingTracksChanged)
    public:
        enum RepeatMode
        {
            NoRepeat,
            RepeatAll,
            RepeatOne
        };

        explicit Queue(QObject* parent);

        const QList<std::shared_ptr<QueueTrack>>& tracks() const;

        int currentIndex() const;
        void setCurrentIndex(int index);

        QString currentFilePath() const;
        QString currentTitle() const;
        QString currentArtist() const;
        QString currentAlbum() const;
        QString currentMediaArt() const;

        bool isShuffle() const;
        void setShuffle(bool shuffle);

        RepeatMode repeatMode() const;
        Q_INVOKABLE void changeRepeatMode();
        void setRepeatMode(int mode);

        bool isAddingTracks() const;

        Q_INVOKABLE void addTrack(const QString& track);
        Q_INVOKABLE void addTracks(const QStringList& trackPaths, bool clearQueue = false, int setAsCurrent = -1);
        Q_INVOKABLE void removeTrack(int index);
        Q_INVOKABLE void removeTracks(QVector<int> indexes);
        Q_INVOKABLE void clear();

        Q_INVOKABLE void next();
        void nextOnEos();
        Q_INVOKABLE void previous();

        Q_INVOKABLE void setCurrentToFirstIfNeeded();
        Q_INVOKABLE void resetNotPlayedTracks();

    private:
        void reset();

    private:
        QList<std::shared_ptr<QueueTrack>> mTracks;
        QList<std::shared_ptr<QueueTrack>> mNotPlayedTracks;

        int mCurrentIndex;
        bool mShuffle;
        RepeatMode mRepeatMode;

        bool mAddingTracks;
    signals:
        void currentTrackChanged();

        void mediaArtChanged();

        void currentIndexChanged();
        void shuffleChanged();
        void repeatModeChanged();

        void tracksAdded(int start);
        void trackRemoved(int index);
        void tracksRemoved(const QVector<int>& indexes);
        void cleared();

        void addingTracksChanged();
    };

    class QueueImageProvider : public QQuickImageProvider
    {
    public:
        static const QString providerId;
        explicit QueueImageProvider(const Queue* queue);
        QPixmap requestPixmap(const QString& id, QSize*, const QSize& requestedSize) override;
    private:
        const Queue* mQueue;
    };
}

#endif // UNPLAYER_QUEUE_H
