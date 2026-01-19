#include <Arduino.h>
#include <stdint.h>
#include <cstring>

//#define FLASH_PAGE_SIZE 0x2000  // 8 KB page, was 4kB 0x1000 before
#define FLASH_PAGE_SIZE 0x8000    // 32 KB page, was 8kB 0x1000 before, notably sluggish with 64kB

#define EEPROM_PAGE_POOL 8                 // Number of rotating 32 KB pages
#define EEPROM_PAGE_HEADER_SIZE 32         // Bytes reserved at top of each page for metadata
static_assert(EEPROM_PAGE_HEADER_SIZE < FLASH_PAGE_SIZE, "EEPROM header must fit inside a page");

constexpr uint32_t kEepromPageMagic = 0x4550524DL; // 'EPRM'
constexpr bool kEnableEepromWrites = true;         // #define DONT_CLEAR_USER_FUNCTIONS on first flashing.

// Wear-leveling diagnostics
#define ENABLE_WRITE_COUNTERS 0  // comment to disable for production
#if ENABLE_WRITE_COUNTERS
#define WRITE_COUNTER_SIZE 256   // Track first 256 logical addresses
static uint32_t writeCounters[WRITE_COUNTER_SIZE] = {0};
#endif

// Wear-leveling for frequently written single-byte values
#define WL_BRIGHTNESS_SLOTS 8    // Rotate brightness across 8 addresses
#define WL_BRIGHTNESS_START 2048 // 256b with MEMNR=10, 2048b with MEMNR=100
#define WL_BRIGHTNESS_INDEX (WL_BRIGHTNESS_START + WL_BRIGHTNESS_SLOTS)
#define WL_DARKTIME_SLOTS 4
#define WL_DARKTIME_START 272
#define WL_DARKTIME_INDEX (WL_DARKTIME_START + WL_DARKTIME_SLOTS)

static inline uint32_t bootloaderStartAddr() {
    constexpr uint32_t BOOTLOADERADDR_REG = 0x10001014UL;
    return *((volatile uint32_t*)BOOTLOADERADDR_REG);
}

static inline uint32_t eepromFlashBase() {
    uint32_t pageSize = NRF_FICR->CODEPAGESIZE;
    uint32_t totalSize = pageSize * NRF_FICR->CODESIZE;
    uint32_t bootAddr = bootloaderStartAddr();
    if (bootAddr == 0xFFFFFFFF) bootAddr = totalSize;
    uint32_t poolSize = FLASH_PAGE_SIZE * EEPROM_PAGE_POOL;
    if (bootAddr < poolSize) bootAddr = poolSize;
    return bootAddr - poolSize; // Lowest address of the rotating pool
}

class EEPROMEmu {
public:
    EEPROMEmu();

#if ENABLE_WRITE_COUNTERS
    void printWriteStats();
    void resetWriteStats();
#endif

    template <typename T>
    void put(int offset, const T& value) {
        ensureInitialized();
        const uint8_t* src = reinterpret_cast<const uint8_t*>(&value);
        size_t len = sizeof(T);
        uint32_t targetAddr = physicalDataAddress(offset);

#if ENABLE_WRITE_COUNTERS
        for (size_t i = 0; i < len && (offset + i) < WRITE_COUNTER_SIZE; ++i) {
            writeCounters[offset + i]++;
        }
#endif

        programBytes(targetAddr, src, len);
    }

    template <typename T>
    void get(int offset, T& value) {
        ensureInitialized();
        const uint8_t* src = reinterpret_cast<const uint8_t*>(physicalDataAddress(offset));
        std::memcpy(&value, src, sizeof(T));
    }

    int length() {
        return FLASH_PAGE_SIZE - EEPROM_PAGE_HEADER_SIZE;
    }

    uint8_t operator[](int address) {
        uint8_t val;
        get(address, val);
        return val;
    }

    void set(int address, uint8_t val) {
        put(address, val);
    }

    bool beginPageRewrite();
    uint32_t activePageBase() const { return pageBase(activePage); }

    // ----- wear-level helpers for small single-byte slots -----
    uint8_t findNextSlot(uint8_t baseAddr, uint8_t numSlots);
    uint8_t findCurrentSlot(uint8_t baseAddr, uint8_t numSlots);
    void putBrightness(uint8_t brightness);
    uint8_t getBrightness();
    void putDarktime(uint8_t darktime);
    uint8_t getDarktime();

private:
    struct PageHeader {
        uint32_t magic;
        uint32_t generation;
        uint32_t generationInverse;
        uint32_t reserved;
    };

    void ensureInitialized();
    void initPool();
    uint32_t poolBase() const { return poolBaseAddr; }
    uint32_t pageBase(uint8_t idx) const { return poolBaseAddr + static_cast<uint32_t>(idx) * FLASH_PAGE_SIZE; }
    uint32_t dataBase(uint8_t idx) const { return pageBase(idx) + EEPROM_PAGE_HEADER_SIZE; }
    uint32_t physicalDataAddress(int offset) const { return dataBase(activePage) + static_cast<uint32_t>(offset); }
    void programBytes(uint32_t targetAddr, const uint8_t* src, size_t len);
    void eraseLogicalPage(uint8_t pageIdx);
    bool readHeader(uint8_t pageIdx, PageHeader& out) const;
    void writeHeader(uint8_t pageIdx, uint32_t generation);
    bool rotateToNextPage();
    uint8_t nextPageIndex(uint8_t idx) const { return (idx + 1) % EEPROM_PAGE_POOL; }

    uint32_t poolBaseAddr;
    uint8_t activePage;
    uint32_t activeGeneration;
    bool initialized;
};

inline EEPROMEmu::EEPROMEmu()
    : poolBaseAddr(0), activePage(0), activeGeneration(0), initialized(false) {}

#if ENABLE_WRITE_COUNTERS
inline void EEPROMEmu::printWriteStats() {
    if (!Serial) return;
    Serial.println(F("\n=== EEPROM WRITE STATISTICS ==="));
    uint32_t total = 0;
    for (int i = 0; i < WRITE_COUNTER_SIZE; ++i) {
        if (writeCounters[i] > 0) {
            Serial.print(F("Addr 0x"));
            if (i < 16) Serial.print('0');
            Serial.print(i, HEX);
            Serial.print(F(": "));
            Serial.println(writeCounters[i]);
            total += writeCounters[i];
        }
    }
    Serial.print(F("Total byte writes: "));
    Serial.println(total);
    Serial.println(F("==============================\n"));
}

inline void EEPROMEmu::resetWriteStats() {
    memset(writeCounters, 0, sizeof(writeCounters));
    if (Serial) Serial.println(F("[EEPROM] Write counters reset"));
}
#endif

inline void EEPROMEmu::ensureInitialized() {
    if (!initialized) initPool();
}

inline void EEPROMEmu::initPool() {
    poolBaseAddr = eepromFlashBase();
    PageHeader hdr{};
    bool found = false;
    uint32_t bestGeneration = 0;
    for (uint8_t i = 0; i < EEPROM_PAGE_POOL; ++i) {
        if (readHeader(i, hdr)) {
            if (!found || hdr.generation > bestGeneration) {
                found = true;
                bestGeneration = hdr.generation;
                activePage = i;
            }
        }
    }

    if (!found) {
        activePage = 0;
        activeGeneration = 1;
        eraseLogicalPage(activePage);
        writeHeader(activePage, activeGeneration);
    }
    else {
        activeGeneration = bestGeneration;
    }

    initialized = true;
}

inline bool EEPROMEmu::readHeader(uint8_t pageIdx, PageHeader& out) const {
    const PageHeader* hdr = reinterpret_cast<const PageHeader*>(pageBase(pageIdx));
    std::memcpy(&out, hdr, sizeof(PageHeader));
    if (out.magic != kEepromPageMagic) return false;
    if (out.generation != (~out.generationInverse)) return false;
    return true;
}

inline void EEPROMEmu::writeHeader(uint8_t pageIdx, uint32_t generation) {
    PageHeader hdr{ kEepromPageMagic, generation, ~generation, 0xFFFFFFFFu };
    programBytes(pageBase(pageIdx), reinterpret_cast<const uint8_t*>(&hdr), sizeof(PageHeader));
}

inline void EEPROMEmu::programBytes(uint32_t targetAddr, const uint8_t* src, size_t len) {
    if (!kEnableEepromWrites) {
        static uint8_t warned = 0;
        if (Serial && warned < 4) {
            Serial.print(F("[EEPROM] write skipped at 0x"));
            Serial.print(targetAddr, HEX);
            Serial.print(F(" len="));
            Serial.println(len);
            warned++;
        }
        return;
    }

    uint32_t priorConfig = NRF_NVMC->CONFIG;
    bool switchedMode = (priorConfig != NVMC_CONFIG_WEN_Wen);
    if (switchedMode) {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (!NRF_NVMC->READY);
    }

    size_t idx = 0;
    while (idx < len) {
        uint32_t addr = targetAddr + idx;
        uint32_t wordAddr = addr & ~0x3UL;
        uint32_t existingWord = *((uint32_t*)wordAddr);
        uint32_t newWord = existingWord;

        size_t byteOffset = addr & 0x3UL;
        size_t spaceInWord = 4 - byteOffset;
        size_t remaining = len - idx;
        size_t chunk = (spaceInWord < remaining) ? spaceInWord : remaining;

        for (size_t b = 0; b < chunk; ++b) {
            size_t byteIndex = byteOffset + b;
            uint8_t desired = src[idx + b];
            uint8_t existing = (existingWord >> (8 * byteIndex)) & 0xFF;
            uint8_t programmed = existing & desired; // only clear bits
            uint32_t mask = 0xFFUL << (8 * byteIndex);
            newWord = (newWord & ~mask) | (static_cast<uint32_t>(programmed) << (8 * byteIndex));
        }

        if (newWord != existingWord) {
            *((volatile uint32_t*)wordAddr) = newWord;
            while (!NRF_NVMC->READY);
        }

        idx += chunk;
    }

    if (switchedMode) {
        NRF_NVMC->CONFIG = priorConfig;
        while (!NRF_NVMC->READY);
    }
}

inline void EEPROMEmu::eraseLogicalPage(uint8_t pageIdx) {
    if (!kEnableEepromWrites) return;
    uint32_t base = pageBase(pageIdx);
    uint32_t hardwarePageSize = NRF_FICR->CODEPAGESIZE;
    uint32_t numPages = FLASH_PAGE_SIZE / hardwarePageSize;

    uint32_t priorConfig = NRF_NVMC->CONFIG;
    bool switchedMode = (priorConfig != NVMC_CONFIG_WEN_Een);
    if (switchedMode) {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
        while (!NRF_NVMC->READY);
    }

    for (uint32_t i = 0; i < numPages; ++i) {
        uint32_t pageAddr = base + i * hardwarePageSize;
        NRF_NVMC->ERASEPAGE = pageAddr;
        while (!NRF_NVMC->READY);
    }

    if (switchedMode) {
        NRF_NVMC->CONFIG = priorConfig;
        while (!NRF_NVMC->READY);
    }
}

inline bool EEPROMEmu::rotateToNextPage() {
    uint8_t next = nextPageIndex(activePage);
    eraseLogicalPage(next);
    activePage = next;
    activeGeneration = (activeGeneration == 0xFFFFFFFFu) ? 1 : activeGeneration + 1;
    writeHeader(activePage, activeGeneration);
    return true;
}

inline bool EEPROMEmu::beginPageRewrite() {
    ensureInitialized();
    if (!kEnableEepromWrites) return false;
    return rotateToNextPage();
}

inline uint8_t EEPROMEmu::findNextSlot(uint8_t baseAddr, uint8_t numSlots) {
    for (uint8_t i = 0; i < numSlots; i++) {
        uint8_t val;
        get(baseAddr + i, val);
        if (val == 0xFF) return i;
    }
    return 0;
}

inline uint8_t EEPROMEmu::findCurrentSlot(uint8_t baseAddr, uint8_t numSlots) {
    for (int8_t i = numSlots - 1; i >= 0; i--) {
        uint8_t val;
        get(baseAddr + i, val);
        if (val != 0xFF) return i;
    }
    return 0;
}

inline void EEPROMEmu::putBrightness(uint8_t brightness) {
    uint8_t slot = findNextSlot(WL_BRIGHTNESS_START, WL_BRIGHTNESS_SLOTS);
    put(WL_BRIGHTNESS_START + slot, brightness);
#if ENABLE_WRITE_COUNTERS
    if (Serial) {
        Serial.print(F("[WL] Brightness→slot "));
        Serial.println(slot);
    }
#endif
}

inline uint8_t EEPROMEmu::getBrightness() {
    uint8_t slot = findCurrentSlot(WL_BRIGHTNESS_START, WL_BRIGHTNESS_SLOTS);
    uint8_t brightness;
    get(WL_BRIGHTNESS_START + slot, brightness);
    if (brightness == 0xFF) brightness = 128;
    return brightness;
}

inline void EEPROMEmu::putDarktime(uint8_t darktime) {
    uint8_t slot = findNextSlot(WL_DARKTIME_START, WL_DARKTIME_SLOTS);
    put(WL_DARKTIME_START + slot, darktime);
#if ENABLE_WRITE_COUNTERS
    if (Serial) {
        Serial.print(F("[WL] Darktime→slot "));
        Serial.println(slot);
    }
#endif
}

inline uint8_t EEPROMEmu::getDarktime() {
    uint8_t slot = findCurrentSlot(WL_DARKTIME_START, WL_DARKTIME_SLOTS);
    uint8_t darktime;
    get(WL_DARKTIME_START + slot, darktime);
    if (darktime == 0xFF) darktime = 6;
    return darktime;
}

extern EEPROMEmu EEPROM;
