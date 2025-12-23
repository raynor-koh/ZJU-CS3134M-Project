#pragma once

// Game state enum for managing gameplay flow
enum class GameState {
    PLAYING,    // Normal gameplay
    PAUSED,     // Free camera mode or menu
    GAME_OVER   // Player died
};
