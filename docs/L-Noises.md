# L-Noises

**L-Noises** is a dual-channel stereo noise generator with logic gates controlling the output of each channel. It’s useful for creating rhythmic textures, pseudo-random bursts, and logic-driven noise ambiences.

---

## 🎛️ Controls and Parameters

| Type        | Name        | Description |
|-------------|-------------|-------------|
| Switch      | **Noise L** | Selects the noise type for the left channel: `Pink`, `White`, or `Brown` |
| Switch      | **Noise R** | Selects the noise type for the right channel: `Pink`, `White`, or `Brown` |
| Switch      | **Gate L**  | Logic gate used for enabling left output: `AND`, `OR`, `XOR`, `NAND`, `NOR`, `XNOR` |
| Switch      | **Gate R**  | Logic gate used for enabling right output: `AND`, `OR`, `XOR`, `NAND`, `NOR`, `XNOR` |
| Knob        | **Vol L**   | Left output volume (0–100%) |
| Knob        | **Vol R**   | Right output volume (0–100%) |

---

## 🎚️ Inputs

| Input        | Description |
|--------------|-------------|
| **Signal A** | Binary signal A (active if > 5V) |
| **Signal B** | Binary signal B (active if > 5V) |

These signals are interpreted as logic levels and are processed through the selected gates for each channel.

---

## 🔌 Outputs

| Output       | Description |
|--------------|-------------|
| **Noise L**  | Left channel noise output, enabled by gate logic and scaled by volume |
| **Noise R**  | Right channel noise output, enabled by gate logic and scaled by volume |

---

## 💡 Indicator LEDs

| LED       | Description |
|-----------|-------------|
| **LED A** | Lights up when Signal A > 5V |
| **LED B** | Lights up when Signal B > 5V |

---

## 🔧 Noise Types

- **White**: Pure white noise with uniform spectrum.
- **Pink**: Filtered white noise with more energy in lower frequencies.
- **Brown**: Brownian (integrated) noise, resulting in smoother, softer changes.

---

## 🔗 Logic Gate Behavior

Each channel only emits noise when its configured logic gate evaluates `true` for the given combination of Signal A and Signal B. This makes it ideal for use as a rhythmically gated noise source or logic-based noise trigger.

---

## 🛠️ Technical Details

- Brown noise includes a simple attack envelope (~12 samples) for smoother transients.
- Both channels share the same white noise source to allow for correlated or contrasting behaviors.
- Noise output is muted entirely when the logic condition is not met.

---

## 💡 Creative Tips

- Use two LFOs or gate sequencers as Signal A and B to rhythmically modulate the noise outputs.
- Combine different noise types (e.g., Pink Left + Brown Right) for wide stereo effects.
- Patch the outputs through filters or envelopes for dynamic sound shaping.

---

[⬅ Back to Module Index](./README.md)
