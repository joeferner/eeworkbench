#!/bin/bash

astyle \
  --style=java \
  --indent=spaces=2 \
  --suffix=none \
  --add-brackets \
  --keep-one-line-blocks \
  --align-pointer=type \
  --align-reference=type \
  --pad-oper \
  --unpad-paren \
  --recursive '*.cpp' '*.h'
