podman run -it --rm \
--security-opt label=disable \
--workdir /workspaces/zmk \
-v /Users/ramjeet.yadav/fafo/zmk:/workspaces/zmk \
-v /Users/ramjeet.yadav/fafo/zmk-config/:/workspaces/zmk-config \
-p 3000:3000 \
--name barebcon \
bareb /bin/bash


west build -p -b nice_nano_v2 -d build/right -s app/ -- -DSHIELD=xru_right -DZMK_CONFIG=/workspaces/zmk-config \
west build -p -b nice_nano_v2 -d build/left -s app/ -- -DSHIELD=xru_left -DZMK_CONFIG=/workspaces/zmk-config \
west build -p -b nice_nano_v2 -d build/reset -s app/ -- -DSHIELD=settings_reset -DZMK_CONFIG=/workspaces/zmk-config