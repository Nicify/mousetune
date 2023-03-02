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

#define VERSION "0.2.0"

#define clamp(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

typedef struct {
  uint32_t HIDPointerResolution;
  uint32_t HIDMouseAcceleration;
} HIDMouseParameters;

typedef struct {
  uint32_t sen;
  uint32_t acc;
  bool daemon;
  bool has_invalid_args;
} input_args;

const CFStringRef K_POINTER_RES_KEY = CFSTR(kIOHIDPointerResolutionKey);
const CFStringRef K_MOUSE_ACCEL_KEY = CFSTR(kIOHIDMouseAccelerationType);

uint32_t sen_to_res(uint32_t sen) {
  return clamp(2000 - (sen * 10), 10, 1990) * 65536;
}

uint32_t res_to_sen(uint32_t res) {
  return clamp((2000 - (res / 65536)) / 10, 1, 199);
}

static HIDMouseParameters get() {
  NXEventHandle hdl = NXOpenEventStatus();

  uint32_t HIDPointerResolution = 0;
  uint32_t HIDMouseAcceleration = 0;

  IOByteCount resByteSize = sizeof(HIDPointerResolution);
  IOByteCount accByteSize = sizeof(HIDMouseAcceleration);

  IOHIDGetParameter(hdl, K_POINTER_RES_KEY, (IOByteCount)sizeof(HIDPointerResolution), &HIDPointerResolution, &resByteSize);
  IOHIDGetParameter(hdl, K_MOUSE_ACCEL_KEY, (IOByteCount)sizeof(HIDMouseAcceleration), &HIDMouseAcceleration, &accByteSize);

  NXCloseEventStatus(hdl);

  return (HIDMouseParameters){HIDPointerResolution, HIDMouseAcceleration};
}

static int set(HIDMouseParameters parameters) {
  NXEventHandle hdl = NXOpenEventStatus();

  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, K_POINTER_RES_KEY, &parameters.HIDPointerResolution, sizeof(parameters.HIDPointerResolution)));
  assert(KERN_SUCCESS == IOHIDSetParameter(hdl, K_MOUSE_ACCEL_KEY, &parameters.HIDMouseAcceleration, sizeof(parameters.HIDMouseAcceleration)));

  NXCloseEventStatus(hdl);

  return 0;
}

static void print_usage(char *bin) {
  printf("mset version %s\n\n", VERSION);
  printf("Usage: %s [-s <sensitivity>] [-a <acceleration>]\n\n", bin);
  printf("Options:\n");
  printf("-s\t\t\t - set mouse sensitivity, default is 190, range is 1-199\n");
  printf("-a\t\t\t - set mouse acceleration, default is 0, range is 0-10000000, 0 means disable acceleration\n\n");
  printf("-d\t\t\t - run as daemon, will check and re-apply mouse settings if system settings are changed or affected by other programs\n");
  printf("-v\t\t\t - print version\n");
  printf("-h, --help\t\t - print this help\n\n");
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
    char *key = argv[idx];
    char *val = argv[++idx];
    // clang-format off
    strcmp(key, "-s") == 0 ? (args.sen = atoi(val), assert(args.sen >= 1 && args.sen <= 199)) :
    strcmp(key, "-a") == 0 ? (args.acc = atoi(val), assert(args.acc >= 0 && args.acc <= 10000000)) :
    strcmp(key, "-d") == 0 ? args.daemon = true :
    (printf("Invalid argument: %s\n\n", key), print_usage(argv[0]), exit(1));
    // clang-format on
  }

  return args;
}

int apply(uint32_t sen, uint32_t acc, bool check, bool verbose) {
  HIDMouseParameters cfg = {sen_to_res(sen), acc};

  HIDMouseParameters curr = get();

  if (verbose) {
    printf("prev: sen=%d acc=%d\n", res_to_sen(curr.HIDPointerResolution), curr.HIDMouseAcceleration);
  }

  if (check && curr.HIDPointerResolution == cfg.HIDPointerResolution && curr.HIDMouseAcceleration == cfg.HIDMouseAcceleration) {
    printf("No change, skip\n");
    return 0;
  }

  set(cfg);

  curr = get();

  if (verbose) {
    printf("curr: sen=%d acc=%d\n", res_to_sen(curr.HIDPointerResolution), curr.HIDMouseAcceleration);
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    return apply(190, 0, false, false);
  }

  if (argc < 3) {
    exit(print_meta(argc, argv));
  }

  input_args args = parse_args(argc, argv);

  apply(args.sen, args.acc, false, false);

  if (args.daemon) {
    while (true) {
      // sleep for 15 seconds
      usleep(15 * 1000 * 1000);

      apply(args.sen, args.acc, true, false);
    }
  }

  return 0;
}
