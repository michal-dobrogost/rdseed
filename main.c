#include <errno.h>
#include <immintrin.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void printUsageAndExit(const char* arg0, const char* errMessage, int code) {
  fprintf(stderr, "Usage: %s [OPTION]\n", arg0);
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates random numbers using the hardware RDSEED instructions provided by x86 processors.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -b,--bits N        generate N-bit numbers (16, 32 or 64, default 64)\n");
  fprintf(stderr, "  -x,--hex           print in hexadecimal format (default decimal)\n");
  fprintf(stderr, "  -n,--num N         generate N different numbers (default 1)\n");
  fprintf(stderr, "  --retries N        retry N times on error (default 16)\n");
  fprintf(stderr, "  --retry-sleep NS   if non-zero sleep for NS nanoseconds upon retry (default 100000)\n");
  fprintf(stderr, "  --help\n");
  if (errMessage != NULL) {
    fprintf(stderr, "%s", errMessage);
  }
  exit(code);
}

typedef struct Args {
  int bits;
  bool hex;
  int num;
  int retries;
  int retrySleepNS;
} Args;

long long tryParseInt(const char* arg0, const char* from, const char* errMessage) {
  char* endPtr = NULL;
  long long result = strtoll(from, &endPtr, 10);
  if (errno != 0 || endPtr != from + strlen(from)) {
    printUsageAndExit(arg0, errMessage, 1);
  }
  return result;
}

Args parseArgs(const int argc, const char** argv) {
  Args args;
  args.bits = 64;
  args.hex = false;
  args.num = 1;
  args.retries = 16;
  args.retrySleepNS = 100000;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--help") == 0) {
      printUsageAndExit(argv[0], NULL, 0);
    }
    if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bits") == 0) {
      if (i + 1 >= argc) {
        printUsageAndExit(argv[0], "\nERROR: No argument for --bits\n", 1);
      }
      i++;
      args.bits = tryParseInt(argv[0], argv[i], "ERROR: Could not parse argument of --bits\n");
    }
    if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--hex") == 0) {
      args.hex = true;
    }
    if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--num") == 0) {
      if (i + 1 >= argc) {
        printUsageAndExit(argv[0], "\nERROR: No argument for --num\n", 1);
      }
      i++;
      args.num = tryParseInt(argv[0], argv[i], "ERROR: Could not parse argument of --num\n");
    }
    if (strcmp(argv[i], "--retries") == 0) {
      if (i + 1 >= argc) {
        printUsageAndExit(argv[0], "\nERROR: No argument for --retries\n", 1);
      }
      i++;
      args.retries = tryParseInt(argv[0], argv[i], "ERROR: Could not parse argument of --retries\n");
    }
    if (strcmp(argv[i], "--retry-sleep") == 0) {
      if (i + 1 >= argc) {
        printUsageAndExit(argv[0], "\nERROR: No argument for --retry-sleep\n", 1);
      }
      i++;
      args.retrySleepNS = tryParseInt(argv[0], argv[i], "ERROR: Could not parse argument of --retry-sleep\n");
    }
  }
  return args;
}

void sleep(unsigned long ns) {
  if (ns > 0) {
    struct timespec delay = { ns / 1000000000, ns % 1000000000 };
    pselect(0, NULL, NULL, NULL, &delay, NULL);
  }
}

int tryStepAndPrint(int bits, bool hex) {
  uint16_t value16;
  uint32_t value32;
  unsigned long long value64;
  switch (bits) {
    case 16:
      if (_rdrand16_step(&value16) == 1)  {
        if (hex) {
          printf("%04" PRIx16 "\n", value16);
        } else {
          printf("%" PRIu16 "\n", value16);
        }
        return 1;
      }
      break;
    case 32:
      if (_rdrand32_step(&value32) == 1) {
        if (hex) {
          printf("%08" PRIx32 "\n", value32);
        } else {
          printf("%" PRIu32 "\n", value32);
        }
        return 1;
      }
      break;
    case 64:
      if (_rdrand64_step(&value64) == 1) {
        if (hex) {
          printf("%016llx\n", value64);
        } else {
          printf("%llu\n", value64);
        }
        return 1;
      }
      break;
  }
  return 0;
}

int main(const int argc, const char** argv) {
  Args args = parseArgs(argc, argv);
  for (int i = 0; i < args.num; ++i) {
    for (int retry = 0; ; ++retry) {
      if (tryStepAndPrint(args.bits, args.hex) == 1) break;
      if (retry == args.retries) {
        fprintf(stderr, "ERROR: retry threshold exceeded");
        return 1;
      }
      sleep(args.retrySleepNS);
    }
  }
  return 0;
}
