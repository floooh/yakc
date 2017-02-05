//------------------------------------------------------------------------------
//  filesystem_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/systems/filesystem.h"

using namespace YAKC;

TEST(filesystem) {

    filesystem fs;
    for (const auto& block : fs.blocks) {
        CHECK(block.free);
    }
    for (const auto& f : fs.files) {
        CHECK(f.name[0] == 0);
        CHECK(f.open_mode == filesystem::mode::none);
        CHECK(!f.valid);
        CHECK(f.pos == 0);
        CHECK(f.size == 0);
    }
    CHECK(!fs.exists("bla"));
    CHECK(!fs.open("bla", filesystem::mode::read));

    // open/closing empty files
    filesystem::file fp = fs.open("bla", filesystem::mode::write);
    CHECK(fp);
    CHECK(fs.exists("bla"));
    CHECK(!fs.open("bla", filesystem::mode::read));
    CHECK(!fs.open("bla", filesystem::mode::write));
    CHECK(fs.get_pos(fp) == 0);
    CHECK(!fs.set_pos(fp, 0));  // no set_pos in write mode
    fs.close(fp);
    CHECK(fs.exists("bla"));
    fp = fs.open("bla", filesystem::mode::read);
    CHECK(fp);
    CHECK(!fs.open("bla", filesystem::mode::read));
    CHECK(!fs.open("bla", filesystem::mode::write));
    CHECK(fs.get_pos(fp) == 0);
    CHECK(!fs.set_pos(fp, -1));
    CHECK(!fs.set_pos(fp, 1));
    CHECK(fs.set_pos(fp, 0));
    fs.close(fp);
    fs.rm("bla");
    CHECK(!fs.exists("bla"));

    // create a file with content
    uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t buf[sizeof(bytes)] = { };
    fp = fs.open("bla", filesystem::mode::write);
    for (int i = 0; i < 1000; i++) {
        int num = fs.write(fp, bytes, sizeof(bytes));
        CHECK(num == sizeof(bytes));
    }
    CHECK(fs.size(fp) == sizeof(bytes) * 1000);
    fs.close(fp);
    fp = fs.open("bla", filesystem::mode::read);
    for (int i = 0; i < 1000; i++) {
        int num = fs.read(fp, buf, sizeof(buf));
        CHECK(num == sizeof(buf));
        for (int j = 0; j < int(sizeof(buf)/sizeof(buf[0])); j++) {
            CHECK(buf[j] == bytes[j]);
        }
    }
    fs.close(fp);

    // create 2 files at the same time
    uint8_t bytes0[] = { 100, 101, 102, 103, 104, 105, 106, 107 };
    uint8_t bytes1[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    uint8_t buf0[sizeof(bytes0)];
    uint8_t buf1[sizeof(bytes1)];
    filesystem::file fp0 = fs.open("bla0", filesystem::mode::write);
    filesystem::file fp1 = fs.open("bla1", filesystem::mode::write);
    for (int i = 0; i < 1000; i++) {
        int num0 = fs.write(fp0, bytes0, sizeof(bytes0));
        CHECK(num0 == sizeof(bytes0));
        int num1 = fs.write(fp1, bytes1, sizeof(bytes1));
        CHECK(num1 == sizeof(bytes1));
    }
    CHECK(fs.size(fp0) == sizeof(bytes0) * 1000);
    CHECK(fs.size(fp1) == sizeof(bytes1) * 1000);
    fs.close(fp0);
    fs.close(fp1);

    fp0 = fs.open("bla0", filesystem::mode::read);
    fp1 = fs.open("bla1", filesystem::mode::read);
    for (int i = 0; i < 1000; i++) {
        int num0 = fs.read(fp0, buf0, sizeof(buf0));
        CHECK(num0 == sizeof(buf0));
        for (int j = 0; j < int(sizeof(buf0)/sizeof(buf0[0])); j++) {
            CHECK(buf0[j] == bytes0[j]);
        }
        int num1 = fs.read(fp1, buf1, sizeof(buf1));
        CHECK(num1 == sizeof(buf1));
        for (int j = 0; j < int(sizeof(buf1)/sizeof(buf1[0])); j++) {
            CHECK(buf1[j] == bytes1[j]);
        }
    }
    fs.close(fp0);
    fs.close(fp1);

    // repeat the read test from the first write file, content should still be the same
    fp = fs.open("bla", filesystem::mode::read);
    for (int i = 0; i < 1000; i++) {
        int num = fs.read(fp, buf, sizeof(buf));
        CHECK(num == sizeof(buf));
        for (int j = 0; j < int(sizeof(buf)/sizeof(buf[0])); j++) {
            CHECK(buf[j] == bytes[j]);
        }
    }
    fs.close(fp);
}

