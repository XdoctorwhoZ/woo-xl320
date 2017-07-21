#ifndef XL320CONTROLLER_HPP
#define XL320CONTROLLER_HPP
#pragma once

// Arduino
#include <Stream.h>
#include <Arduino.h>
#include <HardwareSerial.h>

// Woo
#include "XL320Packet.hpp"

// ---
namespace xl320 {

//! Controller for xl-320 servo chain
//!
class Controller
{

public:

    // === DEFINITIONS ===

    //! To easily change serial class without template
    typedef HardwareSerial XlSerial;

    //! Maximal number of servo xl-320 selectable at once
    static constexpr int MaxServoSelectable = 8;

    //! Size of the Rx buffer, to extends serial buffer
    static constexpr int RxBufferSize = 128;

    //! Flag to tell that the buffer is empty
    static constexpr int RxBufferEmpty = -42;
    
    //! Data about control table
    //!
    static const ControlReg ControlTable[CiTotal];

private:

    // === MEMBERS ===

    //! Serial controller to comminucate with xl-320
    XlSerial* mXlSerial;

    //! Current baudrate of mXlSerial
    BaudRate mXlSerialBaudRate;



    //! Number of selected servo (size of useful id in mSelectedServoIds)
    byte mNumberOfSelectedServo;

    //! Ids of the selected servo
    byte mSelectedServoIds[MaxServoSelectable];



    //! Current read pointer on the rx buffer
    //!
    byte* mRxPtr;

    //! Number of bytes in the RxBuffer
    //!
    short mRxBytes;

    //! Buffer for incoming packets
    //!
    byte mRxBuffer[RxBufferSize];



    //! Number of Hardware error, read from status packets
    //!
    byte mHwError;

    //! Store the last error
    //!
    ErrorData mLastError;

public:
    
    // === Initializers ===

    //! Constructor
    //!
    Controller();

    //! Destructor
    //!
    virtual ~Controller();

    // === xl serial config ===

    //! Initialize the controller with the cmd stream and the xl serial
    //!
    void setup(XlSerial& xlSerial);

    //! Get the xl serial baudrate 
    //!
    BaudRate getXlBaudRate() const;

    //! Get the xl serial baudrate as string 
    //!
    String getXlBaudRateString() const;

    //! Set a new baudrate for the xl serial
    //!
    void setXlBaudRate(BaudRate br);

    // === Select Servos ===

    //! Return the number of servo selected for actions
    //!
    byte getNumberOfSelectedServo() const { return mNumberOfSelectedServo; }

    //! Return the list of selected servo ids in a string form
    //!
    String getSelectedServoString() const;

    //! Select servo ids
    //!
    void selectServo(const byte* ids, byte number = 1);

    // === Hardware error ===

    byte getHwError() const { return mHwError; }
    const ErrorData& getLastError() const { return mLastError; }
    static String ErrorToString(const ErrorData& err);

    // === Packet helpers ===

    //! Read data from serial during usTimeout micro-secondes
    //!
    int receiveData(unsigned long usTimeout);

    //! Get the next packet in the RxBuffer
    //! Return false if no packet found, else true
    //!
    int getNextPacket(Packet& pack);


    //! Read a packet from xl serial, store data in buffer and return number of byte read
    //! msize is the max size of the buffer
    //!
    int readNextPacket(byte* buffer, int msize); // Deprecated

    //! Drop all Rx Packets
    //!
    void dropRxPackets(); // Deprecated

    //! Read incoming packets and fill values table.
    //! values size must be equal to mNumberOfSelectedServo
    //!
    int readValuesFromRxPackets(int* values);

    //! To broadcast a ping packet
    //!
    void sendPingPacket();

    //! To send packet to read 1 reg of 1 servo
    //!
    void sendReadPacket(byte id, ControlIndex ci) const;

    //! To send packet to reg 1 reg of n servo
    //!
    void sendSyncReadPacket(ControlIndex ci) const;

    //! To send packet to write 1 reg of 1 servo
    //!
    void sendWritePacket(byte id, ControlIndex ci, int value) const;

    //! To send packet to write 1 reg of n servo
    //!
    void sendSyncWritePacket(ControlIndex ci, const int* value, int num) const;

    // === Ping ===

    //! Ping the chain and return the number of servo found
    //! Pass a byte table to get Ids of each servo
    //!
    int ping(int* ids = 0);

    // === ModelNumber ===
    int getNumber(int* values) const;

    // === Version ===
    int getVersion(int* values) const;

    // === Id ===
    void setId(byte id); // Only once at time

    // === Baud ===
    int getBaud(BaudRate* brs) const;
    void setBaud(BaudRate br) const; // Only once at time

    // === ReturnDelayTime ===
    int getReturnDelayTime(int* values) const;
    void setReturnDelayTime(const int* values, int number) const;

    // === CwAngleLimit ===
    int getCwAngleLimit(int* values) const;
    void setCwAngleLimit(const int* values, int number) const;

    // === CcwAngleLimit ===
    int getCcwAngleLimit(int* values) const;
    void setCcwAngleLimit(const int* values, int number) const;

    // === ControlMode ===
    int getControlMode(int* values) const;
    void setControlMode(const int* values, int number) const;

    // === LimitTemperature ===
    int getLimitTemperature(int* values) const;
    void setLimitTemperature(const int* values, int number) const;

    // === DownLimitVoltage ===
    int getDownLimitVoltage(int* values) const;
    void setDownLimitVoltage(const int* values, int number) const;

    // === UpLimitVoltage ===
    int getUpLimitVoltage(int* values) const;
    void setUpLimitVoltage(const int* values, int number) const;

    // === MaxTorque ===
    int getMaxTorque(int* values) const;
    void setMaxTorque(const int* values, int number) const;

    // === ReturnLevel ===
    int getReturnLevel(int* values) const;
    void setReturnLevel(const int* values, int number) const;

    // === AlarmShutdown ===
    int getAlarmShutdown(int* values) const;
    void setAlarmShutdown(const int* values, int number) const;

    // === TorqueEnable ===
    int getTorqueEnable(int* values) const;
    void setTorqueEnable(const int* values, int number) const;

    // === Led ===
    int getLed(int* values) const;
    void setLed(const int* values, int number) const;

    // === Dgain ===
    int getDgain(int* values) const;
    void setDgain(const int* values, int number) const;

    // === Igain ===
    int getIgain(int* values) const;
    void setIgain(const int* values, int number) const;

    // === Pgain ===
    int getPgain(int* values) const;
    void setPgain(const int* values, int number) const;

    // === GoalPosition ===
    int getGoalPosition(int* positions) const;
    void setGoalPosition(const int* positions, int number) const;

    // === GoalSpeed ===
    int getGoalSpeed(int* speeds) const;
    void setGoalSpeed(const int* speeds, int number) const;

    // === GoalTorque ===
    int getGoalTorque(int* values) const;
    void setGoalTorque(const int* values, int number) const;

    // === PresentPosition ===
    int getPresentPosition(int* values) const;
    void setPresentPosition(const int* values, int number) const;

    // === PresentSpeed ===
    int getPresentSpeed(int* values) const;
    void setPresentSpeed(const int* values, int number) const;

    // === PresentLoad ===
    int getPresentLoad(int* values) const;
    void setPresentLoad(const int* values, int number) const;

    // === PresentVoltage ===
    int getPresentVoltage(int* values) const;
    void setPresentVoltage(const int* values, int number) const;

    // === PresentTemperature ===
    int getPresentTemperature(int* values) const;
    void setPresentTemperature(const int* values, int number) const;

    // === RegisteredInstruction ===
    int getRegisteredInstruction(int* values) const;
    void setRegisteredInstruction(const int* values, int number) const;

    // === Moving ===
    int getMoving(int* values) const;
    void setMoving(const int* values, int number) const;

    // === HardwareError ===
    int getHardwareError(int* values) const;
    void setHardwareError(const int* values, int number) const;

    // === Punch ===
    int getPunch(int* values) const;
    void setPunch(const int* values, int number) const;
};

} // xl320

#endif // XL320CONTROLLER_HPP
