#pragma once
#include <QtGui>


#include <QFileIconProvider>

#include <QWidget>


class UnixIconProvider : public QFileIconProvider
{
 private:
    mutable QHash<QString, QString> file2mime;
    mutable QHash<QString, QString> mime2desktop;
    mutable QHash<QString, QString> desktop2icon;
    mutable QHash<QString, QString> icon2path;
    QStringList xdgDataDirs;
 public:
    UnixIconProvider();
    ~UnixIconProvider() {}
    virtual QIcon icon(const QFileInfo& info) const;
    QString getDesktopIcon(QString desktopFile, QString IconName = "") const;
};

