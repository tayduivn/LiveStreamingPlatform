#include <iostream>
#include "rtmpstreamer.h"
#include "controller/controller.h"

RTMPStreamer::RTMPStreamer(QObject *parent) : QObject(parent)
{

}

void RTMPStreamer::start()
{
    bool ret = Controller::Get()->Start();
    ret ? setIsRunning(true) : setIsRunning(false);
}

void RTMPStreamer::stop()
{
    Controller::Get()->Stop();
    setIsRunning(false);
}

bool RTMPStreamer::isRunning() const
{
    return m_isRunning;
}

QString RTMPStreamer::outUrl() const
{
    return m_outUrl;
}

void RTMPStreamer::setIsRunning(bool isRunning)
{
    if (m_isRunning == isRunning)
        return;

    m_isRunning = isRunning;
    emit isRunningChanged(m_isRunning);
}

void RTMPStreamer::setOutUrl(QString outUrl)
{
    if (m_outUrl == outUrl)
        return;

    m_outUrl = outUrl;
    Controller::Get()->outUrl = m_outUrl.toStdString();
    emit outUrlChanged(m_outUrl);
}
