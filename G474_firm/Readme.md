# Bootloader & Firmware Version Management and Jump Mechanism

## 1. Version / Name Section

### 1.1 Add `.version` Section (fw_boot.c)

Define a dedicated section in the linker script:

```
.version :
{
    . = ALIGN(1024);
    KEEP(*(.version))
    . = ALIGN(4);
} > FLASH
```

* Start address: **0x08010400**
* Ensures that the ISR vector table does not exceed 1 KB
* Stores firmware version and name information

---

### 1.2 Store Firmware Version and Name

__attribute((section(".version"))) firm_version_t firm_version =
{
  "V260403R1",
  "STM32F411"
};

* Uses a struct (firm_version_t)
* Forces placement into the `.version` section

---

## 2. Accessing Firmware Info from Bootloader

### 2.1 Pointer Access

```c
firm_version_t *fw = (firm_version_t *)0x08010400;
```

* Direct access using a fixed memory address

---

### 2.2 Command Response

* Use this data in CLI or communication responses
* Example: return firmware version and name

---

## 3. Bootloader Version / Name

Same approach as firmware, but with a different address:


# Firmware → Bootloader Jump Mechanism
## Concept

Trigger a system reset and use a backup register to indicate bootloader entry.

Timeout
WriteBackupReg();
NVIC_SystemReset();

---

## 1. RTC Backup Register

* Retains value after reset
* Used to store flag

---

## 2. ResetBoot Function

### 2.1 Timeout (SysTick)

* Wait for a defined timeout before jumping to firmware

### 2.2 Write Boot Flag

* WriteBackupReg(BOOT_MODE);

### 2.3 Trigger Reset

* NVIC_SystemReset();

---

# Bootloader Entry Methods

## 1. CLI Command

* Register command using `cliAdd()`
* Call `resetBoot()` to enter bootloader

---

## 2. Button

* Use `NRST` pin and `_DEF_BUTTON_1`
* Use Rset count

---

## 3. 1200 Baudrate Trick (USB CDC)

### 3.1 PC Side

uartOpenPort(ch, name, 1200);
uartTransmit("boot 5555AAAA");
delay(1000);  // Important

### ⚠️ Important Note

* The delay is critical.

If the delay is too short:

while Firmware jumps to bootloader PC opens port 57600 buadrate
* CLI connection fails

---

---

### 3.2 Firmware Side (USB CDC)

**File: `usbd_cdc_if.c`**

#### Detect Baudrate

*  `CDC_SET_LINE_CODING`
* Check if baudrate is set to **1200**

#### Receive Command

* Function: `CDC_Receive_FS`
* Check for the string:

```
"boot 5555AAAA"
```

# Summary

* Use a dedicated `.version` section to store version and name
* Access data via fixed memory address
* Use RTC backup register to control boot mode
* Provide multiple bootloader entry methods:

  * CLI command
  * Button input
  * 1200 baudrate trigger (USB CDC)

---
