# components-mcu

## Components for my ESP-IDF projects

This is not a well designed library. APIs may change anytime if its no longerfits my needs.

MCU/CPU specific code exists only in sub-folders (e.g. "./esp32", "./host"), to enhance portability.
There were folders for esp8286 too, but since I migrated all my projects to esp32, they are gone.

This separation makes it easy to compile and run tests on host.
The tests are build using the original CMakeLists.txt from each esp-idf component.
By adding a file test_xxx.cpp into the test folder of a component, the test will automatically be included in the ctest host tests..

