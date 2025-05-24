# L-Rantics

**L-Rantics** is a dual-channel random voltage generator controlled by clock (tic) inputs. It outputs random voltages triggered by gate signals or in autonomous random tic mode, with configurable polarity and level spread.

---

## 🎛️ Controls and Parameters

| Type     | Name           | Description |
|----------|----------------|-------------|
| Switch   | **LvL L**      | Left channel level spread, 9 steps labeled 1 to 9 |
| Switch   | **LvL R**      | Right channel level spread, 9 steps labeled 1 to 9 |
| Knob     | **Polarity**   | Output polarity mode: `0` = Bipolar (± voltage), `1` = Unipolar (positive only) |
| Knob     | **Tic Selector** | Clock mode: `0` = external clock inputs, `1` = internal random clock mode |

---

## 🎚️ Inputs

| Input      | Description                      |
|------------|---------------------------------|
| **Gate L** | Left channel gate clock input (triggers new random voltage on rising edge) |
| **Gate R** | Right channel gate clock input (triggers new random voltage on rising edge) |
| **LvL CV L** | CV input to control left channel level spread (overrides knob) |
| **LvL CV R** | CV input to control right channel level spread (overrides knob) |

---

## 🔌 Outputs

| Output     | Description              |
|------------|--------------------------|
| **Random L** | Left channel random voltage output |
| **Random R** | Right channel random voltage output |

---

## 💡 LEDs

| LED              | Description                  |
|------------------|------------------------------|
| **Unipolar LED**  | Lights when polarity is set to unipolar mode |
| **Bipolar LED**   | Lights when polarity is bipolar |

---

## 🔧 Functionality Overview

- Generates random voltages in the range from approximately -1V to +1V, modulated by the level spread parameter (1 to 9).
- When polarity is **bipolar**, output voltages range roughly from -1V to +1V.
- When polarity is **unipolar**, output voltages are positive only (0 to +1V).
- The module responds to rising edges of gate clock signals on **Gate L** and **Gate R** inputs, producing a new random voltage for the corresponding channel.
- If no clock inputs are connected or the tic selector parameter is set to random mode, the module generates random clock tics internally at random intervals (between 0.2 and 1.0 seconds approx).
- Level spread knobs or CV inputs control the base level from which the random voltage is offset.

---

## ⚙️ Detailed Behavior

- **Level spread inputs:** If a CV input for level spread is connected, it overrides the knob for that channel. The input voltage is clamped between -5V and +5V, then shifted to a 1–11 scale.
- **Clock modes:**  
  - **External clocks:** Uses rising edges on Gate L and Gate R inputs to trigger new random voltages independently. If only one input is connected, its clock signal is mirrored to the other channel.  
  - **Random clock:** Generates random tics internally with a 50% probability on each tic event.
- **Random voltage generation:** At each tic, a new random voltage is generated within the defined range, offset by the level spread and multiplied by the polarity factor.
- **Outputs hold last voltage** when no new tic is detected.

---

## 💡 Creative Uses

- Use external rhythmic clock signals to generate pseudo-random modulation voltages.
- Switch to random tic mode for unpredictable generative patches.
- Use CV inputs to dynamically modulate level spread in performance or automation.
- Create complex stereo modulation by independently controlling left and right channels.

---

[⬅ Back to Module Index](../README.md)
