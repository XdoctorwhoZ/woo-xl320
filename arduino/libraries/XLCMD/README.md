XLCMD
=====

Example
-------

```bash
# First ping to know how many servos are connected and responding
> XL+PING?
ping={1, 2, 3, 4} # here we have 4 servos

# Just select the 1 first to control
> XL+SEL=1

# Move the selected servo to 512
> XL+GPOS=512

# Get goal position of the selected servo
> XL+GPOS?

# Get goal position of the selected servo
> XL+GPOS?
```

Commands structure
------------------

- **Command to get values** : XL+CMD?
- **Command to set values** : XL+CMD=ARG0,ARG1, ..., ARGN

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

