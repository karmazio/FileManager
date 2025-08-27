#include "file_item.h"


FileItem::FileItem(const QString &name,
                   const QString &path,
                   qint64 size,
                   const QDateTime &modified,
                   const QString &type)
    : m_name(name), m_path(path), m_size(size), m_modified(modified), m_type(type) {}


QString FileItem::extension() const {
    int dot = m_name.lastIndexOf('.');
    if (dot > 0 && dot < m_name.size() - 1)
        return m_name.mid(dot + 1).toLower();
    return {};
}


QString FileItem::sizeString() const {
    // Use QLocale for nice formatting;
    static const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    double s = static_cast<double>(m_size);
    int idx = 0;
    while (s >= 1024.0 && idx < 4) { s /= 1024.0; ++idx; }
    return QString("%1 %2").arg(QLocale().toString(s, 'f', (idx==0?0:2))).arg(units[idx]);
}


QString FileItem::modifiedString() const {
    return m_modified.toString("yyyy-MM-dd hh:mm");
}
