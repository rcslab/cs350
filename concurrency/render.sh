#!/bin/sh

rm page*.pdf
rm page*.jpg

pdfseparate concurrency.pdf page%d.pdf
find ./ -name "page*.pdf" | cut -f 1,2 -d "." | xargs -I PAGE pdftoppm -jpeg -scale-to-x 3840 -scale-to-y 2160 PAGE.pdf PAGE
