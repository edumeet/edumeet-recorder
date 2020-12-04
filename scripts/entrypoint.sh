#!/usr/bin/env bash
pulseaudio -D --verbose --exit-idle-time=-1 --system --disallow-exit
/build/bin/edumeet-recorder
