#ifndef XXCMD_HPP
#define XXCMD_HPP
#pragma once


struct XxCmd
{

    enum Value
    {
        XBaud   = 0 ,
        Ping    = 1 ,
        Select  = 2 ,
        Baud    = 3 ,
        Gpos    = 4 ,
        Gspeed  = 5 ,
        Led     = 6 ,
        Gtorque = 7 ,
        Total   = 8 ,
    };


    static const char* Names[Value::Total];



    static Value GetValueFromName(const char* beg, const char* end);


private:

    static bool IsTheSameString(const char* beg, const char* end, const char* word);

};





#endif // XXCMD_HPP

