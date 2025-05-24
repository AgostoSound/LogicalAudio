# LogicalAudio Plugin Suite

**LogicalAudio** is a suite of experimental synthesis modules for **VCV Rack 2**, focused on the intersection of audio processing, propositional logic, and random voltage generation.

Each module is designed as a creative tool for modular synthesists, composers, and sound designers interested in pushing the boundaries of logical structures within a modular audio environment.

![logicl-audio-suite](https://github.com/user-attachments/assets/dc47f6f6-ea84-4129-adf3-fb9836ba96c5)


---

## 📦 Included Modules

### • L-Panel  
A decorative front panel module used for visual balance or thematic integration in your patch. No audio or CV functionality.

---

### • L-Carrier  
A dual-input logic processor that routes an incoming audio signal through one of six classic logic gates: **AND**, **OR**, **XOR**, **NAND**, **NOR**, and **XNOR**.  
The selected logic operation is dynamically determined by two control voltage inputs, **A** and **B**, enabling expressive modulation and dynamic logic-based audio shaping.

---

### • L-Random  
A four-channel random voltage generator with two bipolar outputs (**L-R**) and two unipolar outputs (**LR-RR**).  
Each output generates voltages within a ±1V spread around a center voltage defined by the **LvL** knob.  
The generation frequency is controlled via the **Freq** knob.  
Activating the **Just One** switch links all outputs to global controls and synchronizes voltages within each pair, offering more coherent generative behavior.

---

### • L-Rantics  
A dual random voltage generator with configurable **bipolar/unipolar** behavior and individual **gate** control.  
Each side (L and R) can be independently triggered by gate inputs, or simultaneously if only one gate is connected.  
The **LvL** knob controls the spread around a central voltage, and an internal "Vampi" generator can provide random ticks for continuous modulation.  
The module offers fine control for generative and reactive CV sequences, with logic-like gate conditions for voltage updates.

---

### • L-Noise  
A flexible noise generator that blends classic noise types (**white**, **pink**, **brown**) with propositional logic control.  
Two logical inputs (**A** and **B**) modulate parameters based on the selected logic operation (AND, OR, XOR, etc.).  
Includes optional **CV input**, a **low-pass filter** control, **amplitude** adjustment, **stereo panning**, and logic-based modulation for expressive noise shaping.


---

## 📥 Installation

1. Clone this repository into your VCV Rack plugins folder:
   ```bash
   git clone https://github.com/your-username/logicalaudio.git
   ```

2. Build it using the VCV Rack Plugin Development Toolchain:
   See the official guide: VCV Rack Plugin Development

