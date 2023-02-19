#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include "file.h"
namespace zkv {
FileWriter::FileWriter(const std::string& path_name, bool append) {
    std::string::size_type separator_pos = path_name.rfind('/');
    if (separator_pos == std::string::npos) {
    //那说明是当前路径
    } else {
        const auto& dir_path = std::string(path_name.data(), separator_pos);
        if (dir_path != ".") {
            mkdir(dir_path.data(), 0777);
        }
    }
    int32_t mode = O_CREAT | O_WRONLY;
    if (append) {
        mode |= O_APPEND;
    } else {
        mode |= O_TRUNC;
    }
    // LOG(WARN,"path=%s", path_name.c_str());
    fd_ = ::open(path_name.data(), mode, 0644);
    assert(::access(path_name.c_str(), F_OK) == 0);
}

DBStatus FileWriter::Append(const char* data, int32_t len) {
    if (len == 0 || !data) {
        return Status::kSuccess;
    }
    int32_t remain_size =
        std::min<int32_t>(len, kMaxFileBufferSize - current_pos_);
    memcpy(buffer_ + current_pos_, data, remain_size);
    data += remain_size;
    len -= remain_size;
    current_pos_ += remain_size;
    // 如果缓存区足够，我们先不刷盘，尽可能做的批量刷盘
    if (len == 0) {
        return Status::kSuccess;
    }
    // 这里可以保证能全部刷盘结束
    int ret = Writen(buffer_, current_pos_);
    current_pos_ = 0;
    if (ret == -1) {
        return Status::kWriteFileFailed;
    }
    if (len < static_cast<int32_t>(kMaxFileBufferSize)) {
        std::memcpy(buffer_, data, len);
        current_pos_ = len;
        return Status::kSuccess;
    }
    ret = Writen(data, len);
    if (ret == -1) {
        return Status::kWriteFileFailed;
    }
    return Status::kSuccess;
}
// 剩余的那些需要手动刷盘
DBStatus FileWriter::FlushBuffer() {
    if (current_pos_ > 0) {
        int ret = Writen(buffer_, current_pos_);
        current_pos_ = 0;
        if (ret == -1) {
            return Status::kWriteFileFailed;
        }
    }
    return Status::kSuccess;
}
ssize_t /* Write "n" bytes to a descriptor. */
FileWriter::Writen(const char* data, int len) {
    size_t nleft;      //剩余要写的字节数
    ssize_t nwritten;  //单次调用write()写入的字节数
    const char* ptr;   // write的缓冲区

    ptr = data;  //把传参进来的write要写的缓冲区备份一份
    nleft = len;  //还剩余需要写的字节数初始化为总共需要写的字节数
    while (nleft > 0) {  //检查传参进来的需要写的字节数的有效性
        if ((nwritten = write(fd_, ptr, nleft)) <= 0) {  //把ptr写入fd
            if (nwritten < 0 && errno == EINTR) {
                nwritten = 0; /* and call write() again */
            } else {
                return (-1); /* error 其他小于0的情况为错误*/
            }
        }

        nleft -=
            nwritten;  //还剩余需要写的字节数=现在还剩余需要写的字节数-这次已经写的字节数
        ptr +=
            nwritten;  //下次开始写的缓冲区位置=缓冲区现在的位置右移已经写了的字节数大小
    }
    return current_pos_;  //返回已经写了的字节数
}

void FileWriter::Sync() {
    FlushBuffer();
    if (fd_ > -1) {
        fsync(fd_);
    }
}
void FileWriter::Close() {
    FlushBuffer();
    if (fd_ > -1) {
        close(fd_);
        fd_ = -1;
    }
}
FileWriter::~FileWriter() {}


// file_reader
FileReader::~FileReader() {
  if (fd_ > -1) {
    close(fd_);
    fd_ = -1;
  }
}
FileReader::FileReader(const std::string& path_name) {
    if (::access(path_name.c_str(), F_OK) != 0) {
    // LOG(corekv::LogLevel::ERROR, "path_name:%s don't existed!",
    //     path_name.data());
    printf("path_name:%s don't existed!", path_name.data());
    } else {
        fd_ = open(path_name.data(), O_RDONLY);

    }
}
DBStatus FileReader::Read(uint64_t offset, size_t n,
                          std::string* result) const {
    if (!result) {
        return Status::kInvalidObject;
    }
    if (fd_ == -1) {
        // LOG(corekv::LogLevel::ERROR, "Invalid Socket");
        return Status::kInterupt;
    }
    // printf("%s\n", "zzzzzzzzzzzzzzzzzzzzzz");
    // printf("%d\n", fd_);


    pread(fd_, result->data(), n, static_cast<off_t>(offset));
    // printf("%s\n", result->c_str());
    return Status::kSuccess;
}

uint64_t FileTool::GetFileSize(const std::string_view& path) {
    if (path.empty()) {
        return 0;
    }
    struct ::stat file_stat;
    if (::stat(path.data(), &file_stat) != 0) {
        return 0;
    }
    return file_stat.st_size;
}
bool FileTool::Exist(std::string_view path_name) {
    return !path_name.empty() && (::access(path_name.data(), F_OK) == 0);
}
bool FileTool::Rename(std::string_view from, std::string_view to) {
    if (from.empty() || to.empty()) {
        return false;
    }
    if (std::rename(from.data(), to.data()) != 0) {
        // LOG(ERROR, "rename failed, code = [%d]", errno);

        return false;
    }
    return true;
}

bool FileTool::RemoveFile(const std::string& filename) {
if (::unlink(filename.c_str()) != 0) {
    // LOG(ERROR, "RemoveFile failed, code = [%d]", errno);

    return false;
}
return true;
}

bool FileTool::CreateDir(const std::string& dirname)  {
if (::mkdir(dirname.c_str(), 0755) != 0) {
    // LOG(ERROR, "CreateDir failed, code = [%d]", errno);
    return false;
}
return true;
}

bool FileTool::RemoveDir(const std::string& dirname) {
if (::rmdir(dirname.c_str()) != 0) {
    // LOG(ERROR, "RemoveDir failed, code = [%d]", errno);
    return false;
}
return true;
}
}

