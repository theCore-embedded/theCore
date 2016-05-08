#include <iostream>
#include <ecl/list.hpp>
#include <array>
#include <algorithm>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

struct dummy_data
{
    int val;
    ecl::list_node node;
};

TEST_GROUP(list_node)
{

};

TEST(list_node, empty_basic)
{
    ecl::list_node node;
    CHECK_FALSE(node.linked());

    // Unlink shouldn't cause any damage
    node.unlink();
    CHECK_FALSE(node.linked());
}

TEST(list_node, add_before_and_after_and_then_unlink)
{
    ecl::list_node cur;
    ecl::list_node next;
    ecl::list_node prev;

    cur.add_after(next);

    // Test new list structure
    CHECK_EQUAL(cur.next(), &next);
    CHECK_EQUAL(cur.prev(), &next);

    CHECK_EQUAL(next.prev(), &cur);
    CHECK_EQUAL(next.next(), &cur);

    cur.add_before(prev);

    // Test new list structure
    CHECK_EQUAL(cur.prev(), &prev);
    CHECK_EQUAL(cur.next(), &next);

    CHECK_EQUAL(prev.prev(), &next);
    CHECK_EQUAL(prev.next(), &cur);

    CHECK_EQUAL(next.prev(), &cur);
    CHECK_EQUAL(next.next(), &prev);

    CHECK_TRUE(cur.linked());

    // After unlinking next and prev nodes must be still connected
    cur.unlink();

    // Not in list
    CHECK_FALSE(cur.linked());

    // Two other nodes left in list
    CHECK_TRUE(prev.linked());
    CHECK_TRUE(next.linked());

    CHECK_EQUAL(prev.next(), &next);
    CHECK_EQUAL(prev.prev(), &next);

    CHECK_EQUAL(next.next(), &prev);
    CHECK_EQUAL(next.prev(), &prev);
}

//------------------------------------------------------------------------------

TEST_GROUP(list)
{

    ecl::list< dummy_data, &dummy_data::node > *head;

    void setup()
    {
        head = new ecl::list< dummy_data, &dummy_data::node >{};
        CHECK_TRUE(head->empty());
    }

    void teardown()
    {
        CHECK_TRUE(head->empty());
        delete head;
    }
};

TEST(list, push_back)
{
    dummy_data elem;
    CHECK_FALSE(elem.node.linked());

    head->push_back(elem);
    CHECK_TRUE(elem.node.linked());
}

TEST(list, begin_end)
{
    dummy_data elem;

    elem.val = 42;

    auto begin = head->begin();
    auto end = head->end();

    CHECK_TRUE(begin == end);

    head->push_back(elem);

    begin = head->begin();
    end = head->end();

    CHECK_TRUE(begin != end);
    CHECK_EQUAL(42, (*begin).val);
    CHECK_EQUAL(42, begin->val);

    begin++;
    CHECK_TRUE(begin == end);
}

TEST(list, iteration)
{
    std::array< dummy_data, 10 > elems;
    int test_val = 0;

    for (auto &elem : elems) {
        elem.val = test_val++;
        head->push_back(elem);
    }

    // Check required order
    for (auto &elem : *head) {
        std::cout << elem.val << std::endl;
        --test_val;
        CHECK_EQUAL(test_val, elem.val);
    }

    // Modify container using standart algorithms
    std::for_each(head->begin(), head->end(), [](auto &elem) {
        elem.val = 42;
    });

    // Validate modification
    for (auto &elem : elems) {
        CHECK_EQUAL(42, elem.val);
    }

    // Delete all nodes from a list
    for (auto &elem : *head) {
        elem.node.unlink();
    }

    // Check that all nodes has been removed from a list
    CHECK_TRUE(head->empty());
    for (auto &elem : elems) {
        CHECK_FALSE(elem.node.linked());
    }
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
