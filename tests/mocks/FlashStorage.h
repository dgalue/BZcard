#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

template <typename T>
class FlashStorageClass {
public:
    T data{};
    T read() const { return data; }
    void write(const T& value) { data = value; }
};

#define FlashStorage(name, Type) FlashStorageClass<Type> name

#endif // FLASH_STORAGE_H
