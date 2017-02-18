
// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}


TEST_GROUP(serial)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};


TEST(serial, init)
{
    // TODO
}
