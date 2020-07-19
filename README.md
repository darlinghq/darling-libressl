# darling-libressl
This is Darling's version of LibreSSL.

Technically, it contains four different version of LibreSSL, each on its own branch. These versions are chosen to match the versions shipped with macOS.

Why is each one on its own branch? Why not tags? Well, we need to be able to update the code for each version (not so much the sources, but rather the CMakeLists). Tags would make this slightly more difficult (although, I'll admit, it's possible to use them for this; I just personally think it's easier to deal with branch for related but separate code).
