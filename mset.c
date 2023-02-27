#define VERSION "0.1.1"

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
  UInt32 res;
  UInt32 acc;
} m_config;

typedef struct {
  UInt32 sen;
  UInt32 acc;
  bool quiet;
  bool has_invalid_args;
} input_args;

const CFStringRef K_POINTER_RES_KEY = CFSTR(kIOHIDPointerResolutionKey);
const CFStringRef K_MOUSE_ACCEL_KEY = CFSTR(kIOHIDMouseAccelerationType);

UInt32 sen_to_res(UInt32 sen) {
  return clamp(2000 - (sen * 10), 10, 1990) * 65536;
}

UInt32 res_to_sen(UInt32 res) {
  return clamp((2000 - (res / 65536)) / 10, 1, 199);
}

static m_config get() {
  NXEventHandle hdl = NXOpenEventStatus();

  UInt32 res = 0;
  UInt32 acc = 0;

  IOByteCount resByteSize = sizeof(res);
  IOByteCount accByteSize = sizeof(acc);

  IOHIDGetParameter(hdl, K_POINTER_RES_KEY, (IOByteCount)sizeof(res), &res, &resByteSize);
  IOHIDGetParameter(hdl, K_MOUSE_ACCEL_KEY, (IOByteCount)sizeof(acc), &acc, &accByteSize);

  NXCloseEventStatus(hdl);

  m_config cfg = {res, acc};

  return cfg;
}

static int set(m_config cfg) {
  NXEventHandle hdl = NXOpenEventStatus();

  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, K_POINTER_RES_KEY, &cfg.res, sizeof(cfg.res)));
  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, K_MOUSE_ACCEL_KEY, &cfg.acc, sizeof(cfg.acc)));

  NXCloseEventStatus(hdl);

  return 0;
}

static void print_usage(char *bin) {
  // One printf per line to make it easier to read and maintain
  printf("mset version %s\n\n", VERSION);
  printf("Usage: %s [-s <sensitivity>] [-a <acceleration>]\n\n", bin);
  printf("Options:\n");
  printf("-s\t\t\t - set mouse sensitivity, default is 190, range is 1-199\n");
  printf("-a\t\t\t - set mouse acceleration, default is 0, range is 0-10000000, 0 means disable acceleration\n\n");
  printf("Examples:\n");
  printf("%s\t\t\t # set sensitivity to 190 and disable acceleration\n", bin);
  printf("%s -s 180\t\t # set sensitivity to 180 and disable acceleration\n", bin);
  printf("%s -s 100 -a 50000\t # set sensitivity to 100 and acceleration to 50000\n", bin);
}

static int print_meta(int argc, char **argv) {
  if (argc == 1) {
    print_usage(argv[0]);
    return 0;
  }

  char *cmd = argv[1];

  if (strcmp(cmd, "-v") == 0) {
    printf("%s\n", VERSION);
    return 0;
  }

  if (strcmp(cmd, "-h") == 0 || strcmp(cmd, "--help") == 0) {
    print_usage("mset");
    return 0;
  }

  printf("Invalid argument: %s\n\n", cmd);
  print_usage("mset");
  return 1;
}

static input_args parse_args(int argc, char **argv) {
  input_args args = {190, 0, false, false};

  for (int idx = 1; idx < argc; idx++) {
    if (strcmp(argv[idx], "-s") == 0) {
      args.sen = atoi(argv[++idx]);
      assert(args.sen >= 1 && args.sen <= 199);
    } else if (strcmp(argv[idx], "-a") == 0) {
      args.acc = atoi(argv[++idx]);
      assert(args.acc >= 0 && args.acc <= 10000000);
    } else if (strcmp(argv[idx], "-q") == 0) {
      args.quiet = true;
    } else {
      printf("Invalid argument: %s\n\n", argv[idx]);
      print_usage(argv[0]);
      exit(1);
    }
  }

  return args;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    exit(print_meta(argc, argv));
  }

  input_args args = parse_args(argc, argv);
  m_config cfg = {sen_to_res(args.sen), args.acc};

  bool quiet = args.quiet;

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
