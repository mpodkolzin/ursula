#include "io/file_handle.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <iostream>

FileHandle::FileHandle(int fd) : fd_(fd) {}

FileHandle::FileHandle(const std::string& path, int flags, mode_t mode)
{
    fd_ = ::open(path.c_str(), flags, mode);
    if (fd_ < 0) {
        throw std::runtime_error("Failed to open file '" + path + "': " + std::string(std::strerror(errno)));
    }
}

FileHandle::~FileHandle() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}
    
ssize_t FileHandle::write(const void* data, size_t size) {
    if (!is_valid()) {
        return -1;
    }
    return ::write(fd_, data, size);
}

ssize_t FileHandle::read(void* buffer, size_t size) {
    if (!is_valid()) {
        return -1;
    }
    return ::read(fd_, buffer, size);
}

off_t FileHandle::seek(off_t offset, int whence) {
    if (!is_valid()) {
        return -1;
    }
    return ::lseek(fd_, offset, whence);
}

void FileHandle::flush() {
    if (is_valid()) {
        // On Unix-like systems, fsync() flushes the file to disk
        ::fsync(fd_);
    }
}

void FileHandle::close() {
    if (is_valid()) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool FileHandle::is_valid() const {
    return fd_ >= 0;
}

int FileHandle::get_fd() const {
    return fd_;
}
