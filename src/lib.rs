#![warn(trivial_casts, trivial_numeric_casts)]
#![cfg(target_os = "macos")]
#![allow(improper_ctypes)]

mod sys {
    use libc::c_int;
    #[link(name = "mousetune")]
    extern "C" {
        pub fn getPointerResolution() -> c_int;
        pub fn getMouseAcceleration() -> c_int;
        pub fn setPointerResolution(res: c_int) -> c_int;
        pub fn setMouseAcceleration(acc: c_int) -> c_int;
    }
}

pub fn get_pointer_resolution() -> i32 {
    unsafe { sys::getPointerResolution() }
}

pub fn get_mouse_acceleration() -> i32 {
    unsafe { sys::getMouseAcceleration() }
}

pub fn set_pointer_resolution(res: i32) -> bool {
    unsafe { sys::setPointerResolution(res) == 0 }
}

pub fn set_mouse_acceleration(acc: i32) -> bool {
    unsafe { sys::setMouseAcceleration(acc) == 0 }
}
