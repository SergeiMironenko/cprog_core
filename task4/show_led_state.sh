while true; do
    capslock=$(cat /sys/class/leds/input4::capslock/brightness);
    numlock=$(cat /sys/class/leds/input4::numlock/brightness);
    scrollock=$(cat /sys/class/leds/input4::scrolllock/brightness);
    echo "capslock = $capslock, numlock = $numlock, scrollock = $scrollock"
    sleep 1;
done
