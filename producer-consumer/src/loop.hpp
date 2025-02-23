#include "messages.hpp"
#include "queue.hpp"
#include "settings.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>
#include <ratio>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

class MainLoop
{
public:
    MainLoop(Settings settings) : settings_(settings), queue_(settings.number_message_queue)
    {
        // Обновляем протоколы
        if (std::filesystem::exists(protocol_dir))
        {
            std::filesystem::remove_all(protocol_dir);
        }
        std::filesystem::create_directory(protocol_dir);

        // Слушаем сигналы, оповещающие о появлении заказа
        sigemptyset(&set_);
        sigaddset(&set_, SIGUSR1);
        pthread_sigmask(SIG_BLOCK, &set_, nullptr);

        run_ = true;

        parent_pid_ = getpid();

        for (const auto &station : settings_.stations_params)
        {
            const auto pid = fork();

            if (pid == 0)
            {
                setpgid(0, parent_pid_);
                thread_ = std::thread{&MainLoop::Consume, this, station};
                return;
            }

            processes_.push_back(pid);
        }

        thread_ = std::thread{&MainLoop::Produce, this, settings_.create};
    }

    ~MainLoop()
    {
        run_ = false;

        for (auto &i : processes_)
        {
            int status;
            waitpid(i, &status, 0);
        }

        thread_.join();
    }

private:
    void Produce(Params par)
    {
        std::ofstream protocol{std::filesystem::path(protocol_dir) / "rejected"};

        std::random_device rd;
        std::mt19937 gen(rd());

        int id = 0;

        while (run_)
        {
            const auto type = type_gas_(gen);
            const auto time = GetRandomTime(par);

            // Имитируем работу
            std::this_thread::sleep_for(std::chrono::milliseconds(time));

            Message mes;
            mes.gas_type = static_cast<GasType>(type);
            mes.id = id++;

            const auto added_mes = queue_.Push(mes);
            if (mes.status == REJECTED)
            {
                protocol << added_mes << std::endl;
            }
            std::cout << mes << std::endl;

            killpg(parent_pid_, SIGUSR1);
        }
    }

    void Consume(Station station)
    {
        std::stringstream name_protocol{};
        name_protocol << ToString(station.type) << " " << getpid();

        std::ofstream protocol{std::filesystem::path(protocol_dir) / name_protocol.str()};

        while (run_)
        {
            int sig;
            sigwait(&set_, &sig);

            while (auto mes = queue_.Pop(station.type))
            {
                std::cout << mes.value() << std::endl;

                const auto time = GetRandomTime(station.handle);
                std::this_thread::sleep_for(std::chrono::milliseconds(time));
                mes->status = PROCESSED;

                std::cout << mes.value() << std::endl;
                protocol << mes.value() << std::endl;

                queue_.UpdateStatus(mes.value());
            }
        }
    }

    int GetRandomTime(Params st)
    {
        double mean = st.mean;
        double stddev = st.stddev;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dist(mean, stddev);

        return dist(gen);
    }

private:
    Queue queue_;
    std::atomic<bool> run_{false};

    sigset_t set_;
    pid_t parent_pid_;

    std::thread thread_;
    std::vector<pid_t> processes_{};

private:
    Settings settings_;
    std::uniform_int_distribution<int> type_gas_{0, 2};
};