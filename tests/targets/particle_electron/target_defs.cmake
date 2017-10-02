# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

theCore_enable_platform(particle_electron)
theCore_electron_enable_console()

# Give theCore ability to use timers in this target
theCore_electron_use_systmr()
