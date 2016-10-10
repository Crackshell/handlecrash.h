# handlecrash.h

Single-header crash handler for C++ programs. Writes a crash dump with stack trace and compressed stack memory to `/tmp` and also outputs it to stdout.

Currently only supports Linux x86 and x64.

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
$ ~/handlecrash.h/is/decode.py /tmp/crash_1476112323.log
```
