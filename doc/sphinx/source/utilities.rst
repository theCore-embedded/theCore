Libraries and utilities
=======================

.. _theCore_Configurator:

JSON-based configurator
-----------------------

.. note:: This section is under construction.

theCore configurator is the utility that allows to configure
the framework with respect to user-defined parameters, such as a platform name,
a clock speed or a console baud rate.

The configurator runs during build, thus imposing no additional cost in runtime.

To start using the configurator, simply create a JSON file, describing required config
and pass an absolute path of the JSON using either ``THECORE_TARGET_CONFIG_FILE`` CMake variable,
or using ``theCore_set_target_config`` CMake function in your project CMake file.

Simplest example
~~~~~~~~~~~~~~~~

To configure theCore to be used on the host platform, one can perform
following steps:

- Create JSON with the require configuration - ``target_config.json``::

    echo '
    {
        "platform": {
            "name": "host"
        }
    }
    ' > target_config.json

- Create the main source file - ``main.cpp``::

    echo '
    int main() { for(;;); /* Just for demonstration */ }
    ' > main.cpp

- Create the main project CMake file - ``CMakeLists.txt``::

    echo '
    include(${CORE_DIR}/build_api.cmake)

    add_executable(main main.cpp)

    theCore_set_target_config("${CMAKE_CURRENT_LIST_DIR}/target_config.json")
    ' > CMakeLists.txt

- Launch build and the app::

    mkdir build
    cmake ..
    make
    ./main

The approach described here is basic for all platforms, but values in JSON of course
are different. This of JSON as it is being API between user and theCore.

Such APIs exposed by different modules. Here are some references:

  - The :ref:`theCore_Platforms` section contains platform-level configuration explanation.

JSON configuration examples
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get more insights about theCore configuration, refer to the :ref:`theCore_Examples`
page. Each example there contains a subsection with a JSON config displayed.

JSON validation
~~~~~~~~~~~~~~~

Each theCore module that exposes the configuration API, also provides
`JSON schema`_ for basic input validation. Usually if the JSON does not match
required schema the build error looks like this::

    jsonschema.exceptions.ValidationError: 'zzzz' is not of type 'integer'

    Failed validating 'type' in schema['properties']['uart']['items']['properties']['baud']:
        {'type': 'integer'}

    On instance['uart'][0]['baud']:
        'zzzz'

In the example above, baud rate of UART was not specified correctly. This kind
of errors the schema can easily catch. Refer to the concrete module
documentation or the source code to understand which values are validated.


.. _theCore_Console:

Console streams
---------------

.. note:: This section is under construction.

Assertions and error handling
-----------------------------

.. note:: This section is under construction.

Filesystem
----------

.. note:: This section is under construction.

Newlib stubs
------------

.. note:: This section is under construction.


.. _JSON schema: http://json-schema.org/
