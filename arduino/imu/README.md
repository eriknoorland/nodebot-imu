# Xiao IMU
Dedicated firmware to manage the BNO055 IMU.

## Request Packet Format

| Start Flag | Command | Payload Size |
|------------|---------|--------------|
| 1 byte     | 1 byte  | x bytes      |

## Response Packet Format

| Start Flag 1 | Start Flag 2 | Command | Response Data Length | Response |
|--------------|--------------|---------|----------------------|----------|
| `0xA6`       | `0x6A`       | 1 byte  | 1 byte               | x bytes  |

## Requests Overview

| Request   | Value  | Payload |
|-----------|--------|---------|
| IS_READY  | `0x10` | N/A     |
| RESET_IMU | `0x20` | N/A     |

## Is Ready Request
Request: `0xA6 0x10`

Request the Seeeduino IMU to emit the ready response.

## Reset IMU Request
Request: `0xA6 0x20`

Resets the IMU.

### Ready Response
**Response:** `0xA6` `0x6A` `0xFF` `0x01` `0x[imu detection flag]`

This response will be sent when the main controller is ready to be controlled. The imu detection flag describes whether the IMU was found.

### Data Response
**Response:** `0xA6` `0x6A` `0x30` `0x04` `0x[heading 15:8]` `0x[heading 7:0]`

This response will be sent every 20 miliseconds. The heading will need to be divided by 100 to get the correct angle up to two decimals.