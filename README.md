# aridacity VCV Rack plugin
![modules preview](/preview.png)

# Overview of modules

**clkDiv**

A clock divider or basic sequencer with 16 divisions/steps. Move the `seq` switch up to enable the sequencer mode. When connected, the `in` value will be sent to the outputs instead of the clock `clk`. When `rst` is high, the internal clock will reset on the next clock high.

**bCrush**

A bit crusher with bit manipulation. `rate` in and knob controls the sample rate (horizontal) of the signal; `res` in and knob, control the amplitude resolution (vertical) of the signal. The bit operators are accumulative and affected by the `res` input and knob. `hold` is a sample and hold clock input that will override the sample rate controls.

**Clip**

A hard clipping limiter with inner `push` and outer limits `limit`, and center offsets `pos` for each. `pull` zeros the inner limit and `clip` enables/disables the outer limit.

**Remainder Fold**

A wavefolder using remainder division. `shape` changes between unsigned and signed remainder division. `fold` sets the voltage limit where folding/wrapping occurs.

Build instructions are in the [VCV Rack Manual](https://vcvrack.com/manual/Building.html#Building-Rack-plugins)
