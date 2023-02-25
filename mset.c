#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <IOKit/hidsystem/event_status_driver.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
  uint32_t res;
  uint32_t acc;
} m_config;

uint32_t sen_to_res(uint32_t sen) {
  return (2000 - (sen * 10)) * 65536;
}

m_config get() {
  CFStringRef kMouseAccelKey = CFSTR(kIOHIDMouseAccelerationType);
  CFStringRef kPointerResKey = CFSTR(kIOHIDPointerResolutionKey);

  NXEventHandle hdl = NXOpenEventStatus();

  uint32_t res = 0;
  uint32_t acc = 0;

  IOByteCount resByteSize = sizeof(res);
  IOByteCount accByteSize = sizeof(acc);

  assert(KERN_SUCCESS == IOHIDGetParameter(hdl, kPointerResKey, (IOByteCount)sizeof(res), &res, &resByteSize));
  assert(KERN_SUCCESS == IOHIDGetParameter(hdl, kMouseAccelKey, (IOByteCount)sizeof(acc), &acc, &accByteSize));

  NXCloseEventStatus(hdl);

  m_config cfg = {res, acc};

  return cfg;
}

int set(m_config cfg) {
  CFStringRef kMouseAccelKey = CFSTR(kIOHIDMouseAccelerationType);
  CFStringRef kPointerResKey = CFSTR(kIOHIDPointerResolutionKey);

  NXEventHandle hdl = NXOpenEventStatus();

  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, kPointerResKey, &cfg.res, sizeof(cfg.res)));
  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, kMouseAccelKey, &cfg.acc, sizeof(cfg.acc)));

  NXCloseEventStatus(hdl);

  return 0;
}

int main(int argc, char **argv) {
  uint32_t sen = 190;
  uint32_t acc = 0;
  uint32_t res = sen_to_res(sen);
  m_config cfg = {res, acc};

  if (argc == 1) {
    return set(cfg);
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    printf(
      "A cli tool for setting system-wide mouse sensitivity and acceleration on macOS.\n\nUsage:\n mset [sensitivity] [acceleration]\n\nExample:\n\n# Set sensitivity to 190 and disable acceleration. \nmset 190 0\n\n# Set sensitivity to 100 and acceleration to 30000. \nmset 100 30000\n");
    return 0;
  }

  if (argc == 2) {
    sen = atoi(argv[1]);

    assert(sen >= 1 && sen <= 199);
    cfg.res = sen_to_res(sen);
    return set(cfg);
  }

  if (argc == 3) {
    sen = atoi(argv[1]);
    acc = atoi(argv[2]);
    assert(sen >= 1 && sen <= 199);
    assert(acc >= 0 && acc <= 10000000);
    cfg.res = sen_to_res(sen);
    cfg.acc = acc;
    return set(cfg);
  }

  return 1;
}
