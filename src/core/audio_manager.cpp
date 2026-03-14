#include "core/audio_manager.h"
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace vas {

void AudioManager::initialize()
{
    for (size_t i = 0; i < POOL_SIZE; ++i)
        m_pool[i] = std::make_unique<PoolSlot>();
    m_sortedSlot  = std::make_unique<PoolSlot>();
    m_newDataSlot = std::make_unique<PoolSlot>();
}

void AudioManager::setVolume(float v)
{
    m_volume = std::clamp(v, 0.0f, 100.0f);
}

float AudioManager::normToFreq(float norm)
{
    norm = std::clamp(norm, 0.0f, 1.0f);
    const float minHz    = 220.0f;
    const float maxHz    = 1760.0f;
    float hz = minHz * std::pow(maxHz / minHz, norm);
    const float semitone = std::pow(2.0f, 1.0f / 12.0f);
    float semitones = std::log(hz / minHz) / std::log(semitone);
    static const int penta[] = { 0, 2, 4, 7, 9 };
    int oct     = static_cast<int>(semitones) / 12;
    int remain  = static_cast<int>(semitones) % 12;
    int best    = penta[0];
    int bestDist = std::abs(remain - penta[0]);
    for (int p : penta) {
        if (std::abs(remain - p) < bestDist) {
            best = p;
            bestDist = std::abs(remain - p);
        }
    }
    return minHz * std::pow(semitone, static_cast<float>(oct * 12 + best));
}

// SFML 3.0: sf::Int16 dihapus → std::int16_t
std::vector<std::int16_t> AudioManager::makeTone(
    float freqHz, float durationSec,
    float attackSec, float decaySec, float sustainLvl, float releaseSec,
    float amplitude)
{
    const size_t totalSamples   = static_cast<size_t>(SAMPLE_RATE * durationSec);
    std::vector<std::int16_t> buf(totalSamples);
    const size_t attackSamples  = static_cast<size_t>(SAMPLE_RATE * attackSec);
    const size_t decaySamples   = static_cast<size_t>(SAMPLE_RATE * decaySec);
    const size_t releaseSamples = static_cast<size_t>(SAMPLE_RATE * releaseSec);

    for (size_t i = 0; i < totalSamples; ++i) {
        float env = 1.0f;
        if (i < attackSamples) {
            env = static_cast<float>(i) / static_cast<float>(attackSamples + 1);
        } else if (i < attackSamples + decaySamples) {
            float t = static_cast<float>(i - attackSamples) / static_cast<float>(decaySamples + 1);
            env = 1.0f - t * (1.0f - sustainLvl);
        } else if (i >= totalSamples - releaseSamples) {
            float t = static_cast<float>(i - (totalSamples - releaseSamples))
                      / static_cast<float>(releaseSamples + 1);
            env = sustainLvl * (1.0f - t);
        } else {
            env = sustainLvl;
        }
        float t    = static_cast<float>(i) / static_cast<float>(SAMPLE_RATE);
        float wave = std::sin(2.0f * 3.14159265f * freqHz * t);
        wave += 0.25f * std::sin(4.0f * 3.14159265f * freqHz * t);
        wave /= 1.25f;
        buf[i] = static_cast<std::int16_t>(env * amplitude * wave);
    }
    return buf;
}

std::vector<std::int16_t> AudioManager::blendTones(
    const std::vector<std::int16_t>& a,
    const std::vector<std::int16_t>& b,
    float ratioA)
{
    const size_t len = std::max(a.size(), b.size());
    std::vector<std::int16_t> out(len, 0);
    for (size_t i = 0; i < len; ++i) {
        float va = (i < a.size()) ? static_cast<float>(a[i]) : 0.0f;
        float vb = (i < b.size()) ? static_cast<float>(b[i]) : 0.0f;
        out[i] = static_cast<std::int16_t>(va * ratioA + vb * (1.0f - ratioA));
    }
    return out;
}

std::vector<std::int16_t> AudioManager::makeNoise(float durationSec, float amplitude)
{
    const size_t n = static_cast<size_t>(SAMPLE_RATE * durationSec);
    std::vector<std::int16_t> buf(n);
    const size_t fade = n / 8;
    for (size_t i = 0; i < n; ++i) {
        float env = 1.0f;
        if (i < fade)          env = static_cast<float>(i) / static_cast<float>(fade);
        else if (i > n - fade) env = static_cast<float>(n - i) / static_cast<float>(fade);
        float noise = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
        buf[i] = static_cast<std::int16_t>(env * amplitude * noise);
    }
    return buf;
}

// SFML 3.0 loadFromSamples signature (5 argumen — channelMap wajib):
// bool loadFromSamples(const std::int16_t*              samples,
//                      std::uint64_t                    sampleCount,
//                      std::uint32_t                    channelCount,
//                      unsigned int                     sampleRate,
//                      const std::vector<SoundChannel>& channelMap);
void AudioManager::playBuffer(PoolSlot& slot, std::vector<std::int16_t>&& samples)
{
    if (samples.empty()) return;

    // SFML 3.0: channelMap wajib diisi
    const std::vector<sf::SoundChannel> channelMap = { sf::SoundChannel::Mono };

    if (!slot.buffer.loadFromSamples(
            samples.data(),
            samples.size(),
            channelMap.size(),
            SAMPLE_RATE,
            channelMap))
        return;

    // SFML 3.0: sf::Sound harus dikonstruksi dengan buffer — tidak ada default ctor
    slot.sound = std::make_unique<sf::Sound>(slot.buffer);
    slot.sound->setVolume(m_volume);
    slot.sound->play();
}

void AudioManager::playCompare(float normalizedValue)
{
    if (!m_enabled) return;
    if (m_compareClock.getElapsedTime().asSeconds() < MIN_COMPARE_INTERVAL) return;
    m_compareClock.restart();
    size_t idx = m_poolHead.fetch_add(1) % POOL_SIZE;
    auto& slot = *m_pool[idx];
    float freq = normToFreq(normalizedValue);
    auto samples = makeTone(freq, 0.08f, 0.003f, 0.02f, 0.2f, 0.04f, 18000.0f);
    playBuffer(slot, std::move(samples));
}

void AudioManager::playSwap(float normA, float normB)
{
    if (!m_enabled) return;
    size_t idx = m_poolHead.fetch_add(1) % POOL_SIZE;
    auto& slot = *m_pool[idx];
    float freqA = normToFreq(normA);
    float freqB = normToFreq(normB);
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * 0.12f);
    std::vector<std::int16_t> buf(totalSamples);
    const size_t fade = totalSamples / 6;
    for (size_t i = 0; i < totalSamples; ++i) {
        float t     = static_cast<float>(i) / static_cast<float>(totalSamples);
        float freq  = freqA + (freqB - freqA) * t;
        float phase = 2.0f * 3.14159265f * freq * (static_cast<float>(i) / SAMPLE_RATE);
        float env   = 1.0f;
        if (i < fade)                     env = static_cast<float>(i) / static_cast<float>(fade);
        else if (i > totalSamples - fade)  env = static_cast<float>(totalSamples - i) / static_cast<float>(fade);
        float wave = std::sin(phase) + 0.3f * std::sin(2.0f * phase);
        wave /= 1.3f;
        buf[i] = static_cast<std::int16_t>(env * 22000.0f * wave);
    }
    playBuffer(slot, std::move(buf));
}

void AudioManager::playSorted()
{
    if (!m_enabled) return;
    const float notes[]       = { 220.0f, 277.18f, 329.63f, 440.0f, 659.25f };
    const float noteDur       = 0.10f;
    const float totalDur      = noteDur * 5 + 0.15f;
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * totalDur);
    std::vector<std::int16_t> buf(totalSamples, 0);
    for (size_t n = 0; n < 5; ++n) {
        size_t offset = static_cast<size_t>(n * noteDur * SAMPLE_RATE);
        auto tone = makeTone(notes[n], noteDur + 0.12f, 0.005f, 0.04f, 0.5f, 0.12f, 20000.0f);
        for (size_t i = 0; i < tone.size() && offset + i < totalSamples; ++i) {
            float v = static_cast<float>(buf[offset + i]) + static_cast<float>(tone[i]);
            buf[offset + i] = static_cast<std::int16_t>(std::clamp(v, -32768.0f, 32767.0f));
        }
    }
    playBuffer(*m_sortedSlot, std::move(buf));
}

void AudioManager::playNewData()
{
    if (!m_enabled) return;
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * 0.18f);
    std::vector<std::int16_t> buf(totalSamples, 0);
    const size_t fade = totalSamples / 8;
    for (size_t i = 0; i < totalSamples; ++i) {
        float t     = static_cast<float>(i) / static_cast<float>(totalSamples);
        float freq  = 1200.0f - 900.0f * t;
        float phase = 2.0f * 3.14159265f * freq * (static_cast<float>(i) / SAMPLE_RATE);
        float env   = 1.0f;
        if (i < fade)                     env = static_cast<float>(i) / static_cast<float>(fade);
        else if (i > totalSamples - fade)  env = static_cast<float>(totalSamples - i) / static_cast<float>(fade);
        float wave = std::sin(phase)
                   + 0.15f * ((static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f);
        buf[i] = static_cast<std::int16_t>(env * 20000.0f * wave);
    }
    playBuffer(*m_newDataSlot, std::move(buf));
}

} // namespace vas