.. _migrate_7_0_0:

Migrating to 7.0.0
======================


MuJoCo upgrade
-----------------------

This release links against MuJoCo **3.14.0**. Download the matching release and
update your library path. See :ref:`installation` for details.

The FFI bindings follow the upstream API. This page gives no entry for an accessor that only
mirrors a renamed, a removed or a resized MuJoCo field, and none for a shifted enum discriminant.
For these changes, see MuJoCo's changelog for this release:
https://mujoco.readthedocs.io/en/3.14.0/changelog.html


Control-history reads
-----------------------

``read_ctrl`` and ``try_read_ctrl`` now return all ``actuator_ctrlnum[id]`` controls of the
actuator as a ``Cow<[MjtNum]>``. For a one-control actuator, index the result, or read a
one-element array with ``read_ctrl_fixed`` to avoid the allocation that an interpolated read
makes. ``init_ctrl_history`` takes ``nsample * actuator_ctrlnum[id]`` values, sample after sample.

**Before:**

.. code-block:: rust

    let value: MjtNum = data.read_ctrl(id, time, 1);

**After:**

.. code-block:: rust

    let value: MjtNum = data.read_ctrl(id, time, 1)[0];
    let [value] = data.read_ctrl_fixed::<1>(id, time, 1);


Static linking
-----------------------

A static build needs the libraries of the updated ``mujoco`` submodule
(``release/3.14.0-static``). Rebuild them as described in :ref:`static_linking`; a ``lib/``
directory built from the 3.12.0 branch no longer links, because the build script now names
``miniz`` and no longer names ``lodepng``, and only the updated branch keeps the mesh and
``.mjz`` codecs in a static link.
