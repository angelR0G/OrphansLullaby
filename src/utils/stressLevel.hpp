#pragma once

struct StressLevel {
    [[nodiscard]] constexpr float getStress() const noexcept {
        return peakValue;
    }

    void addStress(float s) noexcept {
        if(s > 0) {
            lastAdd         = std::max(lastAdd, s);
            currentValue    += s;
        }
        else if(lastAdd > 0) {
            lastAdd         += s;
        }
        else {
            currentValue    += s;
        }

        // Clamp value
        if      (currentValue > maxValue)   currentValue = maxValue;
        else if (currentValue < 0)          currentValue = 0;

        // Update peak value
        if      (peakValue < currentValue)          peakValue = currentValue;
        else if (peakValue > currentValue + offset) peakValue = currentValue + offset;
    }

    void reset() noexcept {
        currentValue = peakValue = lastAdd = 0.f;
    }

    private:
    float   currentValue    {},
            peakValue       {},
            lastAdd         {};
    const float maxValue    {20.f},
                offset      {4.f};
};