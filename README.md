# BZcard
An interactive Arduino business card with an OLED display and capacitive touch controls.

## Features
- Menu-driven interface on a small OLED screen
- Brick Breaker and Snake games
- Animated logo display and project info
- Sleep, wake and charging detection
- Persistent high score storage

Full documentation lives in [BZ_CARD/README.md](BZ_CARD/README.md).

## Running Unit Tests

A small GoogleTest suite lives in the `tests/` directory. It verifies
that `StorageManager::getDefaultSettings()` returns the expected
values.

To build and run the tests (requires g++ and GoogleTest installed):

```bash
cd tests
make
./test_storage
```

Use `make clean` to remove the test binary.
