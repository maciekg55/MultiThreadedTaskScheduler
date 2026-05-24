#include "views/PresetSelectorView.h"

PresetSelectorView::PresetSelectorView(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font) {}

void PresetSelectorView::setPresets(const std::vector<std::string>& presets) {
    _presets = presets;
    _selectedIndex = 0;
}

std::string PresetSelectorView::consumeSelection() {
    _pendingSelection = false;
    return _pendingName;
}

void PresetSelectorView::handleClick(sf::Vector2f mouse) {
    if (_selectorRect.contains(mouse)) {
        _dropdownOpen = !_dropdownOpen;
        return;
    }

    if (_dropdownOpen) {
        for (int i = 0; i < (int)_dropdownRects.size(); i++) {
            if (_dropdownRects[i].contains(mouse)) {
                _selectedIndex = i;
                _pendingName = _presets[i];
                _pendingSelection = true;
                _dropdownOpen = false;
                return;
            }
        }
        _dropdownOpen = false;
    }
}

void PresetSelectorView::draw() {
    if (_presets.empty()) return;

    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float barH = H * 0.07f;
    float barY = H - barH;
    float labelSize = H * 0.015f;
    float btnH = barH * 0.60f;
    float btnY = barY + barH * 0.20f;

    float btnW = W * 0.10f;
    float btnX = W - (W * 0.07f + W * 0.01f) * 4 - W * 0.02f - btnW - W * 0.01f;

    _selectorRect = sf::FloatRect({btnX, btnY}, {btnW, btnH});

    sf::RectangleShape bg({btnW, btnH});
    bg.setFillColor(sf::Color(40, 40, 65));
    bg.setOutlineThickness(1);
    bg.setOutlineColor(_dropdownOpen ? sf::Color(100, 100, 200) : sf::Color(60, 60, 90));
    bg.setPosition({btnX, btnY});
    _window.draw(bg);

    std::string label = _presets.empty() ? "No presets" : _presets[_selectedIndex];
    sf::Text txt(_font, label);
    txt.setCharacterSize(labelSize * 0.85f);
    txt.setFillColor(sf::Color::White);
    txt.setPosition({btnX + btnW * 0.05f, btnY + (btnH - labelSize) * 0.5f});
    _window.draw(txt);

    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    if (_dropdownOpen) {
        arrow.setPoint(0, {0, 6});
        arrow.setPoint(1, {6, 0});
        arrow.setPoint(2, {12, 6});
    } else {
        arrow.setPoint(0, {0, 0});
        arrow.setPoint(1, {12, 0});
        arrow.setPoint(2, {6, 6});
    }
    arrow.setFillColor(sf::Color(180, 180, 180));
    arrow.setPosition({btnX + btnW - 18, btnY + btnH * 0.45f});
    _window.draw(arrow);

    if (_dropdownOpen) {
        float dropY = btnY - _presets.size() * btnH;
        _dropdownRects.clear();

        for (int i = 0; i < (int)_presets.size(); i++) {
            float itemY = dropY + i * btnH;
            _dropdownRects.push_back(sf::FloatRect({btnX, itemY}, {btnW, btnH}));

            sf::RectangleShape item({btnW, btnH});
            item.setFillColor(i == _selectedIndex
                ? sf::Color(60, 60, 120)
                : sf::Color(30, 30, 50));
            item.setOutlineThickness(1);
            item.setOutlineColor(sf::Color(60, 60, 90));
            item.setPosition({btnX, itemY});
            _window.draw(item);

            sf::Text itemTxt(_font, _presets[i]);
            itemTxt.setCharacterSize(labelSize * 0.85f);
            itemTxt.setFillColor(sf::Color::White);
            itemTxt.setPosition({btnX + btnW * 0.05f, itemY + (btnH - labelSize) * 0.5f});
            _window.draw(itemTxt);
        }
    }
}
