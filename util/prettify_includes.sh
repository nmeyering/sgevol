#!/bin/bash

prettify_includes.py --reserved-prefix sgevol --reserved-prefix sgevollib --reserved-prefix sge --reserved-prefix majutsu --reserved-prefix awl --reserved-prefix mizuiro --reserved-prefix fcppt $(find src -name '*.hpp' -o -name '*.cpp' | xargs) 2>>/tmp/prettify_errors
