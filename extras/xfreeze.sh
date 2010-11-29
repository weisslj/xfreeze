#!/bin/sh

lock_sysrq
lock_vtswitch
xfreeze --vtswitch --sysrq
lock_vtswitch u
lock_sysrq u
