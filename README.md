# aridacity VCV Rack plugin
![modules preview](/preview.png)

# Overview of modules
**clkDiv**

A clock divider or basic sequencer with 16 divisions/steps. Move the `seq` switch up to enable the sequencer mode. If `in` is connected it's value will be sent to the outputs instead of the clock.

**bCrush**

A bit crusher with bit operations. `rate` input and it's knob control the sample rate (horizontal) of the signal; `res` input and it's knob, control the amplitude resolution (vertical) of the signal. The bit operators are accumulative and affected by the `res` input and knob. `hold` is a sample and hold clock input that will override the sample rate controls.

**Clip**

A hard clipping limiter with inner and outer limits `push/limit` and center offsets `pos` for each. `pull` zeros the inner limit and `clip` enables/disables the outer limit.

Building instructions are in the [VCV Rack Manual](https://vcvrack.com/manual/Building.html#building-rack-plugins)

# Licenses
All **source code** is copyright © 2019 aridacity and licensed under [BSD-3-Clause](LICENSE)

All **panel and component graphics** in `res/*` are copyright © 2019 aridacity and licensed under [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/).
