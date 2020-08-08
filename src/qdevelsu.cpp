#include "qdevelsu.h"
#include <QDebug>
#include <QGuiApplication>
#include <QSocketNotifier>
#include <pty.h>
#include <utmp.h>
#include <unistd.h>
#include <sys/wait.h>
#include <nemonotifications-qt5/notification.h>

QDevelSu::QDevelSu(const QStringList &positionalArgs, const QString &title,
                   const QString &description, QObject *parent) :
    QObject(parent),
    m_positionalArguments(positionalArgs)
{
    m_execCmd = m_positionalArguments.join(" ");
    m_showGUI = !isatty(fileno(stdin));

    char develSuBinary[] = "/usr/bin/devel-su";
    int argsLength = positionalArgs.length() + 2;
    char *args[argsLength];

    args[0] = develSuBinary;
    args[argsLength - 1] = nullptr;
    for (int i = 0; i < positionalArgs.length(); ++i) {
        QByteArray ba = positionalArgs.at(i).toLocal8Bit();
        char *c = new char[ba.size() + 1];
        strcpy(c, ba.data());
        args[i+1] = c;
    }

#ifndef QT_DEBUG
    if (!m_showGUI) {
        execv(args[0], args);
        qDebug() << "exec error: " << strerror(errno);
        qApp->exit(1);
        return;
    }
#endif

    char name[255];
    int childPid = forkpty(&m_ptyFd, name, nullptr, nullptr);
    if (childPid < 0) {
        qDebug() << "Can not open pseudoterminal";
        qApp->quit();
        return;
    }

    // child
    if (childPid == 0) {
        execv(args[0], args);
        qDebug() << "exec error: " << strerror(errno);
        qApp->exit(1);
        return;
    }

    m_ptyNotifier = new QSocketNotifier(m_ptyFd, QSocketNotifier::Read, this);
    connect(m_ptyNotifier, &QSocketNotifier::activated, m_ptyNotifier, [childPid, this](){
        m_ptyNotifier->setEnabled(false);
        QString data;
        int size = readFromPty(data);

        if (size <= 0) {
            int status;
            waitpid(childPid, &status, 0);
            int exitStatus = WEXITSTATUS(status);
            qApp->exit(exitStatus);
            return;
        }

        static bool passwordAsked = false;
        if (!passwordAsked && data.startsWith("Password:")) {
            passwordAsked = true;
            emit this->showWindow();
        } else {
            QTextStream ts(stdout);
            ts << data;
        }

        if (data.startsWith("Auth failed"))
            this->notifyWrongPassword();

        m_ptyNotifier->setEnabled(true);
    });

    m_title = title.isEmpty() ? "Run as root" : title;
    m_description = description.isEmpty() ? m_execCmd : description;
}

bool QDevelSu::writePasswd(const QString &passwd)
{
    if (passwd.isEmpty())
        writeToPty("phq");
    else
        writeToPty(passwd);

    return true;
}

void QDevelSu::writeToPty(QString cmd)
{
    if (!cmd.endsWith("\r\n"))
        cmd.append("\r\n");
    int cmdLength = cmd.length();
    int written = write(m_ptyFd, qPrintable(cmd), (size_t)cmdLength);
    if (written < 0 || written != cmdLength)
        qDebug() << "Can not write: " << cmd;
}

int QDevelSu::readFromPty(QString &data)
{
    char buf[1024] = {0};
    int size = read(this->m_ptyFd, buf, sizeof(buf));
    data = buf;
    return size;
}

void QDevelSu::notifyWrongPassword()
{
    Notification notify;
    notify.setPreviewSummary("Wrong password");
    notify.setPreviewBody("Failed to execute: \"" + m_execCmd + "\"");
    notify.publish();
}

void QDevelSu::exit(int status)
{
    qApp->exit(status);
}

