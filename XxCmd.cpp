#include "XxCmd.hpp"

/* ============================================================================
 *
 * */
const char* XxCmd::Names[Value::Total] = {
    "XBAUD"       ,
    "PING"        ,
    "SELECT"      ,
    "BAUD"        ,
    "GPOS"        ,
    "GSPEED"      ,
    "LED"         ,
    "TORQUE"      ,
};

/* ============================================================================
 *
 * */
XxCmd::Value XxCmd::GetValueFromName(const char* beg, const char* end)
{
    for(int i=0 ; i<Value::Total ; i++)
    {
        if (IsTheSameString(beg, end, Names[i]))
        {
            return (XxCmd::Value) i;
        }
    }
    return Value::Total;
}

/* ============================================================================
 *
 * */
bool XxCmd::IsTheSameString(const char* beg, const char* end, const char* word)
{
    int size0 = end - beg;
    int size1 = 0;
    const char* tmp = word;
    while(*tmp != '\0') { size1++; tmp++; }

    if( size0 != size1 )
    {
        return false;
    }

    while (beg != end)
    {
        if (*word == '\0') return false;
        if (*beg != *word) return false;

        word++;
        beg++;
    }
    return true;
}

