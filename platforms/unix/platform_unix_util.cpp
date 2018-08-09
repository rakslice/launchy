#include "platform_unix_util.h"


#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QPainter>
#include <QProcess>

QString getEnvVar(const QString name, const QString defaultVal) {

	if (name.contains('=')) return defaultVal;

	QString prefix = name + "=";
    foreach(QString line, QProcess::systemEnvironment()) {
        if (!line.startsWith(prefix))
            continue;
        return line.section('=', 1);
    }

    return defaultVal;
}

UnixIconProvider::UnixIconProvider() {
    // default per XDG Base Directory Specification 0.7
    // https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html#variables
    QString xdgDataDirsStr = getEnvVar("XDG_DATA_DIRS", "/usr/local/share/:/usr/share/");
    xdgDataDirs = xdgDataDirsStr.split(":");
}


QIcon UnixIconProvider::icon(const QFileInfo& info) const
{
    QString name = info.fileName();

    if (name.endsWith(".png", Qt::CaseInsensitive))
	return QIcon(info.absoluteFilePath());    
    if (name.endsWith(".ico", Qt::CaseInsensitive))
	return QIcon(info.absoluteFilePath());
    if (!name.contains(".")) {
        QIcon themed = QIcon::fromTheme(name);
        if (!themed.isNull())
            return themed;
        return QFileIconProvider::icon(QFileIconProvider::File);
    }

    QString end = name.mid(name.lastIndexOf(".")+1);
    if (!file2mime.contains(end.toLower())) {
	
	QProcess proc;
	
	QStringList args;
	args += "query";
	args += "filetype";
	args += info.absoluteFilePath();
	proc.setReadChannel(QProcess::StandardOutput);
	proc.start(QString("xdg-mime"), args);
	proc.waitForFinished(10000);
	QString mimeType = proc.readAll().trimmed();
	proc.close();
	file2mime.insert(end.toLower(), mimeType);
    }

    QString mimeType = file2mime[end.toLower()];

    if (!mime2desktop.contains(mimeType)) {
	QProcess proc2;
	QStringList args;
	args += "query";
	args += "default";
	args += mimeType;
	proc2.start(QString("xdg-mime"),args);
	proc2.waitForFinished(10000);
	QString desk = proc2.readAll().trimmed();
	proc2.close();
	
	mime2desktop[mimeType] = desk;
    }

    
    QString desktop = mime2desktop[mimeType];

    if (desktop == "")
        return QFileIconProvider::icon(QFileIconProvider::File);

    qDebug() << "desktop icon end case for" << desktop;
    return QIcon::fromTheme(desktop);
}

QString trailingSlash(QString s) {
	int len = s.length();
	if (len > 0 && s[len - 1] == '/') {
		return s;
	} else {
		return s + '/';
	}
}
