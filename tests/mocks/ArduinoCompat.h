#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

#ifndef constrain
#define constrain(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#endif // ARDUINO_COMPAT_H
