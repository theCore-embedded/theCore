Host
----

Host platform is not a full-featured platform, like :ref:`theCore_STM32`,
but rather a place for evaluation and running unit tests.

Example configuration:

.. code-block:: json
   :linenos:

    {
        "platform": {
            "name": "host",
            "console": true
        }
    }

``console`` property controls whether console is enabled in the platform, or not.
For more details on theCore console, check the :ref:`theCore_Console` section.

Related references
~~~~~~~~~~~~~~~~~~

* :ref:`theCore_hello_world`
