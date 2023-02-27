#define VERSION "0.1.0"

#include "mset.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <IOKit/hidsystem/event_status_driver.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  uint32_t res;
  uint32_t acc;
} m_config;

const CFStringRef K_POINTER_RES_KEY = CFSTR(kIOHIDPointerResolutionKey);
const CFStringRef K_MOUSE_ACCEL_KEY = CFSTR(kIOHIDMouseAccelerationType);

uint32_t sen_to_res(uint32_t sen) {
  return clamp(2000 - (sen * 10), 10, 1990) * 65536;
}

uint32_t res_to_sen(uint32_t res) {
  return clamp((2000 - (res / 65536)) / 10, 1, 199);
}

void usage(char *bin) {
  printf(
    "mset version %s\n\nUsage: %s [-s <sensitivity>] [-a <acceleration>]\n\nOptions:\n-s \t\t\t - set mouse sensitivity, default is 190, range is 1-199\n-a\t\t\t - set mouse acceleration, default is 0, range is 0-10000000, 0 means disable acceleration\n\nExamples:\n%s\t\t\t # set sensitivity to 190 and disable acceleration\n%s -s 180\t\t # set sensitivity to 180 and disable acceleration\n%s -s 100 -a 50000\t # set sensitivity to 100 and acceleration to 50000\n",
    VERSION,
    bin,
    bin,
    bin,
    bin);
}

m_config get() {
  NXEventHandle hdl = NXOpenEventStatus();

  uint32_t res = 0;
  uint32_t acc = 0;

  IOByteCount resByteSize = sizeof(res);
  IOByteCount accByteSize = sizeof(acc);

  IOHIDGetParameter(hdl, K_POINTER_RES_KEY, (IOByteCount)sizeof(res), &res, &resByteSize);
  IOHIDGetParameter(hdl, K_MOUSE_ACCEL_KEY, (IOByteCount)sizeof(acc), &acc, &accByteSize);

  NXCloseEventStatus(hdl);

  m_config cfg = {res, acc};

  return cfg;
}

int set(m_config cfg) {
  NXEventHandle hdl = NXOpenEventStatus();

  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, K_POINTER_RES_KEY, &cfg.res, sizeof(cfg.res)));
  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, K_MOUSE_ACCEL_KEY, &cfg.acc, sizeof(cfg.acc)));

  NXCloseEventStatus(hdl);

  return 0;
}

int main(int argc, char **argv) {
  char *bin = argv[0];
  uint32_t sen = 190;
  uint32_t acc = 0;
  uint32_t res = sen_to_res(sen);
  m_config cfg = {res, acc};
  uint32_t idx = 0;

  bool quiet = false;
  bool has_invalid_arg = false;

  if (argc == 2) {
    char *arg = argv[1];
    if (strcmp(arg, "-v") == 0) {
      printf("%s\n", VERSION);
      return 0;
    }

    usage(bin);
    return 0;
  }

  for (idx = 1; idx < argc; idx++) {
    if (argv[idx][0] != '-') {
      has_invalid_arg = true;
      break;
    }
    switch (argv[idx][1]) {
      case 's':
        sen = atoi(argv[++idx]);
        assert(sen >= 1 && sen <= 199);
        res = sen_to_res(sen);
        cfg.res = res;
        break;
      case 'a':
        acc = atoi(argv[++idx]);
        assert(acc >= 0 && acc <= 10000000);
        cfg.acc = acc;
        break;
      case 'q':
        quiet = true;
        break;
      default:
        has_invalid_arg = true;
        break;
    }
  }

  if (has_invalid_arg) {
    printf("Invalid argument: %s\n\n", argv[idx]);
    usage(bin);
    return 1;
  }

  m_config curr = get();

  if (!quiet) {
    printf("prev: sen=%d acc=%d\n", res_to_sen(curr.res), curr.acc);
  }

  set(cfg);

  curr = get();

  if (!quiet) {
    printf("curr: sen=%d acc=%d\n", res_to_sen(curr.res), curr.acc);
  }

  return 0;
}
