#ifndef WOO_ARDUINO_XL320_DLLSPEC_H
#define WOO_ARDUINO_XL320_DLLSPEC_H
#pragma once

// EXPORT / IMPORT
#if defined WOO_ARDUINO_XL320_DECL_EXPORT
    #define WOO_ARDUINO_XL320_DLLSPEC Q_DECL_EXPORT
#else
    #define WOO_ARDUINO_XL320_DLLSPEC Q_DECL_IMPORT
#endif

#endif // WOO_ARDUINO_XL320_DLLSPEC_H
