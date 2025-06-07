# BZcard
nothing to see
https://github.com/willow2ko/BZcard/raw/main/package_bzcard_index.json

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
