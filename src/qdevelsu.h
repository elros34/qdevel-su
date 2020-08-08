#ifndef QDEVELSU_H
#define QDEVELSU_H

#include <QObject>

class QSocketNotifier;
class QDevelSu : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
public:
    explicit QDevelSu(const QStringList &positionalArgs,
                      const QString &title,
                      const QString &description,
                      QObject *parent = nullptr);

    Q_INVOKABLE bool writePasswd(const QString &passwd);
    Q_INVOKABLE void exit(int status);

    QString title() const { return m_title; }
    QString description() const { return m_description; }

private:
    QSocketNotifier *m_ptyNotifier;
    int m_ptyFd;
    bool m_showGUI;
    QStringList m_positionalArguments;
    QString m_execCmd;
    QString m_title;
    QString m_description;

private:
    void writeToPty(QString cmd);
    int readFromPty(QString &data);
    void notifyWrongPassword();

signals:
    void execCmdChanged();
    void showWindow();
};

#endif // QDEVELSU_H
