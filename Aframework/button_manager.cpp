#include "button_manager.h"

#include <utility>

void ButtonManager::on_input(const SDL_Event& e)
{
    if (e.type == SDL_MOUSEMOTION)
    {
        for (Button& button : _buttons)
        {
            button.handle_event(e);
        }

        return;
    }

    for (auto iter = _buttons.rbegin(); iter != _buttons.rend(); ++iter)
    {
        if (iter->handle_event(e))
        {
            break;
        }
    }
}

void ButtonManager::on_update(float dt)
{
    (void)dt;
}

void ButtonManager::on_render(SDL_Renderer* renderer)
{
    (void)renderer;

    for (Button& button : _buttons)
    {
        button.render();
    }
}

Button* ButtonManager::add_button(Button button)
{
    _buttons.emplace_back(std::move(button));

    return &(_buttons.back());
}

void ButtonManager::clear()
{
    _buttons.clear();
}

bool ButtonManager::empty() const
{
    return _buttons.empty();
}

std::size_t ButtonManager::size() const
{
    return _buttons.size();
}

Button* ButtonManager::get_button(std::size_t index)
{
    if (index >= _buttons.size())
    {
        return nullptr;
    }

    return &_buttons[index];
}

const Button* ButtonManager::get_button(std::size_t index) const
{
    if (index >= _buttons.size())
    {
        return nullptr;
    }

    return &_buttons[index];
}
