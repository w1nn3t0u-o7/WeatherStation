#pragma once

namespace MZDK {
    class ComProtocol {
    public:
        virtual int writeByteData(const uint8_t reg, const uint8_t value) = 0;
        virtual int readByteData(const uint8_t reg) = 0;
        virtual int readWordData(const uint8_t reg) = 0;
        virtual int readBlockData(const uint8_t reg, uint8_t *buf, const int length) = 0;

        virtual ~ComProtocol() {}
    };
}