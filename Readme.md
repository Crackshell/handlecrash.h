# handlecrash.h

Single-header* crash handler for C++ programs. Writes a crash dump with stack trace and compressed stack memory to `/tmp` and also outputs it to stdout.

Currently only supports Linux x86 and x64.

*: Relies on miniz.c for compression. However, see the options below on how to exclude this dependency.

## Usage

Include `handlecrash.h` in 1 file and install the handler by calling `hc_install()`.

## Example

```C++
#include "handlecrash.h"

int main()
{
	hc_install();

	int* foo = (int*)0;
	*foo = 1;

	return 0;
}
```

Then, to decode a crash report:
```
$ cd folder/with/debug/bins
$ ~/handlecrash.h/decode.py /tmp/crash_1476112323.log
```

## Options

* Define `HC_NO_COMPRESSION` before including to remove the dependency on miniz and disable compression.
* Define `HC_ALTSTACK_SIZE` before including to change the size of the altstack for the signal handler. This should be at least something like 100+ KB if you are using stack memory compression. The default is `MINSIGSTKSZ + 135 * 1000` which works fine for the default stack memory compression options.
* Define `HC_LOG_DESTINATION` before including to change where crash logs are written to. The default is `"/tmp"` which results in logs like `/tmp/crash_1476112323.log`.
