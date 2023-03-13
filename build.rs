extern crate cc;

fn main() {
    if cfg!(target_os = "macos") {
        cc::Build::new()
            .file("c/mousetune.c")
            .flag("-fmodules")
            .flag("-Wno-deprecated-declarations")
            .compile("mousetune");
    }
}
