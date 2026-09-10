# RTL8852BS

This is an out-of-tree driver for the Realtek RTL8852BS SDIO WiFi chipset.

It is continuously compile-tested for current and edge kernels of the Armbian
`spacemit` and `rockchip64` kernel families (see `.github/workflows/kernel-build.yml`),
which covers the SpacemiT K1 (e.g. BananaPi F3) and Rockchip RK3576/RK3399 boards
this chipset is found on. If your kernel supports RTL8852BS in the RTW89 driver,
please use RTW89 instead.

The original sources for this repopsitory are cloned from [BianbuLinux/linux-6.1](https://gitee.com/bianbu-linux/linux-6.1/tree/bl-v1.0.y/drivers/net/wireless/realtek/rtl8852bs)
and also includes commits from [vicharak-in/rockchip-linux-kernel](https://github.com/vicharak-in/rockchip-linux-kernel/tree/master/drivers/net/wireless/rockchip_wlan/rtl8852bs).

These sources have not been verified or audited. Use at your own risk!

## Armbian Integration

The Armbian build framework fetches this repository in-tree via the
`driver_rtl8852bs()` function in `lib/functions/compilation/patch/drivers_network.sh`,
pinned to a commit via `rtl8852bs_ver='commit:<sha>'`. A snapshot of that function
is kept in `reference/` for historical reference only — the live file in
[armbian/build](https://github.com/armbian/build) is authoritative.

After merging changes here, update the commit pin in armbian/build so images pick
up the new code (the pin's own comment asks for this too).

## CI

`.github/workflows/kernel-build.yml` builds Armbian kernels for bananapif3
(spacemit current/edge), nanopi-m5 (rockchip64 current/edge) and armsom-sige5
(rk35xx vendor) and fails unless `8852bs.ko` was produced. The kernel config fragments under
`workflow_files/` enable `CONFIG_RTL8852BS=m`; the spacemit ones are Armbian's
configs taken as-is, the rockchip64 ones are minimal compile-only configs.
