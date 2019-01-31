# aridacity-VCVRackPlugins
![aridacity pluggins preview](/preview.png)

A set of pluggins for VCV Rack.
Building instructions are in the [VCV Rack Manual](https://vcvrack.com/manual/Building.html#building-rack-plugins)


**clock Div**

A clock divider or basic sequencer with 16 divisions/steps. Move the `seq switch` up to enable the sequencer mode. The `in` input's value will be sent to the outputs if connected

**bit Crush**

A bit crusher with bit manipulation. `rate` in and knob controls the sample rate (horizontal) of the signal; `res` in and knob, control the amplitude resolution (vertical) of the signal. `hold` is a sample and hold clock input that will override the sample rate controls.

**clip**

A hard clipper with inner `push` and outer limits `limit` and center offsets `pos` for each. `pull` zeros the inner limit and `clip` enables/disables the outer limit.

# Licenses
All **source code** is copyright © 2019 aridacity and licensed under [BSD-3-Clause](LICENSE)

All **panel and component graphics** in `res/*` are copyright © 2019 aridacity and licensed under [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/).
