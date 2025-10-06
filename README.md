I didnt have a coil for my tractor, but had one for a mazda and an arduino and an optical encoder sensor.

This is my functional attempt at replacing a single cylinder engine coil to electronic ignition.
It should be able to work on any single cylinder engine provided you have access to the crank to mount a sensor (or cam if you want to avoid wasted spark).
My sensor is mounted where it sees a high signal somewhere near BDC on the compression stroke and the timing compensation is in the code. If you have the sensor too early, slow rpm timing events could be off too much and too close to TDC will result in not being able to get the spark out fast enough.


Always open to suggestions or improvements, I'd like to solve the wasted spark but the engine is not an OHV or OHC so I have no access to the valves - either need to detect in code the slightly slower compression stroke (if it can even be done effectively) or add a sensor the the exhaust or intake for air pressure.
It would be a fun project to add fuel injection to this, then I could get it to idle perfectly at a very low RPM (under 400 RPM, starter works at 600 RPM). AND adding an O2 sensor to the fuel injector could allow easy use of multiple different fuels - e85 or mash whatever laying around together.
