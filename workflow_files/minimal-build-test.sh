# skip non-essential options to speed up tests

function armbian_kernel_config__enable_zram_support() {
    display_alert "Skip" "zram" "info"
}

function armbian_kernel_config__select_nftables() {
    display_alert "Skip" "nftables" "info"
}

function armbian_kernel_config__enable_docker_support() {
    display_alert "Skip" "docker support" "info"
}