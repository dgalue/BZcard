#include "BrickBreaker.h"
#include <Arduino.h>

BrickBreaker::BrickBreaker(DisplayManager& display, InputManager& input, StorageManager& storage)
    : displayManager(display), inputManager(input), storageManager(storage) {}

void BrickBreaker::play() {
    displayManager.u8g2.setPowerSave(0);
    // Game variables
    bool gameRunning = true;
    unsigned long lastMoveTime = 0;
    const unsigned long moveDelay = 25;
    int paddleWidth = 20;
    int paddleHeight = 3;
    int paddleY = 60;
    int paddleX = 54;
    float ballX = 64.0;
    float ballY = 40.0;
    int ballSize = 2;
    float ballDX = 1.8;
    float ballDY = -1.8;
    float maxBallSpeed = 3.5;
    const int brickRows = 3;
    const int brickCols = 5;
    const int brickWidth = 20;
    const int brickHeight = 6;
    const int brickSpacing = 4;
    const int topMargin = 10;
    bool bricks[brickRows][brickCols];
    for (int row = 0; row < brickRows; row++)
        for (int col = 0; col < brickCols; col++)
            bricks[row][col] = true;
    int score = 0;
    int level = 1;
    int highScore = storageManager.getHighScore();
    // Show start screen
    displayManager.u8g2.firstPage();
    do {
        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
        int titleWidth = displayManager.u8g2.getStrWidth("BRICK BREAKER");
        displayManager.u8g2.drawStr((128 - titleWidth) / 2, 20, "BRICK BREAKER");
        displayManager.u8g2.setFont(u8g2_font_6x12_tr);  // Smaller font for instructions
        displayManager.u8g2.drawStr(15, 35, "Press UP to start");
        char highText[20];
        sprintf(highText, "High: %d", highScore);
        int highWidth = displayManager.u8g2.getStrWidth(highText);
        displayManager.u8g2.drawStr((128 - highWidth) / 2, 50, highText);
    } while (displayManager.u8g2.nextPage());
    // Wait for button press to start
    while (true) {
        inputManager.update();
        if (inputManager.isUpPressed()) break;
        delay(10);
    }
    while (inputManager.isUpPressed()) {
        inputManager.update();
        delay(10);
    }
    // Main game loop
    while (gameRunning) {
        inputManager.update();
        if (inputManager.isUpPressed()) {
            paddleX -= 6;
            if (paddleX < 0) paddleX = 0;
        }
        if (inputManager.isDownPressed()) {
            paddleX += 6;
            if (paddleX > 128 - paddleWidth) paddleX = 128 - paddleWidth;
        }
        if (inputManager.isPowerPressed()) {
            gameRunning = false;
        }
        if (millis() - lastMoveTime > moveDelay) {
            lastMoveTime = millis();
            ballX += ballDX;
            ballY += ballDY;
            if (ballX <= 0 || ballX >= 128 - ballSize) {
                ballDX = -ballDX;
                ballDY += (random(-10, 10) / 100.0);
            }
            if (ballY <= 0) {
                ballDY = -ballDY;
                ballDX += (random(-10, 10) / 100.0);
            }
            if (ballY >= paddleY - ballSize && ballY <= paddleY + paddleHeight) {
                if (ballX >= paddleX - ballSize && ballX <= paddleX + paddleWidth) {
                    float hitPosition = (ballX - paddleX) / paddleWidth;
                    float angle = (hitPosition - 0.5) * PI;
                    ballDX = sin(angle) * (maxBallSpeed * 0.8);
                    ballDY = -cos(angle) * maxBallSpeed;
                    float currentSpeed = sqrt(ballDX * ballDX + ballDY * ballDY);
                    if (currentSpeed < maxBallSpeed) {
                        float speedFactor = 1.05;
                        ballDX *= speedFactor;
                        ballDY *= speedFactor;
                    }
                    ballY = paddleY - ballSize - 1;
                }
            }
            for (int row = 0; row < brickRows; row++) {
                for (int col = 0; col < brickCols; col++) {
                    if (bricks[row][col]) {
                        int brickX = col * (brickWidth + brickSpacing) + brickSpacing;
                        int brickY = row * (brickHeight + brickSpacing) + topMargin;
                        if (ballX >= brickX - ballSize && ballX <= brickX + brickWidth &&
                            ballY >= brickY - ballSize && ballY <= brickY + brickHeight) {
                            bricks[row][col] = false;
                            float overlapLeft = ballX - (brickX - ballSize);
                            float overlapRight = (brickX + brickWidth) - ballX;
                            float overlapTop = ballY - (brickY - ballSize);
                            float overlapBottom = (brickY + brickHeight) - ballY;
                            float minOverlap = min(min(overlapLeft, overlapRight), min(overlapTop, overlapBottom));
                            if (minOverlap == overlapLeft || minOverlap == overlapRight) {
                                ballDX = -ballDX;
                            } else {
                                ballDY = -ballDY;
                            }
                            ballDX += (random(-5, 5) / 100.0);
                            ballDY += (random(-5, 5) / 100.0);
                            score += 10 * level;
                            bool allDestroyed = true;
                            for (int r = 0; r < brickRows; r++)
                                for (int c = 0; c < brickCols; c++)
                                    if (bricks[r][c]) allDestroyed = false;
                            if (allDestroyed) {
                                for (int r = 0; r < brickRows; r++)
                                    for (int c = 0; c < brickCols; c++)
                                        bricks[r][c] = true;
                                ballX = 64.0;
                                ballY = 40.0;
                                level++;
                                maxBallSpeed += 0.5;
                                score += 50 * level;
                                displayManager.u8g2.firstPage();
                                do {
                                    displayManager.u8g2.setFont(u8g2_font_ncenB10_tr);
                                    displayManager.u8g2.setCursor(25, 30);
                                    displayManager.u8g2.print("LEVEL ");
                                    displayManager.u8g2.print(level);
                                    displayManager.u8g2.setFont(u8g2_font_ncenB08_tr);
                                    displayManager.u8g2.setCursor(25, 45);
                                    displayManager.u8g2.print("Score: ");
                                    displayManager.u8g2.print(score);
                                } while (displayManager.u8g2.nextPage());
                                delay(1500);
                            }
                            goto endBrickCollision;
                        }
                    }
                }
            }
            endBrickCollision:
            if (ballY >= 64) {
                if (score > highScore) {
                    highScore = score;
                    storageManager.saveHighScore(highScore);
                    displayManager.u8g2.firstPage();
                    do {
                        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
                        int newHighWidth = displayManager.u8g2.getStrWidth("NEW HIGH!");
                        displayManager.u8g2.drawStr((128 - newHighWidth) / 2, 25, "NEW HIGH!");
                        char scoreText[20];
                        sprintf(scoreText, "Score: %d", score);
                        int scoreWidth = displayManager.u8g2.getStrWidth(scoreText);
                        displayManager.u8g2.drawStr((128 - scoreWidth) / 2, 40, scoreText);
                    } while (displayManager.u8g2.nextPage());
                    delay(2000);
                }
                displayManager.u8g2.firstPage();
                do {
                    displayManager.u8g2.setFont(u8g2_font_profont17_tr);
                    int titleWidth = displayManager.u8g2.getStrWidth("GAME OVER");
                    displayManager.u8g2.drawStr((128 - titleWidth) / 2, 20, "GAME OVER");
                    char scoreText[20];
                    sprintf(scoreText, "Score: %d", score);
                    int scoreWidth = displayManager.u8g2.getStrWidth(scoreText);
                    displayManager.u8g2.drawStr((128 - scoreWidth) / 2, 35, scoreText);
                    char highText[20];
                    sprintf(highText, "High: %d", highScore);
                    int highWidth = displayManager.u8g2.getStrWidth(highText);
                    displayManager.u8g2.drawStr((128 - highWidth) / 2, 50, highText);
                } while (displayManager.u8g2.nextPage());
                delay(2000);
                gameRunning = false;
            }
            float currentSpeed = sqrt(ballDX * ballDX + ballDY * ballDY);
            if (currentSpeed > maxBallSpeed) {
                ballDX = (ballDX / currentSpeed) * maxBallSpeed;
                ballDY = (ballDY / currentSpeed) * maxBallSpeed;
            }
            if (abs(ballDY) < 0.2) {
                if (ballDY > 0) ballDY = 0.2;
                else ballDY = -0.2;
            }
        }
        displayManager.u8g2.firstPage();
        do {
            for (int row = 0; row < brickRows; row++)
                for (int col = 0; col < brickCols; col++)
                    if (bricks[row][col]) {
                        int brickX = col * (brickWidth + brickSpacing) + brickSpacing;
                        int brickY = row * (brickHeight + brickSpacing) + topMargin;
                        displayManager.u8g2.drawFrame(brickX, brickY, brickWidth, brickHeight);
                    }
            displayManager.u8g2.drawBox(paddleX, paddleY, paddleWidth, paddleHeight);
            displayManager.u8g2.drawDisc(int(ballX), int(ballY), ballSize);
            displayManager.u8g2.setFont(u8g2_font_ncenB08_tr);
            displayManager.u8g2.setCursor(2, 8);
            displayManager.u8g2.print("Score:");
            displayManager.u8g2.print(score);
            displayManager.u8g2.setCursor(100, 8);
            displayManager.u8g2.print("L:");
            displayManager.u8g2.print(level);
        } while (displayManager.u8g2.nextPage());
        delay(10);
    }
} 