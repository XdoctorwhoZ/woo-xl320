#ifndef WOO_XL320_COMDEVICE_H
#define WOO_XL320_COMDEVICE_H
#pragma once

// Qt
#include <QObject>
#include <QByteArray>

// ---
namespace woo { namespace xl320 {

//! Interface for a communication device
class ComDevice : public QObject
{
    Q_OBJECT

public slots:

    //! Send data through com device
    virtual void sendData(const QByteArray& data) = 0;

signals:

    //! Emitted when data is received in com device
    void dataReceived(QByteArray data);

};

#endif // WOO_XL320_COMDEVICE_H
