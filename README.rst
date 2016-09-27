Cenisys
=======

.. image:: https://gitlab.com/itxtech/cenisys/badges/master/build.svg
   :alt: GitLab CI
   :target: https://gitlab.com/itxtech/cenisys/builds

Canonical source
----------------

The source of Cenisys is maintained on GitLab.com_. Currently Cenisys is mirrored on GitHub for convenience.
You should make contributions on GitLab if possible.

.. _GitLab.com: https://gitlab.com/itxtech/cenisys

Clean, concurrent, coroutine-enabled Minecraft server written in C++
--------------------------------------------------------------------

Cenisys is written in C++ and has the following feature:

- Asynchronous I/O with Boost.Asio
- Beautiful design and API powered by latest C++ standard and Boost library
- Utilizing full CPU power with userland threads

Requirements
------------

This project uses CMake as build system.

You will need the following libraries for build:

- Boost 1.62

To generate documentation, you will need additional tools:

- Sphinx
- breathe
- doxygen

The required C++ standard is C++14.

License
-------

Cenisys is licensed under AGPLv3. Plugins must be licensed under a compatible license.

Documentations
--------------

Documentations are bundled with release. You can also browse the git version on `GitLab Pages`_.

.. _GitLab Pages: https://itxtech.gitlab.io/cenisys
