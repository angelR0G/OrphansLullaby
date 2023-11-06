#pragma once

struct InputComponent {
    // Settings
    double  mouseHorizontalSensitivity{0.005},
            mouseVerticalSensitivity{0.005};
    bool    alternateRunning{false};

    // Other values
    bool    interact{false};
    bool    alternateRunningPressed{false};
};