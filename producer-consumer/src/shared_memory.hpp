#include "session.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

class SharedMemory
{
public:
    SharedMemory() : semaphore_(std::make_shared<Semaphore>(SEMAPHORE_NAME))
    {
        shm_fd_ = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
        if (shm_fd_ == -1)
        {
            throw std::runtime_error("Не удалось открыть объект разделяемой памяти");
        }

        ftruncate(shm_fd_, SHARED_MEMORY_SIZE);

        // Отображение разделяемой памяти в адресное пространство процесса
        auto memory = mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);

        if (memory == MAP_FAILED)
        {
            throw std::runtime_error("Не удалось отобразить разделяемую память");
        }

        stored_messages_ = reinterpret_cast<StoredMessages *>(memory);
    }

    ~SharedMemory()
    {
        shm_unlink(SHARED_MEMORY_NAME);
        munmap(stored_messages_, SHARED_MEMORY_SIZE);
        close(shm_fd_);
    }

    Session OpenSession() { return Session(stored_messages_, semaphore_); }

private:
    StoredMessages *stored_messages_;
    int shm_fd_{-1};
    SemaphorePtr semaphore_;
};