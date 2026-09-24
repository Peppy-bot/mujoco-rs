//! Integration tests for the codecs MuJoCo registers from constructors: the `.mjz` archive
//! encoder and decoder and the OBJ and STL mesh decoders.
//!
//! Each of them lives in a translation unit nothing else references, so a static link keeps it
//! only through the table `libmujoco.a` carries for that purpose. Without it a static link
//! parses XML but reports "no encoder found" for `.mjz` and fails to load any mesh file.

use std::ffi::{CStr, CString, c_char};
use std::path::PathBuf;
use std::ptr;

use mujoco_rs::mujoco_c::mj_encode;
use mujoco_rs::prelude::*;

/// Size of the error buffer `mj_encode` writes into.
const ERROR_BUF_LEN: usize = 256;

/// A fresh directory for one test's files, removed with the guard.
struct ScratchDir(PathBuf);

impl ScratchDir {
    fn new(name: &str) -> Self {
        let dir = std::env::temp_dir().join(format!("mujoco-rs-{name}-{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        Self(dir)
    }
}

impl Drop for ScratchDir {
    fn drop(&mut self) {
        std::fs::remove_dir_all(&self.0).unwrap();
    }
}

#[test]
fn mjz_archive_round_trips_through_the_registered_codec() {
    let dir = ScratchDir::new("mjz");
    let path = dir.0.join("model.mjz");

    let mut spec = MjSpec::from_xml_string(
        "<mujoco><worldbody><body><geom size='.1'/><geom size='.2'/></body></worldbody></mujoco>",
    )
    .unwrap();
    spec.compile().unwrap();

    let c_path = CString::new(path.to_str().unwrap()).unwrap();
    let mut error = [0 as c_char; ERROR_BUF_LEN];
    // SAFETY: the spec is compiled, the path is NUL-terminated, and the error buffer length is
    // passed alongside it. Model, content type and VFS are documented nullable.
    let written = unsafe {
        mj_encode(
            spec.ffi(), ptr::null(), c_path.as_ptr(), ptr::null(), ptr::null(),
            error.as_mut_ptr(), ERROR_BUF_LEN as i32,
        )
    };
    let message = unsafe { CStr::from_ptr(error.as_ptr()) }.to_string_lossy();
    assert!(written > 0, "mj_encode failed: {message}");

    let decoded = MjSpec::from_parse(&path, "application/zip").unwrap().compile().unwrap();
    assert_eq!(decoded.ngeom(), 2);
}

/// A tetrahedron as a binary STL: an 80-byte header, the facet count, then per facet a normal,
/// three vertices (all `f32`) and a two-byte attribute word.
fn tetrahedron_stl() -> Vec<u8> {
    const VERTICES: [[f32; 3]; 4] = [[0., 0., 0.], [1., 0., 0.], [0., 1., 0.], [0., 0., 1.]];
    const FACETS: [[usize; 3]; 4] = [[0, 2, 1], [0, 1, 3], [0, 3, 2], [1, 2, 3]];
    let mut stl = vec![0u8; 80];
    stl.extend((FACETS.len() as u32).to_le_bytes());
    for facet in FACETS {
        stl.extend([0f32; 3].iter().flat_map(|c| c.to_le_bytes()));
        for vertex in facet {
            stl.extend(VERTICES[vertex].iter().flat_map(|c| c.to_le_bytes()));
        }
        stl.extend(0u16.to_le_bytes());
    }
    stl
}

/// The same tetrahedron as a Wavefront OBJ.
const TETRAHEDRON_OBJ: &str = "v 0 0 0\nv 1 0 0\nv 0 1 0\nv 0 0 1\nf 1 3 2\nf 1 2 4\nf 1 4 3\nf 2 3 4\n";

#[test]
fn obj_and_stl_meshes_load_through_the_registered_decoders() {
    let dir = ScratchDir::new("meshes");
    std::fs::write(dir.0.join("tet.obj"), TETRAHEDRON_OBJ).unwrap();
    std::fs::write(dir.0.join("tet.stl"), tetrahedron_stl()).unwrap();
    let xml = format!(
        "<mujoco>
            <compiler meshdir='{}'/>
            <asset>
                <mesh name='obj' file='tet.obj'/>
                <mesh name='stl' file='tet.stl'/>
            </asset>
            <worldbody>
                <geom type='mesh' mesh='obj'/>
                <geom type='mesh' mesh='stl' pos='2 0 0'/>
            </worldbody>
        </mujoco>",
        dir.0.display()
    );

    let model = MjModel::from_xml_string(&xml).unwrap();

    assert_eq!(model.nmesh(), 2);
    assert_eq!(model.mesh_vertnum(), &[4, 4]);
    assert_eq!(model.mesh_facenum(), &[4, 4]);
}
