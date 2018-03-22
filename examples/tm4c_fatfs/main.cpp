/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/core_generated.hpp>
#include <ecl/iostream.hpp>

extern "C"
void board_init()
{
    // Init GPIO
    gpio_init_generated();
}

int main()
{
    ecl::cout << "Starting FATFS example..." << ecl::endl;

    char buf[1024];
    size_t total_files = 0;
    size_t idx;

    // Mount filesystems
    auto rc = ecl::filesystem::mount_all();
    ecl_assert_msg(is_ok(rc), "failed to mount");

    // Open root directory of the filesystem
    auto root =  ecl::filesystem::open_dir("/sdcard/");
    ecl_assert_msg(root, "failed to open root");

    // Iterate over files in the root directory, and display their names

    ecl::fs::inode_ptr node;

    while ((node = root->read())) {
        size_t name_len = sizeof(buf);
        node->get_name(buf, name_len);

        ecl::cout << "#. "
            << total_files
            << " type: "
            << (node->get_type() == ecl::fs::inode::type::dir ? "dir " : "file")
            << " name : " << buf << ecl::endl;

        total_files++;
    }

    // Make sure some files are present in root directory,
    // otherwise this example is meaningles.
    ecl_assert_msg(total_files > 0, "no files in root directory");

    ecl::cout << "Which file to open?" << ecl::endl;
    ecl::cin >> idx;
    ecl_assert_msg(idx < total_files, "index is too big");

    // Back to the directory start.
    root->rewind();

    // Iterate all again to find appropriate file.
    size_t i = 0;
    do {
        node = root->read();
    } while (i++ < idx);

    // Check that requested file is a file
    ecl_assert_msg(node->get_type() == ecl::fs::inode::type::file, "selected item is not a file");

    // Open a file
    auto file = node->open();

    // Display a file
    while (true) {
        // After `read()` call, buf_size variable will contain amount of bytes
        // actually read. One byte at the end is reserver for NUL-terminator.
        auto buf_size = sizeof(buf) - 1;

        auto rc = file->read(reinterpret_cast<uint8_t*>(buf), buf_size);
        ecl_assert_msg(is_ok(rc), "failed to read a file");

        if (!buf_size) {
            // End-of-file is reached.
            break;
        }

        // NUL-terminate the buffer.
        buf[buf_size] = 0;

        // Print contents.
        ecl::cout << buf;
    }

    ecl::cout << ecl::endl;

    // Example is completed

    file->close();
    root->close();

    for(;;);
    return 0;
}
