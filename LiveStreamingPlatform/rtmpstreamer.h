#ifndef RTMPSTREAMER_H
#define RTMPSTREAMER_H

#include <QObject>

class RTMPStreamer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning WRITE setIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QString outUrl READ outUrl WRITE setOutUrl NOTIFY outUrlChanged)

public:
    explicit RTMPStreamer(QObject *parent = nullptr);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    bool isRunning() const;
    QString outUrl() const;

public slots:
    void setIsRunning(bool isRunning);
    void setOutUrl(QString outUrl);

signals:
    void isRunningChanged(bool isRunning);
    void outUrlChanged(QString outUrl);

private:
    bool m_isRunning;
    QString m_outUrl;
};

#endif // RTMPSTREAMER_H
