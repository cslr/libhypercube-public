# libhypercube-public
C++ binary repository (DLL C API) of (audio synthesizer) parameter reduction library for Windows (Visual C++, MSYS2 MINGW)

Parameter reduction makes it easier to find new sounds when exploring only 2D/3D space of "good sounds".

LibHypercube is a library that can be used to reduce parameters of controller so that finding good parameters (of an audio plugin) is easier. Machine learning uses existing good preset values to find lower dimensional space of good parameters/sounds. You need reasonably number of presets to calculate parameter reduction.

Because a parameter reconstruction to higher dimensional original parameter space is done using neural networks, it requires many cycles of CPU time and you cannot use automation in DAW to keep changing those parameters in 44.100 Hz times per second or something. You can change parameters and then play instrument offline manner but cannot keep changing parameters real-time.

YouTube example videos: 

https://www.youtube.com/watch?v=uSAzedM0fJ0

https://www.youtube.com/watch?v=oPZDn4Ya_ZA
