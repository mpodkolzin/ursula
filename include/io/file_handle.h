#pragma once

#include <cstddef>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <unistd.h>

class FileHandle {
public:
    explicit FileHandle(int fd);
    FileHandle(const std::string& path, int flags, mode_t mode = 0644); // overloaded constructor
    ~FileHandle();

    ssize_t write(const void* data, size_t size);
    ssize_t read(void* buffer, size_t size);
    off_t seek(off_t offset, int whence);
    void flush();
    void close();

    bool is_valid() const;
    int get_fd() const;
    void fsync();
    size_t file_size() const;
private:
    int fd_;
    void close_fd();
};
