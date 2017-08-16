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
    static constexpr unsigned long RxBaseTimeout = 100000;

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

    // === Std ===

    //! Standart getter
    //!
    int getStdVals(int* values, ControlIndex ci);

    //! Standart setter
    //!
    void setStdVals(const int* values, int number, ControlIndex ci);

    // === Ping ===

    //! Ping the chain and return the number of servo found
    //! Pass a byte table to get Ids of each servo
    //!
    int ping(int* ids = 0);

    // === ModelNumber ===
    int getNumber(int* values);

    // === Version ===
    int getVersion(int* values);

    // === Id ===
    void setId(byte id); // Only once at time

    // === Baud ===
    int getBaud(BaudRate* brs) const;
    void setBaud(BaudRate br) const; // Only once at time

    // === ReturnDelayTime ===
    int getReturnDelayTime(int* values) const;
    void setReturnDelayTime(const int* values, int number) const;

    // === CwAngleLimit ===
    int getCwAngleLimit(int* values) { return getStdVals(values, CiCwAngleLimit); }
    void setCwAngleLimit(const int* values, int number) { setStdVals(values, number, CiCwAngleLimit); }

    // === CcwAngleLimit ===
    int getCcwAngleLimit(int* values) { return getStdVals(values, CiCcwAngleLimit); }
    void setCcwAngleLimit(const int* values, int number) { setStdVals(values, number, CiCcwAngleLimit); }

    // === ControlMode ===
    int getControlMode(int* values) { return getStdVals(values, CiControlMode); }
    void setControlMode(const int* values, int number) { setStdVals(values, number, CiControlMode); }

    // === LimitTemperature ===
    int getLimitTemperature(int* values) { return getStdVals(values, CiLimitTemperature); }
    void setLimitTemperature(const int* values, int number) { setStdVals(values, number, CiLimitTemperature); }

    // === DownLimitVoltage ===
    int getDownLimitVoltage(int* values) { return getStdVals(values, CiDownLimitVoltage); }
    void setDownLimitVoltage(const int* values, int number) { setStdVals(values, number, CiDownLimitVoltage); }

    // === UpLimitVoltage ===
    int getUpLimitVoltage(int* values) { return getStdVals(values, CiUpLimitVoltage); }
    void setUpLimitVoltage(const int* values, int number) { setStdVals(values, number, CiUpLimitVoltage); }

    // === MaxTorque ===
    int getMaxTorque(int* values) { return getStdVals(values, CiMaxTorque); }
    void setMaxTorque(const int* values, int number) { setStdVals(values, number, CiMaxTorque); }

    // === ReturnLevel ===
    int getReturnLevel(int* values) { return getStdVals(values, CiReturnLevel); }
    void setReturnLevel(const int* values, int number) { setStdVals(values, number, CiReturnLevel); }

    // === AlarmShutdown ===
    int getAlarmShutdown(int* values) { return getStdVals(values, CiAlarmShutdown); }
    void setAlarmShutdown(const int* values, int number) { setStdVals(values, number, CiAlarmShutdown); }

    // === TorqueEnable ===
    int getTorqueEnable(int* values) { return getStdVals(values, CiTorqueEnable); }
    void setTorqueEnable(const int* values, int number) { setStdVals(values, number, CiTorqueEnable); }

    // === Led ===
    int getLed(int* values) { return getStdVals(values, CiLed); }
    void setLed(const int* values, int number) { setStdVals(values, number, CiLed); }

    // === Dgain ===
    int getDgain(int* values) { return getStdVals(values, CiDgain); }
    void setDgain(const int* values, int number) { setStdVals(values, number, CiDgain); }

    // === Igain ===
    int getIgain(int* values) { return getStdVals(values, CiIgain); }
    void setIgain(const int* values, int number) { setStdVals(values, number, CiIgain); }

    // === Pgain ===
    int getPgain(int* values) { return getStdVals(values, CiPgain); }
    void setPgain(const int* values, int number) { setStdVals(values, number, CiPgain); }

    // === GoalPosition ===
    int getGoalPosition(int* values) { return getStdVals(values, CiGoalPosition); }
    void setGoalPosition(const int* values, int number) { setStdVals(values, number, CiGoalPosition); }

    // === GoalSpeed ===
    int getGoalSpeed(int* values) { return getStdVals(values, CiGoalSpeed); }
    void setGoalSpeed(const int* values, int number) { setStdVals(values, number, CiGoalSpeed); }

    // === GoalTorque ===
    int getGoalTorque(int* values) { return getStdVals(values, CiGoalTorque); }
    void setGoalTorque(const int* values, int number) { setStdVals(values, number, CiGoalTorque); }

    // === PresentPosition ===
    int getPresentPosition(int* values) { return getStdVals(values, CiPresentPosition); }

    // === PresentSpeed ===
    int getPresentSpeed(int* values) { return getStdVals(values, CiPresentSpeed); }

    // === PresentLoad ===
    int getPresentLoad(int* values) { return getStdVals(values, CiPresentLoad); }

    // === PresentVoltage ===
    int getPresentVoltage(int* values) { return getStdVals(values, CiPresentVoltage); }

    // === PresentTemperature ===
    int getPresentTemperature(int* values) { return getStdVals(values, CiPresentTemperature); }

    // === RegisteredInstruction ===
    int getRegisteredInstruction(int* values) const;

    // === Moving ===
    int getMoving(int* values) const;

    // === HardwareError ===
    int getHardwareError(int* values) const;

    // === Punch ===
    int getPunch(int* values) const;
    void setPunch(const int* values, int number) const;
};

} // xl320

#endif // XL320CONTROLLER_HPP
