//! Integration test for the two writers of the last loaded XML: [`MjModel::save_last_xml`] and
//! the XML encoder of [`MjModel::encode`].
//!
//! MuJoCo keeps the spec of the last XML load in a process-wide slot and writes that spec. It
//! fails the write when the model was not compiled from it, so a load in a parallel test makes
//! the write fail. This binary therefore holds one test, which no other test of the process runs
//! beside.

use std::fs;

use mujoco_rs::wrappers::mj_auxiliary::MjVfs;
use mujoco_rs::prelude::*;

const SAVED_MODEL: &str = "<mujoco><worldbody><body><joint type='hinge'/><geom size='.1'/></body></worldbody></mujoco>";
const OTHER_MODEL: &str = "<mujoco><worldbody><geom size='.2'/><geom size='.3'/></worldbody></mujoco>";

#[test]
fn only_the_model_of_the_last_loaded_xml_writes_it() {
    let dir = std::env::temp_dir().join(format!("mujoco-rs-last-xml-{}", std::process::id()));
    fs::create_dir_all(&dir).unwrap();
    let saved_path = dir.join("saved.xml");
    let encoded_path = dir.join("encoded.xml");

    let model = MjModel::from_xml_string(SAVED_MODEL).unwrap();
    model.save_last_xml(&saved_path).unwrap();
    model.encode_with_vfs(&encoded_path, "text/xml", &MjVfs::new()).unwrap();
    assert!(model.save_last_xml("/some/non-existent/path/").is_err());

    // Both files hold the model they were written from.
    for path in [&saved_path, &encoded_path] {
        let reloaded = MjModel::from_xml(path).unwrap();
        assert!(model.is_compatible_with_model(&reloaded), "{}", path.display());
    }

    // Once another XML is loaded, the first model writes neither file.
    let _other = MjModel::from_xml_string(OTHER_MODEL).unwrap();
    assert!(matches!(model.save_last_xml(&saved_path), Err(MjModelError::SaveFailed(_))));
    assert!(matches!(model.encode(&encoded_path, "text/xml"), Err(MjModelError::SaveFailed(_))));

    fs::remove_dir_all(&dir).unwrap();
}
