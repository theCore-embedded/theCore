/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/fs/path.hpp>

#include <CppUTest/CommandLineTestRunner.h>

using ecl::fs::path_iter;

TEST_GROUP(path_iter)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(path_iter, iterate_empty_path)
{
    const char path[] = "";

    path_iter iter{path};
    path_iter::segment seg;

    CHECK_FALSE(iter.next(seg));
    CHECK_FALSE(iter.next(seg));
}

TEST(path_iter, iterate_root_only_path)
{
    const char path[] = "////////";

    path_iter iter{path};
    path_iter::segment seg;

    CHECK_FALSE(iter.next(seg));
    CHECK_FALSE(iter.next(seg));
}

TEST(path_iter, iterate_relative_single_filename)
{
    const char path[] = "single_file.txt";

    path_iter iter{path};
    path_iter::segment seg;

    CHECK_TRUE(iter.next(seg));
    CHECK_EQUAL(&path[0], seg.first);
    CHECK_EQUAL(&path[sizeof(path) - 1], seg.second);

    // No more tokens must be fetched
    CHECK_FALSE(iter.next(seg));
}

TEST(path_iter, iterate_relative_single_dir)
{
    const char path[] = "this_is_directory/";

    path_iter iter{path};
    path_iter::segment seg;

    CHECK_TRUE(iter.next(seg));
    CHECK_EQUAL(&path[0], seg.first);
    CHECK_EQUAL(&path[sizeof(path) - 2], seg.second);

    // No more tokens must be fetched
    CHECK_FALSE(iter.next(seg));
}

TEST(path_iter, iterate_global_path)
{
    const char path[] = "/abc////def//hello.txt";

    path_iter iter{path};
    path_iter::segment seg;

    // abc portion
    CHECK_TRUE(iter.next(seg));

    CHECK_EQUAL(&path[1], seg.first);
    CHECK_EQUAL(&path[4], seg.second);

    // def portion
    CHECK_TRUE(iter.next(seg));

    CHECK_EQUAL(&path[8], seg.first);
    CHECK_EQUAL(&path[11], seg.second);

    // hello.txt portion
    CHECK_TRUE(iter.next(seg));

    CHECK_EQUAL(&path[13], seg.first);
    CHECK_EQUAL(&path[22], seg.second);

    // No more tokens must be fetched
    CHECK_FALSE(iter.next(seg));
}




int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
