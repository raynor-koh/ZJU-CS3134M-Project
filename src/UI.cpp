#include "ui.h"
#include "Shapes.h"  // Include Shape class definition
#include <sstream>
#include <iostream>

UI::UI(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      crossX(windowWidth / 2.0f), crossY(windowHeight / 2.0f), crossHalfSize(10.0f) {}

void UI::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void UI::setCrossPosition(float x, float y) {
    crossX = x;
    crossY = y;
}

void UI::drawCross() const {
    if (editModeState != EditModeState::INACTIVE) return; // Don't show crosshair in edit mode

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // Horizontal line
    glVertex2f(crossX - crossHalfSize, crossY);
    glVertex2f(crossX + crossHalfSize, crossY);
    // Vertical line
    glVertex2f(crossX, crossY - crossHalfSize);
    glVertex2f(crossX, crossY + crossHalfSize);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void UI::toggleEditMode() {
    if (editModeState == EditModeState::INACTIVE) {
        editModeState = EditModeState::ACTIVE;
        std::cout << "Edit mode activated" << std::endl;
    } else {
        editModeState = EditModeState::INACTIVE;
        std::cout << "Edit mode deactivated" << std::endl;
    }
}

void UI::setEditMode(EditModeState state) {
    editModeState = state;
}

void UI::drawEditorUI() const {
    if (editModeState == EditModeState::INACTIVE) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Draw editor background
    glColor4f(0.1f, 0.1f, 0.1f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth * 0.5f, 0);
    glVertex2f(windowWidth * 0.5f, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Draw title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Edit Mode", 20, 30, 1.5f);

    // Draw object list box
    drawRect(20, 60, 300, 400, 0.2f, 0.2f, 0.2f, 0.7f);

    // Draw object list
    glColor3f(1.0f, 1.0f, 1.0f);
    float y_pos = 80;
    for (size_t i = 0; i < sceneObjects.size(); ++i) {
        std::string objName = "Object " + std::to_string(i) + ": ";
        
        if (auto cube = std::dynamic_pointer_cast<Cube>(sceneObjects[i])) {
            objName += "Cube";
        } else if (auto sphere = std::dynamic_pointer_cast<Sphere>(sceneObjects[i])) {
            objName += "Sphere";
        } else if (auto cylinder = std::dynamic_pointer_cast<Cylinder>(sceneObjects[i])) {
            objName += "Cylinder";
        } else if (auto cone = std::dynamic_pointer_cast<Cone>(sceneObjects[i])) {
            objName += "Cone";
        } else if (auto prism = std::dynamic_pointer_cast<Prism>(sceneObjects[i])) {
            objName += "Prism";
        } else if (auto frustum = std::dynamic_pointer_cast<Frustum>(sceneObjects[i])) {
            objName += "Frustum";
        } else {
            objName += "Unknown";
        }

        // Highlight selected object
        if (static_cast<int>(i) == selectedObjectIndex) {
            glColor3f(0.0f, 1.0f, 1.0f); // Cyan when selected
        } else {
            glColor3f(1.0f, 1.0f, 1.0f); // Default white
        }

        drawText(objName, 40, y_pos);
        y_pos += 25;
    }

    // If an object is selected, show details
    if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(sceneObjects.size())) {
        auto& obj = sceneObjects[selectedObjectIndex];
        
        // Draw details box
        drawRect(340, 60, 400, 300, 0.2f, 0.2f, 0.2f, 0.7f);
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("Object Details", 360, 80, 1.2f);
        
        // Show object type and position
        std::string typeText = "Type: ";
        if (std::dynamic_pointer_cast<Cube>(obj)) {
            typeText += "Cube";
        } else if (std::dynamic_pointer_cast<Sphere>(obj)) {
            typeText += "Sphere";
        } else if (std::dynamic_pointer_cast<Cylinder>(obj)) {
            typeText += "Cylinder";
        } else if (std::dynamic_pointer_cast<Cone>(obj)) {
            typeText += "Cone";
        } else if (std::dynamic_pointer_cast<Prism>(obj)) {
            typeText += "Prism";
        } else if (std::dynamic_pointer_cast<Frustum>(obj)) {
            typeText += "Frustum";
        } else {
            typeText += "Unknown";
        }
        
        drawText(typeText, 360, 110);
        
        Vector3 pos = obj->getPosition();
        std::string posText = "Position: (" + 
                             std::to_string(pos.x).substr(0, 4) + ", " +
                             std::to_string(pos.y).substr(0, 4) + ", " +
                             std::to_string(pos.z).substr(0, 4) + ")";
        drawText(posText, 360, 135);
        
        Vector3 size = obj->getSize();
        std::string sizeText = "Size: (" + 
                              std::to_string(size.x).substr(0, 4) + ", " +
                              std::to_string(size.y).substr(0, 4) + ", " +
                              std::to_string(size.z).substr(0, 4) + ")";
        drawText(sizeText, 360, 160);
        
        // Texture name display
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("Texture: ", 360, 185);
        std::string textureText = currentTextureName.empty() ? "No texture" : currentTextureName;
        drawText(textureText, 420, 185);
        
        // Material editing area
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("Material Editing", 360, 210, 1.2f);
        
        // Material parameters with clickable labels
        float materialY = 240;
        drawText("R:", 360, materialY);
        std::string rText = std::to_string(currentColor.r).substr(0, 4);
        drawText(rText, 450, materialY);
        
        drawText("G:", 360, materialY + 25);
        std::string gText = std::to_string(currentColor.g).substr(0, 4);
        drawText(gText, 450, materialY + 25);
        
        drawText("B:", 360, materialY + 50);
        std::string bText = std::to_string(currentColor.b).substr(0, 4);
        drawText(bText, 450, materialY + 50);
    }

    // Draw active slider if one is selected
    if (activeSlider != SliderType::NONE) {
        drawSlider(sliderX, sliderY, sliderWidth, sliderHeight, sliderValue, sliderMin, sliderMax, "Value");
    }

    // Draw buttons
    drawButton("Apply Material", 340, 380, 120, 30);
    drawButton("Apply Texture", 470, 380, 120, 30);
    drawButton("Exit Edit", 600, 380, 120, 30);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void UI::handleEditorClick(int button, int state, int x, int y) {
    if (editModeState == EditModeState::INACTIVE) return;

    // 鼠标按下事件
    if (state == GLUT_DOWN) {
        // Check if clicked on object list area
        if (x >= 20 && x <= 320 && y >= 60 && y <= 460) {
            // Calculate which object was clicked
            int objIndex = (y - 65) / 25;
            if (objIndex >= 0 && objIndex < static_cast<int>(sceneObjects.size())) {
                selectObject(objIndex);
            }
        }
        
        // Check if clicked on material parameter labels (for slider activation)
        else if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(sceneObjects.size())) {
            float materialY = 225;
            
            // Check for diffuse R
            if (x >= 360 && x <= 479 && y >= materialY && y <= materialY + 15) {
                activeSlider = SliderType::COLOR_R;
                sliderValue = currentColor.r;
                sliderMin = 0.0f;
                sliderMax = 1.0f;
                sliderX = 480;
                sliderY = materialY - 5;
                isDraggingSliderB = true;  // 鼠标按下时开始拖动
            }
            // Check for diffuse G
            else if (x >= 360 && x <= 479 && y >= materialY + 25 && y <= materialY + 40) {
                activeSlider = SliderType::COLOR_G;
                sliderValue = currentColor.g;
                sliderMin = 0.0f;
                sliderMax = 1.0f;
                sliderX = 480;
                sliderY = materialY + 20;
                isDraggingSliderB = true;  // 鼠标按下时开始拖动
            }
            // Check for diffuse B
            else if (x >= 360 && x <= 479 && y >= materialY + 50 && y <= materialY + 65) {
                activeSlider = SliderType::COLOR_B;
                sliderValue = currentColor.b;
                sliderMin = 0.0f;
                sliderMax = 1.0f;
                sliderX = 480;
                sliderY = materialY + 45;
                isDraggingSliderB = true;  // 鼠标按下时开始拖动
            }
            else {
                // Check if clicked on slider
                handleSliderClick(button, state, x, y);
            }
        }
        
        // Check button clicks
        if (x >= 340 && x <= 460 && y >= 380 && y <= 410) { // Apply Material
            applyMaterialChanges();
        } else if (x >= 470 && x <= 590 && y >= 380 && y <= 410) { // Apply Texture
            applyTextureChanges();
        } else if (x >= 600 && x <= 720 && y >= 380 && y <= 410) { // Exit Edit
            toggleEditMode();
        }
    }
    // 鼠标松开事件
    else if (state == GLUT_UP) {
        if (isDraggingSliderB) {
            isDraggingSliderB = false;  // 鼠标松开时停止拖动
        }
    }
}

void UI::handleMouseMotion(int x, int y) {
    // 在拖动滑动条时更新值
    if (isDraggingSliderB && activeSlider != SliderType::NONE) {
        updateSliderValue(x);
        updateMaterialFromSlider();
    }
}

void UI::updateObjectList(const std::vector<std::shared_ptr<Shape>>& objects) {
    sceneObjects = objects;
}

void UI::selectObject(int index) {
    if (index >= 0 && index < static_cast<int>(sceneObjects.size())) {
        selectedObjectIndex = index;
        
        // Get current object's material info
        auto obj = sceneObjects[index];
        currentColor = obj->getColor();
        
        // 获取当前对象的纹理名称
        currentTextureName = obj->getTextureName(Shape::BOTH);
        
        std::cout << "Selected object: " << index << std::endl;
        
        // Reset slider
        activeSlider = SliderType::NONE;
        isDraggingSliderB = false;
    }
}

void UI::applyMaterialChanges() {
    if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(sceneObjects.size())) {
        auto obj = sceneObjects[selectedObjectIndex];
        obj->setColor(currentColor);
        std::cout << "Applied material to object " << selectedObjectIndex << std::endl;
    }
}

void UI::applyTextureChanges() {
    if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(sceneObjects.size())) {
        std::cout << "Applied texture to object " << selectedObjectIndex << std::endl;
        // Here you would implement texture application based on your actual texture system
    }
}

void UI::drawText(const std::string& text, float x, float y, float scale) const {
    glRasterPos2f(x, y);
    
    // Apply scaling
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);
    glTranslatef(-x, -y, 0);
    
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glPopMatrix();
}

void UI::drawRect(float x, float y, float width, float height, float r, float g, float b, float a) const {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void UI::drawButton(const std::string& text, float x, float y, float width, float height) const {
    // Button background
    glColor4f(0.3f, 0.3f, 0.5f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Button border
    glColor3f(0.7f, 0.7f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    glLineWidth(1.0f);

    // Button text
    glColor3f(1.0f, 1.0f, 1.0f);
    float textX = x + (width - text.length() * 8) / 2;  // Estimate text width
    float textY = y + height / 2 + 5;  // Center vertically
    drawText(text, textX, textY);
}

void UI::drawSlider(float x, float y, float width, float height, float value, float min, float max, const std::string& label) const {
    // Draw slider track
    glColor4f(0.4f, 0.4f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Calculate handle position
    float normalizedValue = (value - min) / (max - min);
    float handleX = x + normalizedValue * width;
    float handleY = y + height / 2.0f;
    float handleSize = 8.0f;

    // Draw handle
    glColor4f(0.8f, 0.8f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(handleX - handleSize/2, handleY - handleSize/2);
    glVertex2f(handleX + handleSize/2, handleY - handleSize/2);
    glVertex2f(handleX + handleSize/2, handleY + handleSize/2);
    glVertex2f(handleX - handleSize/2, handleY + handleSize/2);
    glEnd();

    // Draw label and current value
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(label + ": " + std::to_string(value).substr(0, 4), x, y - 10);
}

void UI::handleSliderClick(int button, int state, int x, int y) {
    if (activeSlider == SliderType::NONE) return;

    if (state == GLUT_DOWN) {
        float handleX = sliderX + ((sliderValue - sliderMin) / (sliderMax - sliderMin)) * sliderWidth;
        float handleY = sliderY + sliderHeight / 2.0f;
        float handleSize = 8.0f;

        // Check if clicked on handle
        if (x >= handleX - handleSize && x <= handleX + handleSize &&
            y >= handleY - handleSize && y <= handleY + handleSize) {
            isDraggingSliderB = true; // 鼠标按下时开始拖动
        }
        // Check if clicked on slider bar (set value directly)
        else if (x >= sliderX && x <= sliderX + sliderWidth &&
                 y >= sliderY && y <= sliderY + sliderHeight) {
            updateSliderValue(x);
            updateMaterialFromSlider();
            isDraggingSliderB = true; // 鼠标按下时开始拖动
        }
    }
    else if (state == GLUT_UP) {
        if (isDraggingSliderB) {
            isDraggingSliderB = false; // 鼠标松开时停止拖动
        }
    }
}

void UI::updateSliderValue(float x) {
    float normalizedX = (x - sliderX) / sliderWidth;
    normalizedX = max(0.0f, min(1.0f, normalizedX)); // Clamp between 0 and 1
    sliderValue = sliderMin + normalizedX * (sliderMax - sliderMin);
}

void UI::updateMaterialFromSlider() {
    switch (activeSlider) {
        case SliderType::COLOR_R:
            currentColor.r = sliderValue;
            break;
        case SliderType::COLOR_G:
            currentColor.g = sliderValue;
            break;
        case SliderType::COLOR_B:
            currentColor.b = sliderValue;
            break;
        default:
            break;
    }
}