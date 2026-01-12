# rdseed

A command line program for generating random numbers using the x86 hardware `rdseed` instruction. This is a source of conditioned entropy samples, not a pseudo-random number generator. This means that subsequent bits can not be predicted from prior bits. You can safely concatenate generated values to produce a random value of arbitary bit-length.


See the [Intel DRNG Guide](https://www.intel.com/content/www/us/en/developer/articles/guide/intel-digital-random-number-generator-drng-software-implementation-guide.html).

```
$ rdseed
419121190766937331
$ rdseed --bits 16 --hex -n 3
366c
010f
efed
```

The `rdrand` instruction is not supported as it is closer to a pseudo-random number generator.

## Building

If using nix to manage dependencies, run `./scripts/build.sh`.
Without nix, install depdencies yourself (see `scripts/build.nix`) and run `bash scripts/build.sh`.

To specify an installation destination: `BIN_PREFIX=~/bin ./scripts/build.sh`.

## Documentation

Try `rdseed --help`.
