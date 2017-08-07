


# 1) Driver for xl-320 on arduino

## 2.1) About

## 2.2) Install

## 2.3) Use



# 2) Command machine to control servos through serial


## 2.1) About

## 2.2) Install

## 2.3) Use


### Command structure

- **Command to get values** : XX+CMD?
- **Command to set values** : XX+CMD=ARGS

Available CMD values and its ARGS are here in the table

| Command           | Description   | Write Arguments  |
| ----------------- |:-------------:|:-------------:|
| XBAUD             |               |                   |
| PING              |               |                   |
| SEL               |               |                   |
| MODEL             |               |                   |
| VERSION           |               |                   |
| ID                |               |                   |
| BAUD              |               |                   |
| RDT               |               |                   |
| ANG_LIM_MIN       |               |                   |
| ANG_LIM_MAX       |               |                   |
| CTRL_MODE         |               |                   |
| TEMP_LIM          |               |                   |
| VOLT_LIMIT_D      |               |                   |
| VOLT_LIMIT_U      |               |                   |
| MAX_TORQUE        |               |                   |
| RET_LEVEL         |               |                   |
| ALARM             |               |                   |
| TORQUE_ENABLE     |               |                   |
| LED               |               |                   |
| DGAIN             |               |                   |
| IGAIN             |               |                   |
| PGAIN             |               |                   |
| GPOS              |               |                   |
| GSPEED            |               |                   |
| GTORQUE           |               |                   |
| PPOS              |               |  Read Only        |
| PSPEED            |               |  Read Only        |
| PLOAD             |               |  Read Only        |
| PVOLT             |               |  Read Only        |
| PTEMP             |               |  Read Only        |
| REG_INSTRU        |               |  Read Only        |
| MOVING            |               |  Read Only        |
| HARDWARE_ERROR    |               |                   |
| PUNCH             |               |                   |

