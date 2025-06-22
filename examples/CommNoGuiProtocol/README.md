# Common Communication Protocol

## frame format

| offset | field       | length | comment                |
| ------ | ----------- | ------ | ---------------------- |
| 0      | header      | 2      | 0xEB90                 |
| 2      | version     | 1      | 0x00                   |
| 3      | data length | 2      | N                      |
| 5      | data        | N      |                        |
| 5 + N  | check code  | 2      | CRC-16/MODBUS(0x18005) |

By default, data longer than one byte use big endian.

```
 0      1      2      3      4      5      6...    N-2    N-1
+------+------+------+------+------+------+-----+------+------+
| 0xEB | 0x90 | Ver  | LenH | LenL | Data | ... | CRCH | CRCL |
+------+------+------+------+------+------+-----+------+------+
|<---------- fixed header -------->|<-- data -->|<--- CRC --->|
```

## example

- empty frame

```
EB90 00 0000 172D

header：0xEB90
version： 0x00
data length：0x0000
data：-
check code：0x172D
```
- standard frame

```
EB90 00 0007 20211025000000 7F52

header：0xEB90
version： 0x00
data length：0x0007
data：0x20211025000000
check code：0x7F52
```

## FSM

```mermaid
stateDiagram-v2
    [*] --> STATE_IDLE

    STATE_IDLE --> STATE_FIND_HEADER1: remain size>0

    STATE_FIND_HEADER1 --> STATE_FIND_HEADER2: found 0xEB [start=offset]
    STATE_FIND_HEADER1 --> STATE_FIND_HEADER1: not found 0xEB [offset++]
    STATE_FIND_HEADER1 --> [*]: data not enough [skip=size]

    STATE_FIND_HEADER2 --> STATE_PARSE_LENGTH: found 0x90 [offset++]
    STATE_FIND_HEADER2 --> STATE_IDLE: not found 0x90 [offset=start+1]
    STATE_FIND_HEADER2 --> [*]: data not enough [skip=offset-1]

    STATE_PARSE_LENGTH --> STATE_DATA: len ok [offset+=3]
    STATE_PARSE_LENGTH --> STATE_IDLE: len>MAX_SIZE [offset=start+1]
    STATE_PARSE_LENGTH --> [*]: data not enough [skip=start]

    STATE_DATA --> STATE_CHECK_CODE: parse data [offset+=dataLen]
    STATE_DATA --> [*]: data not enough [skip=start]

    STATE_CHECK_CODE --> STATE_IDLE: check passed, parse next [offset=start+totalLen]
    STATE_CHECK_CODE --> STATE_IDLE: check failed [offset=start+1]
    STATE_CHECK_CODE --> [*]: data not enough [skip=start]
```