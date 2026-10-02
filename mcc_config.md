# MCC Melody Peripheral Configuration

All peripherals are configured using **MPLAB Code Configurator (MCC) Melody**. The MCC project file is located at `dsPIC33AK512MC510_MCLV48V300W_T1S/mcc/dspic33ak512_t1s.mc3`. Generated driver source files are in `dsPIC33AK512MC510_MCLV48V300W_T1S/mcc_generated_files/` and should never be edited manually — regenerate from MCC instead.

<img src="assets/MCC/mcc_resources.png" alt="MCC Project Resources" title="All resources included in the MCC project" width="30%">

## Motor Control Peripherals

### Pin Manager

Pin assignments for motor control signals, Hall sensor inputs (RD4/RD5/RD6 — configured but not used by the sensorless FOC), user buttons (RA12/RB6), LEDs, and communication interfaces (SPI1, I2C3, UART1).

<img src="assets/MCC/pin_manager.png" alt="Pin Manager" title="MCC Pin Manager configuration" width="40%">

### PWM (PG1 / PG2 / PG3)

Three PWM generators in center-aligned complementary mode at 20 kHz (400 MHz PWM clock). Dead time: 0.75 µs. Master period set by PG1. All generators use independent duty cycle registers (PG1DC, PG2DC, PG3DC). Generator 1 additionally generates trigger signals to the ADCs. Generators 2 and 3 are configured identically to Generator 1 (parts 1 and 2) without the ADC trigger output.

<img src="assets/MCC/pwm_master.png" alt="PWM Master" title="MCC PWM Master settings" width="50%">

<img src="assets/MCC/pwm_gen1_1.png" alt="PWM Generator 1 — Part 1" title="MCC PWM Generator 1 settings — general" width="50%">

<img src="assets/MCC/pwm_gen1_2.png" alt="PWM Generator 1 — Part 2" title="MCC PWM Generator 1 settings — duty/dead-time" width="50%">

<img src="assets/MCC/pwm_gen1_3.png" alt="PWM Generator 1 — Trigger" title="MCC PWM Generator 1 settings — ADC trigger" width="50%">

### ADC1 / ADC2 / ADC3

All ADC modules use **Clock Generator 6** as their clock source.

- **ADC1** — Phase A current (Ia) via OPA1 output
- **ADC2** — Phase B current (Ib) via OPA2 output, potentiometer (speed reference). PWM trigger drives 20 kHz sampling
- **ADC3** — DC bus current (Ibus) via OPA3 output, DC bus voltage (Vbus)

<img src="assets/MCC/adc_clk.png" alt="ADC Clock" title="MCC ADC shared clock configuration" width="50%">

<img src="assets/MCC/adc1.png" alt="ADC1 Configuration" title="MCC ADC1 configuration" width="80%">

<img src="assets/MCC/adc2.png" alt="ADC2 Configuration" title="MCC ADC2 configuration" width="80%">

<img src="assets/MCC/adc3.png" alt="ADC3 Configuration" title="MCC ADC3 configuration" width="80%">

### CMP3 / DAC3

Comparator 3 monitors the DC bus current (CMP3A input) against a DAC3 reference for hardware overcurrent protection. DAC3 default value: 2900 counts (valid range: 0–3890 / 0x000–0xF32). Comparator blanking is enabled (CBE) with 45 mV hysteresis. Leading-edge blanking (TMCB = 500 ns) is configured in firmware post-init.

<img src="assets/MCC/cmp3_dac3.png" alt="CMP3 DAC3 Configuration" title="MCC CMP3/DAC3 configuration" width="50%">

### OPA (OPA1 / OPA2 / OPA3)

On-chip operational amplifiers configured for current sense signal conditioning. The screenshot shows OPA1; **OPA2 and OPA3 are configured identically**. OPA1 and OPA2 amplify phase current shunt signals for ADC1 and ADC2. OPA3 amplifies the DC bus current shunt signal for ADC3 and CMP3.

<img src="assets/MCC/opa.png" alt="OPA1 Configuration" title="MCC OPA1 configuration" width="50%">

## System and Communication Peripherals

### Clock Configuration

External Input Clock Source is **None**. System clock: 200 MHz FCY. Peripheral clock: 200 MHz (Fp). PWM clock: 400 MHz.

<img src="assets/MCC/clock_pll.png" alt="Clock PLL" title="MCC Clock PLL configuration" width="50%">

<img src="assets/MCC/clock_system.png" alt="Clock System" title="MCC System clock configuration" width="50%">

<img src="assets/MCC/clock_pwm.png" alt="Clock PWM" title="MCC PWM clock configuration" width="50%">

<img src="assets/MCC/clock_adc.png" alt="Clock ADC" title="MCC ADC clock configuration" width="50%">

<img src="assets/MCC/clock_dac.png" alt="Clock DAC" title="MCC DAC clock configuration" width="50%">

<img src="assets/MCC/clock_spi.png" alt="Clock SPI" title="MCC SPI clock configuration" width="50%">

<img src="assets/MCC/clock_ccp.png" alt="Clock CCP" title="MCC CCP clock configuration" width="50%">

### SPI1

SPI1 at 20 MHz communicates with the LAN8651 10BASE-T1S MAC-PHY.

<img src="assets/MCC/spi.png" alt="SPI1 Configuration" title="MCC SPI1 configuration" width="50%">

### DMA (DMA0 / DMA1)

DMA channels 0 and 1 provide TX/RX acceleration for zero-copy TC6 frame transfers between SPI1 and the LAN8651.

<img src="assets/MCC/dma0.png" alt="DMA0 Configuration" title="MCC DMA Channel 0 configuration" width="50%">

<img src="assets/MCC/dma1.png" alt="DMA1 Configuration" title="MCC DMA Channel 1 configuration" width="50%">

### I2C3

I2C3 reads the MAC address from the EEPROM (address 0x58) on the LAN8651 Click board at startup.

<img src="assets/MCC/i2c.png" alt="I2C Configuration" title="MCC I2C3 configuration" width="50%">

### UART1

UART1 provides printf debug output (startup banner and status messages). Used for development diagnostics only.

<img src="assets/MCC/uart.png" alt="UART Configuration" title="MCC UART1 configuration" width="50%">

### Timers (TMR1 / SCCP1)

- **TMR1** — System tick timer (10 µs resolution) for non-real-time timing (LED heartbeat, button debounce, lwIP timeouts)
- **SCCP1** — FOC execution time measurement timer. Started/stopped around the FOC ISR to measure loop time (~15.6 µs)

<img src="assets/MCC/tmr1.png" alt="TMR1 Configuration" title="MCC TMR1 — System tick timer" width="60%">

<img src="assets/MCC/timerMeasurement.png" alt="Timer Measurement Configuration" title="MCC SCCP1 — FOC execution time measurement" width="60%">
