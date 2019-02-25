#!/usr/bin/env python3

import sys

levels = 'trace debug info warn error'.split()

color_codes = {
    '[1;30m': 0,
    '[0m': 1,
    '[1;36m': 2,
    '[1;33m': 3,
    '[1;31m': 4,
}

def main():
    level = 5
    if len(sys.argv) != 2 or sys.argv[1] not in levels:
        print(f'Usage: {sys.argv[0]} [{"|".join(levels)}]')
        return
    min_level = levels.index(sys.argv[1])

    def ch():
        c = sys.stdin.read(1)
        if level >= min_level or c == '\033':
            sys.stdout.write(c)
        return c

    while True:
        if ch() == '\033':
            level = 5
            buf = ''
            while True:
                buf += ch()
                if buf in color_codes:
                    level = color_codes[buf]
                    break
                if not any(k.startswith(buf) for k in color_codes):
                    break

if __name__ == '__main__':
    main()
