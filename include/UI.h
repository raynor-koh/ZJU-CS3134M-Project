#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <vector>
#include <memory>
#include "Vector3.h"

class Shape;  // Forward declaration

// Edit mode state enum
enum class EditModeState {
    INACTIVE,    // Not in edit mode
    ACTIVE,      // Edit mode activated
    SELECTING    // Selecting object
};

// 滑动条类型枚举
enum class SliderType {
    NONE,
    COLOR_R,
    COLOR_G,
    COLOR_B
};

class UI {
public:
    UI(int windowWidth, int windowHeight);

    void setWindowSize(int width, int height);

    // 设置准星在屏幕空间中的位置（像素坐标）
    void setCrossPosition(float x, float y);

    // 绘制准星（在 display 中 3D 场景绘制完之后调用）
    void drawCross() const;

    // Edit mode related methods
    void toggleEditMode();
    void setEditMode(EditModeState state);
    EditModeState getEditMode() const { return editModeState; }
    void drawEditorUI() const;
    void handleEditorClick(int button, int state, int x, int y);
    void handleMouseMotion(int x, int y);
    void updateObjectList(const std::vector<std::shared_ptr<Shape>>& objects);
    void selectObject(int index);
    void applyMaterialChanges();
    void applyTextureChanges();
    bool isDraggingSlider() const { return isDraggingSliderB; }
    
private:
    int windowWidth;
    int windowHeight;
    float crossX;
    float crossY;
    float crossHalfSize;
    
    // Edit mode related
    EditModeState editModeState = EditModeState::INACTIVE;
    std::vector<std::shared_ptr<Shape>> sceneObjects;
    int selectedObjectIndex = -1;
    Color currentColor;
    std::string currentTextureName;
    
    // 滑动条相关
    SliderType activeSlider = SliderType::NONE;
    float sliderValue = 0.0f;
    float sliderMin = 0.0f;
    float sliderMax = 1.0f;
    float sliderX = 0.0f;
    float sliderY = 0.0f;
    float sliderWidth = 150.0f;
    float sliderHeight = 20.0f;
    bool isDraggingSliderB = false;

    // UI drawing helper methods
    void drawText(const std::string& text, float x, float y, float scale = 1.0f) const;
    void drawRect(float x, float y, float width, float height, float r, float g, float b, float a = 1.0f) const;
    void drawButton(const std::string& text, float x, float y, float width, float height) const;
    void drawSlider(float x, float y, float width, float height, float value, float min, float max, const std::string& label) const;
    void handleSliderClick(int button, int state, int x, int y);
    void updateSliderValue(float x);
    void updateMaterialFromSlider();
};