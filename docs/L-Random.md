# L-Random

![Captura de pantalla 2025-05-24 160541](https://github.com/user-attachments/assets/516707d6-52da-42ed-af0a-4d70a18abf7e)

**L-Random** is a random voltage generator module with independent bipolar and unipolar outputs for left and right channels. It provides control over frequency and value spread, with options for global or individual parameters.

---

## 🎛️ Controls and Parameters

| Control/Parameter         | Description                                  | Range       |
|---------------------------|----------------------------------------------|-------------|
| **GENERAL SWITCH**        | Enables global parameters for all channels   | `0 (OFF) / 1 (ON)` |
| **Freq (x4)**             | Adjusts change frequency (L/R bipolar/unipolar) | `1–9` (50–450ms) |
| **Spread (x4)**           | Controls random value spread (L/R bipolar/unipolar) | `1–9` |
| **GENERAL Freq**          | Global frequency when GENERAL SWITCH is ON   | `1–9` |
| **GENERAL Spread**        | Global spread when GENERAL SWITCH is ON      | `1–9` |

---

## 🎚️ Inputs

| Input                | Description                                  |
|----------------------|----------------------------------------------|
| **Freq CV (x4)**     | CV input for frequency (separate for L/R bipolar/unipolar) |
| **Spread CV (x4)**   | CV input for spread (separate for L/R bipolar/unipolar) |

*Note: CV inputs expect `-5V to +5V` signals.*

---

## 🔌 Outputs

| Output               | Description                                  |
|----------------------|----------------------------------------------|
| **Left bipolar**     | Random bipolar signal (`-XV to +XV`)         |
| **Left unipolar**    | Rectified random signal (`0V to +XV`)       |
| **Right bipolar**    | Random bipolar signal (`-XV to +XV`)         |
| **Right unipolar**   | Rectified random signal (`0V to +XV`)        |

---

## 🔧 Functionality Overview

- Generates random voltages with 4 independent outputs:
  - **Bipolar**: Oscillates between negative and positive values
  - **Unipolar**: Positive values only (rectified)
- Two operation modes:
  - **Individual**: Separate parameters per channel (frequency/spread)
  - **Global**: Unified parameters for all outputs (activated by GENERAL SWITCH)
- CV conversion:
  - Frequency inputs: `-5V to +5V` → mapped to `1–9`
  - Spread inputs: adjusted based on output type (bipolar/unipolar)
- Timing:
  - Frequency controls update interval (`1` = 50ms, `9` = 450ms)

---

## 💡 Creative Uses

- Chaotic modulation source for VCOs or filters
- Random rhythm generation (convert to gates using comparators)
- Evolving texture creation by modulating frequency/spread with LFOs
- Spatial sound design using independent L/R outputs
- Randomized synth when patched to effect parameters

---

[⬅ Back to Module Index](../README.md)
