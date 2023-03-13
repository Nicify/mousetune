use mac_mousetune_sys::*;

#[test]
fn resolution() {
    let val = 655360;

    set_pointer_resolution(val);

    let ret = get_pointer_resolution();

    assert_eq!(ret, val);
}

#[test]
fn acceleration() {
    let val = 1234;

    set_mouse_acceleration(val);

    let ret = get_mouse_acceleration();

    assert_eq!(ret, val);
}
