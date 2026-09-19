7.0.0 (MuJoCo 3.13.0)
======================

.. rubric:: Breaking changes

*MuJoCo FFI upgraded to 3.13.0*

- MuJoCo-rs now builds against MuJoCo 3.13.0 FFI. This is a breaking dependency-level change for
  consumers tied to older MuJoCo C ABI details. See MuJoCo's changelog for this release:
  https://mujoco.readthedocs.io/en/3.13.0/changelog.html

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

.. rubric:: Bug fixes

- A static link (``MUJOCO_STATIC_LINK_DIR``) now loads OBJ and STL mesh files and reads and
  writes ``.mjz`` archives. MuJoCo registers those codecs from constructors in objects nothing
  references, so the linker dropped them: every mesh file failed with "no decoder found" and
  ``mj_encode`` reported "no encoder found". The ``mujoco`` submodule's static build now carries
  a table that names them, so a plain archive link keeps them.
- A static link with the ``cpp-viewer`` feature links on macOS. The build script names the Apple
  frameworks the static GLFW and Simulate resolve (Cocoa, IOKit, CoreFoundation and CoreVideo).

.. rubric:: Other changes

- A static link names ``miniz``, the archive library the ``.mjz`` codec needs, and no longer
  names ``lodepng``, whose objects MuJoCo 3.13.0 folds into ``libmujoco.a``. Rebuild the static
  libraries from the updated submodule, see :ref:`static_linking`.
