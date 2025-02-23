#include <chrono>
#include <csignal>
#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "car.hpp"
#include "race.hpp"

using namespace ftxui;

std::unique_ptr<Car> car = nullptr;

void signalHandler(int signum)
{
    if (car != nullptr)
        car->Start();
}

int main()
{
    signal(SIGUSR1, signalHandler);

    const auto parent_pid = getpid();

    for (auto i = 0; i < 5; ++i)
    {
        const auto pid = fork();

        if (pid == 0)
        {
            car = std::make_unique<Car>(i);

            setpgid(0, parent_pid);
            car->Wait();

            return 0;
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Race race;
    race.StartRace();

    auto screen = ScreenInteractive::Fullscreen();

    auto component = Renderer(
        [&race]
        {
            const auto results = race.GetResult();

            const auto positions = race.GetPositions();

            Color colorPlace = race.IsFinally() ? Color::Blue : Color::Green;

            return vbox({hbox({text("Машина 1: ") | color(Color::Blue) | bold | center,
                               gauge(positions[0]) | color(Color::Red) | border | size(WIDTH, EQUAL, 60),
                               (results[0] != -1 ? text(std::to_string(results[0]) + " место") : text("")) |
                                   color(colorPlace) | bold | center}),

                         hbox({text("Машина 2: ") | color(Color::Blue) | bold | center,
                               gauge(positions[1]) | color(Color::Red) | border | size(WIDTH, EQUAL, 60),
                               (results[1] != -1 ? text(std::to_string(results[1]) + " место") : text("")) |
                                   color(colorPlace) | bold | center}),

                         hbox({
                             text("Машина 3: ") | color(Color::Blue) | bold | center,
                             gauge(positions[2]) | color(Color::Red) | border | size(WIDTH, EQUAL, 60),
                             (results[2] != -1 ? text(std::to_string(results[2]) + " место") : text("")) |
                                 color(colorPlace) | bold | center,
                         }),

                         hbox({
                             text("Машина 4: ") | color(Color::Blue) | bold | center,
                             gauge(positions[3]) | color(Color::Red) | border | size(WIDTH, EQUAL, 60),
                             (results[3] != -1 ? text(std::to_string(results[3]) + " место") : text("")) |
                                 color(colorPlace) | bold | center,
                         }),

                         hbox({text("Машина 5: ") | color(Color::Blue) | bold | center,
                               gauge(positions[4]) | color(Color::Red) | border | size(WIDTH, EQUAL, 60),
                               (results[4] != -1 ? text(std::to_string(results[4]) + " место") : text("")) |
                                   color(colorPlace) | bold | center})});
        });

    std::thread(
        [&]
        {
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                screen.PostEvent(Event::Custom);
            }
        })
        .detach();

    screen.Loop(component);
}
