#ifndef WOO_ARDUINO_XL320_SERVOIDS_H
#define WOO_ARDUINO_XL320_SERVOIDS_H
#pragma once

// Qt
#include <QList>

// ---
namespace woo { namespace arduino_xl320 {

//! Class to store servos ids in 2 formats list and string
//!
class ServoIds
{
    //! Servo id type
    typedef int sid_t;

    //! Ids in list format
    QList<sid_t>    mLisId;

    //! Ids in string format id0,id1,id2
    QString         mStrId;

public:

    ServoIds(const QString& str = QString()): mStrId(str) { str2lis(mStrId, mLisId); }
    ServoIds(const QList<int>& lis)         : mLisId(lis) { lis2str(mLisId, mStrId); }

    void set(const QString& str)    { mStrId = str; str2lis(mStrId, mLisId); }
    void set(const QList<int>& lis) { mLisId = lis; lis2str(mLisId, mStrId); }

    const QString& getStr() const    { return mStrId; }
    const QList<int>& getLis() const { return mLisId; }

    static void str2lis(const QString& str, QList<sid_t>& lis);
    static void lis2str(const QList<sid_t>& lis, QString& str);

};

} // arduino_xl320
} // woo

#endif // WOO_ARDUINO_XL320_SERVOIDS_H
