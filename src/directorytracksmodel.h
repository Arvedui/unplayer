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

#ifndef UNPLAYER_DIRECTORYTRACKSMODEL_H
#define UNPLAYER_DIRECTORYTRACKSMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>

#include "filterproxymodel.h"

class QSparqlConnection;
class QSparqlResult;

namespace Unplayer
{

struct DirectoryTrackFile;

class DirectoryTracksModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)
    Q_PROPERTY(QString parentDirectory READ parentDirectory NOTIFY directoryChanged)
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(int tracksCount READ tracksCount NOTIFY loadedChanged)
public:
    DirectoryTracksModel();
    ~DirectoryTracksModel();
    void classBegin();
    void componentComplete();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex&) const;

    QString directory() const;
    void setDirectory(QString newDirectory);

    QString parentDirectory() const;
    bool isLoaded() const;
    int tracksCount() const;

    Q_INVOKABLE QVariantMap get(int fileIndex) const;
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void loadDirectory();
    void onQueryFinished();
private:
    QList<DirectoryTrackFile*> m_files;
    int m_rowCount;

    QSparqlConnection *m_sparqlConnection;
    QSparqlResult *m_result;

    QString m_directory;
    bool m_loaded;
    int m_tracksCount;
signals:
    void directoryChanged();
    void loadedChanged();
};

class DirectoryTracksProxyModel : public FilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int tracksCount READ tracksCount NOTIFY tracksCountChanged)
public:
    void componentComplete();

    int tracksCount() const;
    Q_INVOKABLE QVariantList getTracks() const;
    Q_INVOKABLE QVariantList getSelectedTracks() const;
    Q_INVOKABLE void selectAll();
signals:
    void tracksCountChanged();
};

}

#endif // UNPLAYER_DIRECTORYTRACKSMODEL_H