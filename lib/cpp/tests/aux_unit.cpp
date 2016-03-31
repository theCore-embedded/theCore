#include "memory.hpp"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

struct mock_aux : ecl::aux
{
    virtual void destroy()
    {
        mock("aux").actualCall("destroy");
    }
};

TEST_GROUP(aux)
{
    ecl::aux *test_aux;

    void setup()
    {
        test_aux = new mock_aux;
    }

    void teardown()
    {
        mock().clear();
        delete test_aux;
    }
};

TEST(aux, constructed)
{
    auto weak = test_aux->weak_ref();
    auto ref = test_aux->ref();
    CHECK_EQUAL(0, weak);
    CHECK_EQUAL(0, ref);
}

TEST(aux, destroy_called_and_state_not_changed)
{
    mock("aux").expectOneCall("destroy");
    test_aux->destroy();

    auto weak = test_aux->weak_ref();
    auto ref = test_aux->ref();
    CHECK_EQUAL(0, weak);
    CHECK_EQUAL(0, ref);
}

TEST(aux, inc_dec_get_methods)
{
    test_aux->inc();
    test_aux->weak_inc();

    auto ref = test_aux->inc();
    auto weak = test_aux->weak_inc();

    CHECK_EQUAL(2, ref);
    CHECK_EQUAL(2, weak);

    ref = test_aux->ref();
    weak = test_aux->weak_ref();

    CHECK_EQUAL(2, ref);
    CHECK_EQUAL(2, weak);

    ref = test_aux->dec();
    weak = test_aux->weak_dec();

    CHECK_EQUAL(1, ref);
    CHECK_EQUAL(1, weak);
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
