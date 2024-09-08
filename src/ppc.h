#ifndef PPC_H
#define PPC_H

#ifdef __PPC__
#include <iostream>
#ifdef __GCN__
#include <ogcsys.h>
#include <gccore.h>
#include <SDL2/SDL.h>
#include <fat.h>
#elif __WII__
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <SDL2/SDL.h>
#include <fat.h>
#endif

//Generic file copy function.
int cp(const char *to, const char *from);

//Init the target system
void sys_init();

#define access checkFile

class little_int16_t {
public:
    little_int16_t(int16_t value = 0) {
        set_value(value);
    }

    int16_t get_value() const {
        return (static_cast<int16_t>(data[0]) << 0) |
               (static_cast<int16_t>(data[1]) << 8);
    }

    void set_value(int16_t value) {
        data[0] = static_cast<uint8_t>(value >> 0);
        data[1] = static_cast<uint8_t>(value >> 8);
    }

    friend std::ostream& operator<<(std::ostream& os, const little_int16_t& obj) {
        os << std::hex << obj.get_value();
        return os;
    }

private:
    uint8_t data[2];
};

class little_uint16_t {
public:
    little_uint16_t(uint16_t value = 0) {
        set_value(value);
    }

    uint16_t get_value() const {
        return (static_cast<uint16_t>(data[0]) << 0) |
               (static_cast<uint16_t>(data[1]) << 8);
    }

    void set_value(uint16_t value) {
        data[0] = static_cast<uint8_t>(value >> 0);
        data[1] = static_cast<uint8_t>(value >> 8);
    }

    friend std::ostream& operator<<(std::ostream& os, const little_uint16_t& obj) {
        os << std::hex << obj.get_value();
        return os;
    }

private:
    uint8_t data[2];
};

class little_int32_t {
public:
    little_int32_t(int32_t value = 0) {
        set_value(value);
    }

    int32_t get_value() const {
        return (static_cast<int32_t>(data[0]) << 0) |
               (static_cast<int32_t>(data[1]) << 8) |
               (static_cast<int32_t>(data[2]) << 16) |
               (static_cast<int32_t>(data[3]) << 24);
    }

    void set_value(int32_t value) {
        data[0] = static_cast<uint8_t>(value >> 0);
        data[1] = static_cast<uint8_t>(value >> 8);
        data[2] = static_cast<uint8_t>(value >> 16);
        data[3] = static_cast<uint8_t>(value >> 24);
    }

    friend std::ostream& operator<<(std::ostream& os, const little_int32_t& obj) {
        os << std::hex << obj.get_value();
        return os;
    }

private:
    uint8_t data[4];
};

class little_uint32_t {
public:
    little_uint32_t(uint32_t value = 0) {
        set_value(value);
    }

    uint32_t get_value() const {
        return (static_cast<uint32_t>(data[0]) << 0) |
               (static_cast<uint32_t>(data[1]) << 8) |
               (static_cast<uint32_t>(data[2]) << 16) |
               (static_cast<uint32_t>(data[3]) << 24);
    }

    void set_value(uint32_t value) {
        data[0] = static_cast<uint8_t>(value >> 0);
        data[1] = static_cast<uint8_t>(value >> 8);
        data[2] = static_cast<uint8_t>(value >> 16);
        data[3] = static_cast<uint8_t>(value >> 24);
    }

    friend std::ostream& operator<<(std::ostream& os, const little_uint32_t& obj) {
        os << std::hex << obj.get_value();
        return os;
    }

private:
    uint8_t data[4];
};


#ifdef __GCN__
#define SDMC "sd:/"
#define HDFS "fat:/"
#define RAP_SD_DIR SDMC "gcn/Raptor/"
#define RAP_HD_DIR HDFS "gcn/Raptor/"
#elif __WII__
#define SDMC "sd:/"
#define HDFS "fat:/"
#define RAP_SD_DIR SDMC "wii/Raptor/"
#define RAP_HD_DIR HDFS "wii/Raptor/"
#endif

#endif
#endif