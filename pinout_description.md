![Microchip Logo](assets/logo.png "Microchip Technology")

# Pinout Description — dsPIC33AK512MC510

**Pin and peripheral assignment reference for the MCLV-48V-300W + T1S FOC project**

![Device](https://img.shields.io/badge/device-dsPIC33AK512MC510-blue)
![Board](https://img.shields.io/badge/board-MCLV--48V--300W-blue)
![Updated](https://img.shields.io/badge/updated-2026--07--31-informational)

---

## Table of Contents

- [Named GPIO Pins](#named-gpio-pins)
- [ADC Channels](#adc-channels)
- [Analog Modules](#analog-modules-fixed-silicon-pins)
- [PWM Outputs](#pwm-outputs-fixed-silicon-pins)
- [Communication Peripherals](#communication-peripherals)
- [Analog Port Configuration](#analog-port-configuration-ansel-registers)
- [Source Files](#source-files)

---

## Named GPIO Pins

| Name | Pin | Direction | Function |
|------|-----|-----------|----------|
| `SW1` | **RA12** | Input | Push-button switch 1 |
| `SW2` | **RE1** | Input | Push-button switch 2 — FOC direction reversal |
| `LED1` | **RE2** | Output | User LED 1 |
| `LED2` | **RE3** | Output | User LED 2 |
| `HALL_A` | **RD4** | Input | Hall sensor phase A |
| `HALL_B` | **RD5** | Input | Hall sensor phase B |
| `HALL_C` | **RD6** | Input | Hall sensor phase C |
| `XPRO1_CS` | **RA14** | Output | T1S LAN865x SPI chip select |
| `XPRO1_INT` | **RB3** | Input | T1S LAN865x IRQ (IOC falling edge) |
| `XPRO1_RST` | **RG0** | Output | T1S LAN865x hardware reset |

> Defined in: `mcc_generated_files/system/pins.h`

---

## ADC Channels

All three ADCs are configured as 12-bit with 4x oversampling (effective 13-bit), triggered by **PWM1 Trigger1**.

### ADC1 — Phase A Current + Motor Voltages

| Channel Name | ADC | ANx | Signal |
|---|---|---|---|
| `ADC1_IA` | ADC1 CH0 | **AD1AN0** | Phase A current (amplified via OPA1) |
| `ADC1_VM1` | ADC1 CH1 | **AD1AN3** | Motor voltage VM1 |
| `ADC1_VM2` | ADC1 CH2 | **AD1AN4** | Motor voltage VM2 |

### ADC2 — Phase B Current + Motor Voltage + Speed Reference

| Channel Name | ADC | ANx | Signal |
|---|---|---|---|
| `ADC2_IB` | ADC2 CH0 | **AD2AN0** | Phase B current (amplified via OPA2) |
| `ADC2_VM3` | ADC2 CH1 | **AD2AN4** | Motor voltage VM3 |
| `ADC2_POT` | ADC2 CH2 | **AD2AN5** | Speed reference potentiometer *(interrupt enabled → `MC_APP_ISR()`)* |

### ADC3 — DC Bus Current + Phase C Current + DC Bus Voltage

| Channel Name | ADC | ANx | Signal |
|---|---|---|---|
| `ADC3_IBUS` | ADC3 CH0 | **AD3AN0** | DC bus current |
| `ADC3_IC` | ADC3 CH1 | **AD3AN3** | Phase C current (amplified via OPA3) |
| `ADC3_VBUS` | ADC3 CH2 | **AD3AN4** | DC bus voltage |

> Defined in: `mcc_generated_files/adc/src/adc1.c`, `adc2.c`, `adc3.c`

---

## Analog Modules (Fixed Silicon Pins)

These modules use dedicated, non-remappable analog pins on the dsPIC33AK512MC510.

### On-Chip Operational Amplifiers (OPA)

| Module | Purpose |
|--------|---------|
| **OPA1** (AMP1) | Phase A current shunt amplifier — output feeds AD1AN0 |
| **OPA2** (AMP2) | Phase B current shunt amplifier — output feeds AD2AN0 |
| **OPA3** (AMP3) | Phase C current shunt amplifier — output feeds AD3AN3 |

All OPAs configured in high-power mode with NMOS/PMOS differential input pair.

> Defined in: `mcc_generated_files/opa/src/opa1.c`, `opa2.c`, `opa3.c`

### Comparator / DAC (CMP3 / DAC3) — Overcurrent Protection

**MCC-configured (CMP3_Initialize):**

| Parameter | Value | Description |
|---|---|---|
| Input pin | **CMP3A** | DC bus current signal (via OPA3) |
| `INPSEL` | 0 | Positive input = CMP3A |
| `INNSEL` | 0 | Negative input = DAC3 internal reference |
| `CMPPOL` | 0 | Non-inverted: output HIGH when overcurrent |
| `HYSSEL` | 3 | 45 mV hysteresis |
| `CBE` | 1 | Comparator blanking enabled |
| DAC threshold | 2900 counts | Corresponds to ~10 A fault threshold |
| `PGxF1PCI2` | `0x40000000` | Fault source = CMP3 output (bit 30) |

**HAL-configured (MC_HAL_OverCurrentProtectionInit):**

| Setting | Value | Description |
|---|---|---|
| Flash calibration | `CMP3_Calibrate()` | Factory trim from `0x7F20E0`: POSINLADJ / NEGINLADJ / DNLADJ + RREN |
| `TMCB` | 100 | Leading-edge blanking time (500 ns at 200 MHz Fp) |
| `PGxF1PCI1` | `0x03001000` | ACP[26:24]=3 latched, TERM[14:12]=1 cycle-by-cycle auto-terminate |

> MCC base config: `mcc_generated_files/cmp/src/cmp3.c`  
> HAL supplements: `src/hal/mc/mc_hal.c` (calibration, blanking time, PCI1 behavior)

---

## PWM Outputs (Fixed Silicon Pins)

Three high-speed PWM generators drive the 3-phase inverter in **complementary center-aligned** mode at **20 kHz** with **0.75 µs dead time**.

| Pin | Phase | Side |
|-----|-------|------|
| **PWM1H** | A | High-side |
| **PWM1L** | A | Low-side |
| **PWM2H** | B | High-side |
| **PWM2L** | B | Low-side |
| **PWM3H** | C | High-side |
| **PWM3L** | C | Low-side |

> Defined in: `mcc_generated_files/pwm_hs/src/pwm.c`

---

## Communication Peripherals

### UART1 (PPS-assigned)

| Signal | PPS Pin | Port | Config |
|--------|---------|------|--------|
| U1RX | RP47 | **RC14** | Input |
| U1TX | RP48 | **RC15** | Output (LAT initialized HIGH) |

115200 baud, 8-bit, 1 stop bit, no parity.

> Defined in: `mcc_generated_files/uart/src/uart1.c`

### SPI1 (PPS-assigned) — T1S LAN865x

| Signal | PPS Pin | Port | Direction |
|--------|---------|------|-----------|
| SCK (clock) | RP14 | **RA13** | Output |
| SDO (MOSI) | RP16 | **RA15** | Output |
| SDI (MISO) | RP69 | **RE4** | Input |
| CS (GPIO) | — | **RA14** (`XPRO1_CS`) | Output |

Mode 0, 20 MHz. DMA channel 0 (TX) and DMA channel 1 (RX).

> Defined in: `mcc_generated_files/spi_host/src/spi1.c`

### I2C3 (Fixed silicon pins)

~400 kHz (`I2C3HBRG = I2C3LBRG = 0x66` at 100 MHz peripheral clock).  
SCL and SDA are dedicated, non-PPS-remappable pins on the dsPIC33AK512MC510.

> Defined in: `mcc_generated_files/i2c_host/src/i2c3.c`

---

## Analog Port Configuration (ANSEL Registers)

Bits set to `1` in ANSEL configure the corresponding port pin as analog (disconnected from digital logic).

| Register | Value | Analog-mode pins |
|---|---|---|
| `ANSELA` | `0x0FFF` | RA0–RA11 |
| `ANSELB` | `0xFFF7` | RB0–RB15 except **RB3** (kept digital for `XPRO1_INT` IOC) |
| `ANSELC` | `0x3F27` | RC0–RC2, RC5, RC8–RC13 |
| `ANSELD` | `0xFF80` | RD7–RD15 |
| `ANSELE` | `0x0001` | RE0 |
| `ANSELF` | `0x0001` | RF0 |

> Defined in: `mcc_generated_files/system/src/pins.c`

---

## Source Files

| File | Content |
|------|---------|
| `mcc_generated_files/system/src/pins.c` | TRIS, LAT, ANSEL, PPS, IOC register initialization |
| `mcc_generated_files/system/pins.h` | Named GPIO macros (SW1, SW2, LED1, LED2, HALL_A/B/C, XPRO1_*) |
| `mcc_generated_files/adc/src/adc1.c` | ADC1: IA=AD1AN0, VM1=AD1AN3, VM2=AD1AN4 |
| `mcc_generated_files/adc/src/adc2.c` | ADC2: IB=AD2AN0, VM3=AD2AN4, POT=AD2AN5 (interrupt enabled) |
| `mcc_generated_files/adc/src/adc3.c` | ADC3: IBUS=AD3AN0, IC=AD3AN3, VBUS=AD3AN4 |
| `mcc_generated_files/pwm_hs/src/pwm.c` | PG1/PG2/PG3: complementary, center-aligned, 20 kHz |
| `mcc_generated_files/cmp/src/cmp3.c` | CMP3/DAC3 init (threshold, comparator, PCI2 source), calibration from `0x7F20E0` |
| `src/hal/mc/mc_hal.c` | OC protection: flash calibration, blanking time, PWM fault PCI1 behavior |
| `mcc_generated_files/opa/src/opa1.c` | OPA1 (Phase A shunt amplifier) |
| `mcc_generated_files/opa/src/opa2.c` | OPA2 (Phase B shunt amplifier) |
| `mcc_generated_files/opa/src/opa3.c` | OPA3 (Phase C shunt amplifier) |
| `mcc_generated_files/spi_host/src/spi1.c` | SPI1: Mode 0, 20 MHz, DMA-backed |
| `mcc_generated_files/uart/src/uart1.c` | UART1: 115200 baud, 8N1 |
| `mcc_generated_files/i2c_host/src/i2c3.c` | I2C3: ~400 kHz |
| `mcc_generated_files/dma/src/dma.c` | DMA0 = SPI1 TX, DMA1 = SPI1 RX |

---

*Copyright © Microchip Technology Inc. All rights reserved.*
