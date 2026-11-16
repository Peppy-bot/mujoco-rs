9.0.0 (MuJoCo 4.0.1)
====================

.. rubric:: Breaking changes

- MuJoCo-rs now builds against MuJoCo 4.0.1 FFI. A program that links another MuJoCo release
  panics at the version check. See MuJoCo's changelog for this release:
  https://mujoco.readthedocs.io/en/4.0.1/changelog.html. See :ref:`migrate_9_0_0`.
- The crate declares ``links = "mujoco"``, which 7.1.0 introduced: Cargo permits only one crate with
  this ``links`` value in a dependency graph.
- On Windows,
  :docs-rs:`~mujoco_rs::wrappers::mj_plugin::<fn>load_plugin_library` ends the process when the
  library fails to load, as it does on Linux and macOS: MuJoCo calls ``mju_error`` for the failure.

.. rubric:: New features and improvements

- 9.0.0 includes the changes of 7.1.0 (see below): the batch API, the sensor and state windows of
  the viewer UI, the exit confirmation of the viewer and ``DEP_MUJOCO_LIB_DIR``. The state window
  also shows the plastic state of MuJoCo 4.0.
- The string vectors of the specification have getters, which return the entries in order:
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex::<method>material` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex::<method>nodebody` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsFlex::<method>vertbody` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsSkin::<method>bodyname` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsTuple::<method>objname` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsMaterial::<method>textures` :sup:`new` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_editing::<struct>MjsTexture::<method>cubefiles` :sup:`new`.
  They read each entry with MuJoCo's ``mjs_getInStringVec``.

.. rubric:: Bug fixes

- MuJoCo 4.0.1 fixes the sparse Cholesky factorization of 4.0.0. With a sparse constraint
  Jacobian, the Newton solver received a wrong Hessian factor, which caused slow convergence,
  wrong accelerations and divergence.
- MuJoCo 4.0.1 fixes the single-shot multicontact of cylinder edges.
- :docs-rs:`~mujoco_rs::wrappers::mj_plugin::<fn>load_all_plugin_libraries` skips a library that
  fails to load: MuJoCo writes a warning, loads the other libraries of the directory and does not
  end the process.

.. rubric:: Other changes

- The ``mujoco`` submodule holds MuJoCo 4.0.1 with the fork's static-link patches. Rebuild the
  static libraries from the updated submodule, see :ref:`static_linking`.
