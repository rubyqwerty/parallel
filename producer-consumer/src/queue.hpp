#include "messages.hpp"
#include "shared_memory.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>

constexpr auto protocol_dir = "protocols";

class Queue
{
public:
    Queue(int capacity) { shared_.OpenSession().GetMessages()->capacity = capacity; }

    ~Queue() { Write(shared_.OpenSession().GetMessages()); }

    Message Push(Message &elem)
    {
        auto session = shared_.OpenSession();

        if (session.GetMessages()->current_size >= session.GetMessages()->capacity)
        {
            elem.status = REJECTED;
            session.GetMessages()->capacity++;
        }

        session.GetMessages()->messages[session.GetMessages()->current_size++] = elem;

        Write(session.GetMessages());

        return elem;
    }

    std::optional<Message> Pop(const GasType type)
    {
        auto session = shared_.OpenSession();
        auto messages = session.GetMessages();
        for (auto i = 0; i < messages->current_size; ++i)
        {
            if (messages->messages[i].gas_type == type && messages->messages[i].status == EXPECTED)
            {
                messages->messages[i].status = INWORK;
                messages->capacity++;
                const auto mes = messages->messages[i];
                Write(messages);
                return mes;
            }
        }

        return {};
    }

    void UpdateStatus(const Message &mes)
    {
        auto session = shared_.OpenSession();
        auto messages = session.GetMessages();

        for (int i = 0; i < messages->current_size; ++i)
        {
            if (mes.id == messages->messages[i].id)
            {
                messages->messages[i].status = mes.status;
                Write(messages);
                return;
            }
        }
    }

private:
    static void Write(StoredMessages *messages)
    {
        std::ofstream protocol{std::filesystem::path(protocol_dir) / "queue", std::ios_base::trunc};
        for (auto i = 0; i < messages->current_size; ++i)
        {
            protocol << messages->messages[i] << std::endl;
        }
    }

    void Remove(Message *mes, int index, int &size)
    {
        for (auto i = index; i < size; ++i)
        {
            mes[i] = mes[i + 1];
        }
        size--;
    }

private:
    SharedMemory shared_;
};