#ifndef XXCMD_HPP
#define XXCMD_HPP
#pragma once


struct XxCmd
{

    enum Value
    {
          XBaud   = 0
        , Select  = 1
        , Baud    = 2
        , Gpos    = 3
        , Gspeed  = 4
        , Led     = 5
        , Gtorque  = 6
        , Total   = 7
    };


    static const char* Names[Value::Total];



    static Value GetValueFromName(const char* beg, const char* end);


private:

    static bool IsTheSameString(const char* beg, const char* end, const char* word);

};





#endif // XXCMD_HPP
