#ifndef XL320PACKET_HPP
#define XL320PACKET_HPP
#pragma once


#include <Stream.h>

class XL320Packet
{
    bool freeData;
    public:
    unsigned char *data;
    int data_size;

    // wrap a received data stream in an Packet object for analysis
    XL320Packet(unsigned char *data, int size);
    // build a packet into the pre-allocated data array
    // if data is null it will be malloc'ed and free'd on destruction.

    XL320Packet(
      unsigned char *data, 
      int        size,
      unsigned char id,
      unsigned char instruction,
      int           parameter_data_size,
      ...);
    ~XL320Packet();
    unsigned char getId();
    int getLength();
    int getSize();
    int getParameterCount();
    unsigned char getInstruction();
        unsigned char getParameter(int n);
    bool isValid();

    void toStream(Stream &stream);
    
};

#endif // XL320PACKET_HPP

