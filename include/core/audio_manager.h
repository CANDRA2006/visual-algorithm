#pragma once
#ifndef VAS_AUDIO_MANAGER_H
#define VAS_AUDIO_MANAGER_H

/**
 * @file audio_manager.h
 * @brief Procedural sci-fi audio engine for VAS.
 *
 * SFML 3.0 note: sf::Int16 dihapus — semua PCM buffer menggunakan std::int16_t
 */

#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <array>
#include <memory>
#include <atomic>

namespace vas {

class AudioManager {
public:
    static AudioManager& instance() {
        static AudioManager mgr;
        return mgr;
    }

    void initialize();

    void setEnabled(bool on) { m_enabled = on; }
    bool isEnabled()  const  { return m_enabled; }

    void setVolume(float v);
    float getVolume() const { return m_volume; }

    void playCompare(float normalizedValue);
    void playSwap(float normA, float normB);
    void playSorted();
    void playNewData();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    AudioManager() = default;

    static constexpr unsigned SAMPLE_RATE = 44100;

    // std::int16_t menggantikan sf::Int16 yang dihapus di SFML 3.0
    static std::vector<std::int16_t> makeTone(
        float freqHz, float durationSec,
        float attackSec  = 0.005f,
        float decaySec   = 0.03f,
        float sustainLvl = 0.3f,
        float releaseSec = 0.05f,
        float amplitude  = 22000.0f);

    static std::vector<std::int16_t> blendTones(
        const std::vector<std::int16_t>& a,
        const std::vector<std::int16_t>& b,
        float ratioA = 0.5f);

    static std::vector<std::int16_t> makeNoise(
        float durationSec, float amplitude = 8000.0f);

    static float normToFreq(float norm);

    static constexpr size_t POOL_SIZE = 8;

    // SFML 3.0: sf::Sound has no default constructor — store separately
    struct PoolSlot {
        sf::SoundBuffer              buffer;
        std::unique_ptr<sf::Sound>   sound;   // constructed after buffer is ready

        PoolSlot() = default;
    };

    std::array<std::unique_ptr<PoolSlot>, POOL_SIZE> m_pool;
    std::atomic<size_t>  m_poolHead { 0 };

    std::unique_ptr<PoolSlot> m_sortedSlot;
    std::unique_ptr<PoolSlot> m_newDataSlot;

    void playBuffer(PoolSlot& slot, std::vector<std::int16_t>&& samples);

    std::atomic<bool>  m_enabled { true };
    float              m_volume  { 60.0f };
    std::mutex         m_mutex;

    sf::Clock          m_compareClock;
    static constexpr float MIN_COMPARE_INTERVAL = 0.012f;
};

} // namespace vas

#define VAS_AUDIO vas::AudioManager::instance()

#endif // VAS_AUDIO_MANAGER_H