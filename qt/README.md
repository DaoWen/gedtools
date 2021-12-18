# Developing with Qt 4.7.4

GedTools uses a fairly old version of the Qt Framework,
and updating to the current Qt5 version is non-trivial.
Using the resources here can make dev with Qt4 easier.

## Docker Image

Dockerfile adapted from <https://github.com/grantslape/qt>.
Distributed under GPLv3 license (same as GedTools).
See parent directory for license file.

```
docker build -t qt:4.7.4 .
```

## Start script

Use the `start` script to get an interactive shell
for building and running the GedTools project.

## Windows Patch

If developing on Windows, and you're building qt from source,
then you may need the included `qt474fixup.patch` to get Qt4
to compile successfully with a current version of MinGW.

Instructions are included in the comment at the top of the file.
