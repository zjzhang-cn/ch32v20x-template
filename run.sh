#!/bin/bash
docker run --rm -it --net=host --privileged -v $PWD:$PWD -w $PWD zjzhang/wch:latest "$@"