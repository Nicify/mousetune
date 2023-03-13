use mac_mousetune_sys::*;

fn main() {
    let val = 655360;

    set_pointer_resolution(val);

    let ret = get_pointer_resolution();

    assert_eq!(ret, val);
}
