7.0.0 (MuJoCo 3.14.0)
======================

.. rubric:: Breaking changes

*MuJoCo FFI upgraded to 3.14.0*

- MuJoCo-rs now builds against MuJoCo 3.14.0 FFI. This is a breaking dependency-level change for
  consumers tied to older MuJoCo C ABI details. See MuJoCo's changelog for this release:
  https://mujoco.readthedocs.io/en/3.14.0/changelog.html

*Control-history reads return every control of the actuator*

- :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>read_ctrl` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>try_read_ctrl` now return a
  ``Cow<[MjtNum]>`` with the ``actuator_ctrlnum[id]`` controls of the actuator instead of one
  ``MjtNum``, because MuJoCo 3.14.0 reads the controls of a multi-input actuator as one block.
  |mj_data| gains readers that do not allocate, as the sensor readers do:
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>read_ctrl_into` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>read_ctrl_fixed` :sup:`new`
  and :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>try_read_ctrl_fixed`
  :sup:`new`. See :ref:`migrate_7_0_0`.
- :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>init_ctrl_history` now
  expects ``nsample * actuator_ctrlnum[id]`` values. For a multi-input actuator it returns
  ``LengthMismatch`` (:docs-rs:`~mujoco_rs::error::<enum>MjDataError`) for the ``nsample`` values
  it accepted before.

.. rubric:: New features and improvements

*Site inside test*

- :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>inside_site` :sup:`new` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>try_inside_site` :sup:`new`
  report whether a point lies inside a site; a mesh site tests its convex hull. They wrap
  :docs-rs:`~mujoco_rs::mujoco_c::<fn>mj_insideSite`.

*New* |mj_data| *effective-metric accessors*

- The accessors that read the metric terms MuJoCo fills at a later pipeline stage are ``unsafe fn``:
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_diag`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_sdiag`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_fluid`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_ts` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_tk`, together with their
  ``_mut`` counterparts. Run the velocity stage (``forward`` or ``step1``) before you read them.
- The tendon and actuator lists
  (:docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_tid`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_ts`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_tk`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_aid`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_as` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>efm_ak`) are sized by the
  number of entries the metric holds (``nefmT`` and ``nefmA``), not by the ``ntendon`` and
  ``nactuator`` capacities MuJoCo allocates.

*Allocation-free Jacobians*

- |mj_data| gains an allocation-free variant of every Jacobian method, which writes into a buffer
  that the caller keeps, so a control loop no longer allocates two vectors per call:
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_into` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_body_into` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_body_com_into` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_subtree_com_into`
  :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_geom_into` :sup:`new`,
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>jac_site_into` :sup:`new` and
  :docs-rs:`~~mujoco_rs::wrappers::mj_data::<struct>MjData::<method>angmom_mat_into` :sup:`new`.
- The guide gains a :ref:`performance <simulation_performance>` section that covers the thread
  pool and buffer reuse.

.. rubric:: Bug fixes

- A static link (``MUJOCO_STATIC_LINK_DIR``) now loads OBJ and STL mesh files and reads and
  writes ``.mjz`` archives. MuJoCo registers those codecs from constructors in objects nothing
  references, so the linker dropped them: every mesh file failed with "no decoder found" and
  ``mj_encode`` reported "no encoder found". The ``mujoco`` submodule's static build now carries
  a table that names them, so a plain archive link keeps them.
- A static link with the ``cpp-viewer`` feature links on macOS. The build script names the Apple
  frameworks the static GLFW and Simulate resolve (Cocoa, IOKit, CoreFoundation and CoreVideo).
- A copy of |mj_model| now shares the layout snapshot of the original even when the original had
  not built it yet, so
  :docs-rs:`~~mujoco_rs::wrappers::mj_model::<struct>MjModel::<method>is_compatible_with_model`
  between the two is a pointer comparison again. The :ref:`mj_rust_viewer` runs that test on
  every :docs-rs:`~~mujoco_rs::viewer::<struct>MjViewer::<method>sync_data` call.

.. rubric:: Other changes

- A static link names ``miniz``, the archive library the ``.mjz`` codec needs, and no longer
  names ``lodepng``, whose objects MuJoCo 3.13.0 folds into ``libmujoco.a``. Rebuild the static
  libraries from the updated submodule, see :ref:`static_linking`.
