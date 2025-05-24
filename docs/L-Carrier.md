# L-Carrier

![Captura de pantalla 2025-05-24 160544](https://github.com/user-attachments/assets/0f728405-d7d0-40fd-b048-7e79d37d517b)

**L-Carrier** is a logic gate audio carrier module that processes two control voltage inputs (A and B) and an audio input. It outputs audio signals routed through various logic gate conditions: AND, OR, XOR, NAND, NOR and XNOR, with visual feedback through LEDs indicating gate states.

---

## 🎛️ Controls and Parameters

This module has **no user-adjustable parameters**.

---

## 🎚️ Inputs

| Input        | Description                                  |
|--------------|----------------------------------------------|
| **A IN**     | Control voltage input A (logic gate input)  |
| **B IN**     | Control voltage input B (logic gate input)  |
| **AUDIO IN** | Audio signal input to be routed via logic gates |

---

## 🔌 Outputs

| Output         | Description                                   |
|----------------|-----------------------------------------------|
| **AND**    | Audio output gated by logical AND of A and B |
| **OR**     | Audio output gated by logical OR of A and B  |
| **XOR**    | Audio output gated by logical XOR of A and B |
| **NAND**   | Audio output gated by logical NAND of A and B|
| **NOT**    | Audio output gated by logical NOR (NOT OR) of A and B |
| **XNOR**   | Audio output gated by logical XNOR of A and B|

---

## 💡 LEDs

| LED                  | Description                                       |
|----------------------|-------------------------------------------------|
| **A LED**            | Lights when input A voltage equals 10V (logic HIGH) |
| **B LED**            | Lights when input B voltage equals 10V (logic HIGH) |
| **Circuit A LED**    | Mirrors the state of input A LED                  |
| **Circuit B LED**    | Mirrors the state of input B LED                  |
| **Circuit XOR LED**  | Lights when inputs A and B differ (XOR condition active) |

---

## 🔧 Functionality Overview

- The module reads two logic control voltages (A IN and B IN) and an audio input.
- Logic HIGH is defined as exactly 10V on the A or B inputs; any other voltage is considered LOW.
- Outputs route the audio input through gates corresponding to basic logical operations:
  - **AND**: Audio passes only if both A and B are HIGH.
  - **NAND**: Audio passes if NOT(AND) condition is met.
  - **OR**: Audio passes if either A or B is HIGH.
  - **NOR**: Audio passes if neither A nor B is HIGH.
  - **XOR**: Audio passes if exactly one of A or B is HIGH.
  - **XNOR**: Audio passes if both A and B are equal (both HIGH or both LOW).
- Corresponding LEDs illuminate to indicate active logic states for inputs and XOR condition.
- When a logic condition is not met, its paired output is muted (0V).

---

## 💡 Creative Uses

- Use as a dynamic audio gate controlled by logic CV inputs.
- Combine with clock or trigger signals to create complex rhythmic gating patterns.
- Apply logic-based audio routing for creative patching and modulation.
- Visual feedback LEDs help monitor logic states live during performance.

---

[⬅ Back to Module Index](../README.md)
