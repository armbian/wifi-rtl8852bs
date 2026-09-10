# Snapshot of Armbian build framework, lib/functions/compilation/patch/drivers_network.sh
# (fetched from armbian/build main on 2026-09-10). Historical reference only --
# the live file in armbian/build is authoritative.

driver_rtw88() {

	# Upstream wireless RTW88 drivers
	# Quite a few kernel families have KERNEL_DRIVERS_SKIP listing this driver. If so, this won't even be called.

	if linux-version compare "${version}" ge 6.1; then
		display_alert "Adding" "Upstream wireless RTW88 drivers" "info"
		if [[ -f "${SRC}/patch/misc/rtw88/${version}/001-drivers-net-wireless-realtek-rtw88-upstream-wireless.patch" ]]; then
			process_patch_file "${SRC}/patch/misc/rtw88/${version}/001-drivers-net-wireless-realtek-rtw88-upstream-wireless.patch" "applying"
			process_patch_file "${SRC}/patch/misc/rtw88/hack/003-rtw88-decrease-the-log-level-of-tx-report.patch" "applying"
		fi
		display_alert "Added" "Upstream wireless RTW88 drivers" "info"
	fi

	if linux-version compare "${version}" eq 6.1; then
		process_patch_file "${SRC}/patch/misc/rtw88/hack/004-rtw88-sdio-rf-path-detection-fix.patch" "applying" # This patch is only for kernel 6.1.x. Not needed for 6.18+ (already upstream)
	fi
}

driver_rtl8852bs() {
	# Wireless driver for Realtek 8852BS SDIO Wireless driver used in BananaPi F3 and Armsom Sige5
	if linux-version compare "${version}" ge 6.1 && [[ "${LINUXFAMILY}" == spacemit || "${LINUXFAMILY}" == rk35xx || "${LINUXFAMILY}" == rockchip64 ]]; then

		# Attach to specific commit
		local rtl8852bs_ver='commit:643a4990752ad4761c2ad216901257adfd4fd8db' # Commit date: Sep 10, 2026 (please update when updating commit ref)

		display_alert "Adding" "Wireless drivers for Realtek 8852BS SDIO chipset ${rtl8852bs_ver}" "info"

		fetch_from_repo "$GITHUB_SOURCE/armbian/wifi-rtl8852bs" "rtl8852bs" "${rtl8852bs_ver}" "yes" # https://github.com/armbian/wifi-rtl8852bs
		cd "$kerneldir" || exit
		rm -rf "$kerneldir/drivers/net/wireless/realtek/rtl8852bs"
		mkdir -p "$kerneldir/drivers/net/wireless/realtek/rtl8852bs/"

		# Copy folders into kernel-work-dir
		cp -R "${SRC}/cache/sources/rtl8852bs/${rtl8852bs_ver#*:}"/{core,include,os_dep,phl,platform} \
			"$kerneldir/drivers/net/wireless/realtek/rtl8852bs"

		# Copy Kconfig into kernel-work-dir
		cp "${SRC}/cache/sources/rtl8852bs/${rtl8852bs_ver#*:}"/Kconfig \
			"$kerneldir/drivers/net/wireless/realtek/rtl8852bs/Kconfig"

		# Copy Makefile into kernel-work-dir
		cp "${SRC}/cache/sources/rtl8852bs/${rtl8852bs_ver#*:}"/Makefile \
			"$kerneldir/drivers/net/wireless/realtek/rtl8852bs/Makefile"

		# Copy common.mk into kernel-work-dir
		cp "${SRC}/cache/sources/rtl8852bs/${rtl8852bs_ver#*:}"/common.mk \
			"$kerneldir/drivers/net/wireless/realtek/rtl8852bs/common.mk"

		# Disable debug
		sed -i "s/^CONFIG_RTW_DEBUG.*/CONFIG_RTW_DEBUG = n/" \
			"$kerneldir/drivers/net/wireless/realtek/rtl8852bs/Makefile"

		# Add to section Makefile
		echo "obj-\$(CONFIG_RTL8852BS) += rtl8852bs/" >> "$kerneldir/drivers/net/wireless/realtek/Makefile"
		sed -i '/source "drivers\/net\/wireless\/realtek\/rtw89\/Kconfig"/a source "drivers\/net\/wireless\/realtek\/rtl8852bs\/Kconfig"' \
			"$kerneldir/drivers/net/wireless/realtek/Kconfig"

		# We have to enable specific platforms in the driver Makefile to enable specific driver tweaks, they are all "n" by default
		case ${LINUXFAMILY} in
			# For Rockchip devices, add family name here
			rk35xx | rockchip64)
				sed -i "s/CONFIG_PLATFORM_ARM_ROCKCHIP = n/CONFIG_PLATFORM_ARM_ROCKCHIP = y/g" "$kerneldir/drivers/net/wireless/realtek/rtl8852bs/Makefile"
				;;
			# For Spacemit devices, add family name here
			spacemit)
				sed -i "s/CONFIG_PLATFORM_SPACEMIT = n/CONFIG_PLATFORM_SPACEMIT = y/g" "$kerneldir/drivers/net/wireless/realtek/rtl8852bs/Makefile"
				;;
		esac
		display_alert "Done adding" "Wireless drivers for Realtek 8852BS SDIO chipset ${rtl8852bs_ver}" "info"
	else
		display_alert "Skipping" "Wireless drivers for Realtek 8852BS SDIO chipset" "info"
	fi
}
