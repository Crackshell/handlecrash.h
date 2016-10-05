# handlecrash.h

Single-header crash handler for C++ programs. Writes a crash dump with stack trace to `/tmp` and also outputs it to stdout.

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
