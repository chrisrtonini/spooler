#include "gtest/gtest.h"
#include "../src/fs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

TEST(FS, CreateFifoSuccess) {
    const char* path = "test_fifo";
    ::unlink(path);
    bool created = false;
    try {
        created = fs::create_node(path, S_IFIFO | 0600, 0);
    } catch(const fs_exp&) {
        created = false;
    }
    EXPECT_TRUE(created);
    struct stat st{};
    EXPECT_EQ(0, ::stat(path, &st));
    EXPECT_TRUE((st.st_mode & S_IFIFO) == S_IFIFO);
    EXPECT_EQ(0600, st.st_mode & 0777);
    ::unlink(path);
}

TEST(FS, CreateNodeInvalidPath) {
    bool created = true;
    try {
        created = fs::create_node("/invalid_path/does_not_exist/file", S_IFIFO | 0600, 0);
    } catch(const fs_exp&) {
        created = false;
    }
    EXPECT_TRUE(!created);
}

int main() {
    return RUN_ALL_TESTS();
}
