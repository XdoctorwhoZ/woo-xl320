

#include "dxl_pro.h"

// Woo
#include "XL320Packet.hpp"

XL320Packet::XL320Packet(
    unsigned char *data,
    int data_size,
    unsigned char id,
    unsigned char instruction,
    int parameter_data_size,
    ...) {


    // [ff][ff][fd][00][id][len1][len2] { [instr][params(parameter_data_size)][crc1][crc2] }
    unsigned int length=3+parameter_data_size;
    if(!data) {
    // [ff][ff][fd][00][id][len1][len2] { [data(length)] }
    this->data_size = 7+length;   
    this->data = (unsigned char*)malloc(data_size);
    this->freeData = true;
    } else {
    this->data = data;
    this->data_size = data_size;
    this->freeData = false;
    }
    this->data[0]=0xFF;
    this->data[1]=0xFF;
    this->data[2]=0xFD;
    this->data[3]=0x00;
    this->data[4]=id;
    this->data[5]=length&0xff;
    this->data[6]=(length>>8)&0xff;
    this->data[7]=instruction;
    va_list args;
    va_start(args, parameter_data_size); 
    for(int i=0;i<parameter_data_size;i++) {
    unsigned char arg = va_arg(args, int);
    this->data[8+i]=arg;
    }
    unsigned short crc = update_crc(0,this->data,this->getSize()-2);
    this->data[8+parameter_data_size]=crc&0xff;
    this->data[9+parameter_data_size]=(crc>>8)&0xff;
    va_end(args);
}

XL320Packet::XL320Packet(unsigned char *data, int size) {
    this->data = data;
    this->data_size = size;
    this->freeData = false;
}


XL320Packet::~XL320Packet() {
    if(this->freeData==true) {
    free(this->data);
    }
}

void XL320Packet::toStream(Stream &stream) {
    stream.print("id: ");
    stream.println(this->getId(),DEC);
    stream.print("length: ");
    stream.println(this->getLength(),DEC);
    stream.print("instruction: ");
    stream.println(this->getInstruction(),HEX);
    stream.print("parameter count: ");
    stream.println(this->getParameterCount(), DEC);
    for(int i=0;i<this->getParameterCount(); i++) {
    stream.print(this->getParameter(i),HEX);
    if(i<this->getParameterCount()-1) {
        stream.print(",");
    }
    }
    stream.println();
    stream.print("valid: ");
    stream.println(this->isValid()?"yes":"no");
}

unsigned char XL320Packet::getId() {
    return data[4];
}

int XL320Packet::getLength() {
    return data[5]+((data[6]&0xff)<<8);
}

int XL320Packet::getSize() {
    return getLength()+7;
}

int XL320Packet::getParameterCount() {
    return getLength()-3;
}

unsigned char XL320Packet::getInstruction() {
    return data[7];
}

unsigned char XL320Packet::getParameter(int n) {
    return data[8+n];
}

bool XL320Packet::isValid() {
    int length = getLength();
    unsigned short storedChecksum = data[length+5]+(data[length+6]<<8);
    return storedChecksum == update_crc(0,data,length+5);
}

