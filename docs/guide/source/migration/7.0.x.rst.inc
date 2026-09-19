.. _migrate_7_0_0:

Migrating to 7.0.0
======================


MuJoCo upgrade
-----------------------

This release links against MuJoCo **3.13.0**. Download the matching release and
update your library path. See :ref:`installation` for details.

The FFI bindings follow the upstream API. This page gives no entry for an accessor that only
mirrors a renamed, a removed or a resized MuJoCo field, and none for a shifted enum discriminant.
For these changes, see MuJoCo's changelog for this release:
https://mujoco.readthedocs.io/en/3.13.0/changelog.html

