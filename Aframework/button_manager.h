#pragma once
#include <SDL.h>
#include <cstddef>
#include <utility>
#include <vector>

#include "button.h"

class ButtonManager
{
public:

    void on_input(const SDL_Event& e);
    void on_update(float dt);
    void on_render(SDL_Renderer* renderer);

    Button* add_button(Button button);

    template <typename... Args>
    Button& emplace_button(Args&&... args)
    {
        _buttons.emplace_back(std::forward<Args>(args)...);
        return _buttons.back();
    }

    void clear();

    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] Button* get_button(std::size_t index);
    [[nodiscard]] const Button* get_button(std::size_t index) const;

private:
    std::vector<Button> _buttons;
};
