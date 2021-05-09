#!/bin/sh

# smaller memory will decrease performance!
aws lambda update-function-configuration \
  --function-name passwords \
  --timeout 15 \
  --memory-size 256
