Documentation development and contribution
==========================================

theCore documentation, as you may have noticed, is hosted on GitHub pages and
built using Sphinx_ with few extensions:

 - `Read The Docs theme`_ - awesome theme.
 - FullToc_ - full ToC in the sidebar.

All the documentation sources placed under ``doc`` top-level directory.

To contribute to the documentation, you probably would like to first build
and validate docs locally. For that, build dependencies must be installed either
manually or by using Nix. Check the :ref:`theCore_GettingStarted` for more info about
Nix option.

Following sections will provide you with insights about docs structure and build
procedure.

.. _theCore_building_Sphinx:

Building Sphinx documentation
-----------------------------

theCore build system provides a separate target for building documentation, so
there is no need to invoke Sphinx manually. Instead, proceed as follows:

#. Finish :ref:`theCore_GettingStarted` to get Sphinx and additional extensions.
#. Launch documentation build::

     # From theCore directory
     mkdir doc_build
     cd doc_build
     cmake ../doc
     make core_sphinx

#. Open generated docs by any preferable browser:

   - ``doc_build/sphinx/theCore/index.html`` - main page of multi-page version.
   - ``doc_build/sphinx/theCore/singlehtml/index.html`` - main page of single-paged version.

Building Doxygen documentation
------------------------------

theCore also contains Doxygen and it is possible to generate Doxygen pages
locally using CMake target:

#. Launch command in the ``doc_build`` directory
   :ref:`created on previous step <theCore_building_Sphinx>`::

     make core_doxygen

#. Open index page of generated Doxygen by any preferable browser:
   ``doc_build/doc/doxygen/html/index.html``

What all these sections are about?
----------------------------------

.. note:: This section is under construction.

Publishing docs on github pages
-------------------------------

As noted previously, docs are hosted in GitHub pages. `The separate repository`_
holds auto-generated static HTML files which are automatically displayed if placed in
master branch of that repo. Jekyll is disabled.

See also `the GitHub pages docs`_ for more information.

Contributing to the documentation consist of creating a PR against `theCore repository`_.

After the PR is opened, the new documentation is processed by the author manually in following fashion:

#. New docs are build and tested locally, as described in `Building Sphinx documentation`_ section.
#. Generated static files are copied to the github pages clone::

     cp doc_build/doc/sphinx/* /path/to/the/gh_pages/ -rv

#. Changes are committed and pushed to the remote::

     git add -u
     git commit
     git push origin master

.. _Sphinx: http://www.sphinx-doc.org/en/stable/
.. _Read The Docs theme: http://docs.readthedocs.io/en/latest/theme.html
.. _FullToc: https://sphinxcontrib-fulltoc.readthedocs.io
.. _The separate repository: https://github.com/forGGe/forGGe.github.io
.. _theCore repository: https://github.com/forGGe/theCore
.. _the GitHub pages docs: https://help.github.com/articles/user-organization-and-project-pages/
