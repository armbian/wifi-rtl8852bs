<h2 align="center">
  <a href=#><img src="https://raw.githubusercontent.com/armbian/.github/master/profile/logosmall.png" alt="Armbian logo"></a>
  <br><br>
</h2>

# rtl8852bs

## Purpose of This Repository

Out-of-tree Linux kernel driver sources for the Realtek RTL8852BS SDIO Wi-Fi 6 chipset, packaged for consumption by the Armbian build system.

## Origin

The sources in this repository are cloned from
[BianbuLinux/linux-6.1](https://gitee.com/bianbu-linux/linux-6.1/tree/bl-v1.0.y/drivers/net/wireless/realtek/rtl8852bs)
and additionally include commits from
[vicharak-in/rockchip-linux-kernel](https://github.com/vicharak-in/rockchip-linux-kernel/tree/master/drivers/net/wireless/rockchip_wlan/rtl8852bs).

These sources have not been independently verified or audited. Use at your own risk.

If your kernel already supports the RTL8852BS through the mainline `rtw89` driver, prefer `rtw89` over this out-of-tree driver.

## Hardware Target

- Realtek RTL8852BS (Wi-Fi 6, SDIO host interface)
- Resulting module name (per `Makefile`): `8852bs`

The `Makefile` enables `CONFIG_RTL8852B = y` and `CONFIG_SDIO_HCI = y` by default. Other Realtek chip and bus variants (`RTL8852A`, `RTL8852BP`, `RTL8851B`, `RTL8852C`; USB / PCI / GSPI) are present in the tree but disabled in this configuration.

## Repository Layout

```
Kconfig            Kconfig entry for the driver
Makefile           Kbuild makefile with feature switches (chip, bus, options)
common.mk          Shared build fragment included by Makefile
LICENSE            GNU GPL v2
README.md          This file
ifcfg-wlan0        Sample network interface configuration
include/           Public/internal driver headers (osdep, ieee80211, PHL, ...)
core/              Core driver: MLME, xmit/recv, security/crypto, P2P, mesh, ...
  crypto/          AES-CCM/GCM/SIV, SHA-256, wrappers
  mesh/            802.11s mesh support
  monitor/         Monitor-mode / radiotap
  wds/             WDS support
os_dep/            OS abstraction layer
  linux/           Linux glue (cfg80211, sdio_intf, procfs, netlink, ...)
platform/          Platform-specific glue
phl/               Realtek PHL (PHY/HAL) layer
  custom/          Vendor customization hooks
  hal_g6/          "G6" HAL sources
    btc/           Bluetooth coexistence
    efuse/         eFuse access
    mac/           MAC layer, including mac_ax/ and 8852B-specific bits
```

Only entries confirmed to exist in this repository are listed.

## Building

This driver is not built by users of this repository directly. In Armbian, `armbian/build` copies these sources into a kernel source tree under `drivers/net/wireless/realtek/rtl8852bs` and builds them **in-tree** as part of the kernel package, rather than as an external module. The build system also overrides some defaults from the `Makefile` (for example, setting `CONFIG_RTW_DEBUG = n` and enabling the appropriate platform switch for the target SoC family).

Selected build-time switches available in `Makefile`:

| Switch | Default | Purpose |
|---|---|---|
| `CONFIG_RTL8852B` | `y` | Target chip |
| `CONFIG_SDIO_HCI` | `y` | Host interface |
| `CONFIG_MP_INCLUDED` | `y` | Manufacturing test support |
| `CONFIG_POWER_SAVE` | `y` | Power-save features |
| `CONFIG_BTC` | `y` | Bluetooth coexistence |
| `CONFIG_WIFI_6` | `y` | 802.11ax features |
| `CONFIG_80211W` | `y` | Management frame protection |
| `CONFIG_RTW_NAPI` / `CONFIG_RTW_GRO` | `y` | Networking offloads |
| `CONFIG_LAYER2_ROAMING` | `y` | Roaming support |
| `CONFIG_WOWLAN` | `n` | Wake-on-WLAN |
| `CONFIG_80211BE_EHT` | `n` | 802.11be (EHT/Wi-Fi 7) |
| `CONFIG_RTW_DEBUG` | `y` | Debug logging (Armbian sets `n`) |

See `Makefile` and `common.mk` for the complete list.

## License

Released under the GNU General Public License, version 2. See [`LICENSE`](LICENSE).

## Related Projects

- Armbian project: <https://www.armbian.com>
- Armbian documentation: <https://docs.armbian.com>
- Armbian build system: <https://github.com/armbian/build>
