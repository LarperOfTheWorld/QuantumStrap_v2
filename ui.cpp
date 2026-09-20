#include <imgui.h>

// Custom Checkbox Style for the Red/Grey look in the image
void Checkboxes(const char* label, bool* v) {
    ImGui::Checkbox(label, v);
}

void Button(const char* label) {
    ImGui::Button(label, ImVec2(150, 30));
}
