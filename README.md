# Remote Keyboard

This is a demo of sending keyboard events over network.
And using uinput emulated keyboard to perform the action.

It shows up quite useful in some cases, have fun :)

## Usage

first run receiver on target device

```shell
nc -l -p <port> | sudo ./kev_writer
```

then run sender, `Ctrl-C` to quit, lol

```shell
sudo ./kev_reader <evdev> | nc <ip> <port>

# which is equivalent to 
stty -noecho && sudo cat <evdev> | nc <ip> <port>
```

Tips: you can find the event device corresponding to your keyboard in `/dev/input/by-path/` or `cat /proc/bus/input/devices`.
