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

#ifndef UNPLAYER_LIBRARYUTILS_H
#define UNPLAYER_LIBRARYUTILS_H

#include <QMimeDatabase>
#include <QObject>

class QFileInfo;
class QSqlDatabase;

namespace unplayer
{
    namespace tagutils
    {
        struct Info;
    }

    enum class MimeType
    {
        Flac,
        Mp4,
        Mp4b,
        Mpeg,
        VorbisOgg,
        FlacOgg,
        OpusOgg,
        Ape,
        Matroska,
        Wav,
        Wavpack,
        Other
    };

    MimeType mimeTypeFromString(const QString& string);

    class LibraryUtils : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool databaseInitialized READ isDatabaseInitialized CONSTANT)
        Q_PROPERTY(bool createdTable READ isCreatedTable CONSTANT)
        Q_PROPERTY(bool updating READ isUpdating NOTIFY updatingChanged)
        Q_PROPERTY(int artistsCount READ artistsCount NOTIFY databaseChanged)
        Q_PROPERTY(int albumsCount READ albumsCount NOTIFY databaseChanged)
        Q_PROPERTY(int tracksCount READ tracksCount NOTIFY databaseChanged)
        Q_PROPERTY(int tracksDuration READ tracksDuration NOTIFY databaseChanged)
        Q_PROPERTY(QString randomMediaArt READ randomMediaArt NOTIFY mediaArtChanged)
    public:
        static const QVector<QString> mimeTypesByExtension;
        static const QVector<QString> mimeTypesByContent;
        static LibraryUtils* instance();

        const QString& databaseFilePath();

        static QString findMediaArtForDirectory(QHash<QString, QString>& directoriesHash, const QString& directoryPath);

        void initDatabase();
        Q_INVOKABLE void updateDatabase();
        Q_INVOKABLE void resetDatabase();

        bool isDatabaseInitialized();
        bool isCreatedTable();
        bool isUpdating();

        int artistsCount();
        int albumsCount();
        int tracksCount();
        int tracksDuration();
        QString randomMediaArt();
        Q_INVOKABLE QString randomMediaArtForArtist(const QString& artist);
        Q_INVOKABLE QString randomMediaArtForAlbum(const QString& artist, const QString& album);

        Q_INVOKABLE void setMediaArt(const QString& artist, const QString& album, const QString& mediaArt);
    private:
        LibraryUtils();

        QString getTrackMediaArt(const tagutils::Info& info,
                                 QHash<QByteArray, QString>& embeddedMediaArtHash,
                                 const QFileInfo& fileInfo,
                                 QHash<QString, QString>& directoriesHash,
                                 bool useDirectoriesMediaArt);
        QString saveEmbeddedMediaArt(const QByteArray& data, QHash<QByteArray, QString>& embeddedMediaArtHash);

        bool mDatabaseInitialized;
        bool mCreatedTable;
        bool mUpdating;

        QString mDatabaseFilePath;
        QString mMediaArtDirectory;
        QMimeDatabase mMimeDb;
    signals:
        void updatingChanged();
        void databaseChanged();
        void mediaArtChanged();
    };
}

#endif // UNPLAYER_LIBRARYUTILS_H
