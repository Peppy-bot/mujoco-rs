.. _migrate_9_0_0:

Migrating to 9.0.0
==================

9.0.0 binds MuJoCo 4.0.1, a bug-fix release of MuJoCo 4.0. Read MuJoCo's
`changelog <https://mujoco.readthedocs.io/en/4.0.1/changelog.html>`__ for its fixes.


Link MuJoCo 4.0.1
-----------------

MuJoCo-rs compares the version of the MuJoCo library that it links with the version of its
bindings, and panics when they are not the same. If you link your own MuJoCo
(``MUJOCO_DYNAMIC_LINK_DIR``), install the 4.0.1 release and point the variable at it. If you link
statically (``MUJOCO_STATIC_LINK_DIR``), rebuild the static libraries from the ``mujoco``
submodule, see :ref:`static_linking`. The ``auto-download-mujoco`` feature downloads 4.0.1.


One MuJoCo-rs per dependency graph
----------------------------------

The crate declares ``links = "mujoco"``, so Cargo refuses a dependency graph that holds two versions
of MuJoCo-rs. Make every crate of the graph depend on the same version.


Plugin libraries
----------------

:docs-rs:`~mujoco_rs::wrappers::mj_plugin::<fn>load_all_plugin_libraries` no longer ends the process
when a library of the directory fails to load: MuJoCo writes a warning and skips the library. If
your program needs a plugin, check that the model loads or that the plugin is registered after the
call. On Windows,
:docs-rs:`~mujoco_rs::wrappers::mj_plugin::<fn>load_plugin_library` ends the process when the
library fails to load. Make sure that the path names a library that loads.


Simulation results
------------------

The fix of the sparse Cholesky factorization changes the result of the Newton solver when the
constraint Jacobian is sparse. A test that compares a trajectory with values recorded on MuJoCo
4.0.0 can need new reference values.
